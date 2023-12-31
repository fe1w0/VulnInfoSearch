import Metrics

/// Vapor's main `Responder` type. Combines configured middleware + router to create a responder.
internal struct DefaultResponder: Responder {
    private let router: TrieRouter<CachedRoute>
    private let notFoundResponder: Responder

    private struct CachedRoute {
        let route: Route
        let responder: Responder
    }

    /// Creates a new `ApplicationResponder`
    public init(routes: Routes, middleware: [Middleware] = []) {
        let options = routes.caseInsensitive ?
            Set(arrayLiteral: TrieRouter<CachedRoute>.ConfigurationOption.caseInsensitive) : []
        let router = TrieRouter(CachedRoute.self, options: options)
        
        for route in routes.all {
            // Make a copy of the route to cache middleware chaining.
            let cached = CachedRoute(
                route: route,
                responder: middleware.makeResponder(chainingTo: route.responder)
            )
            // remove any empty path components
            let path = route.path.filter { component in
                switch component {
                case .constant(let string):
                    return string != ""
                default:
                    return true
                }
            }
            router.register(cached, at: [.constant(route.method.string)] + path)
        }
        self.router = router
        self.notFoundResponder = middleware.makeResponder(chainingTo: NotFoundResponder())
    }

    /// See `Responder`
    public func respond(to request: Request) -> EventLoopFuture<Response> {
        let startTime = DispatchTime.now().uptimeNanoseconds
        let response: EventLoopFuture<Response>
        if let cachedRoute = self.getRoute(for: request) {
            request.route = cachedRoute.route
            response = cachedRoute.responder.respond(to: request)
        } else {
            response = self.notFoundResponder.respond(to: request)
        }
        return response.always { result in
            let status: HTTPStatus
            switch result {
            case .success(let response):
                status = response.status
            case .failure:
                status = .internalServerError
            }
            self.updateMetrics(
                for: request,
                startTime: startTime,
                statusCode: status.code
            )
        }
    }
    
    /// Gets a `Route` from the underlying `TrieRouter`.
    private func getRoute(for request: Request) -> CachedRoute? {
        let pathComponents = request.url.path
            .split(separator: "/")
            .map(String.init)
        
        let method = (request.method == .HEAD) ? .GET : request.method
        return self.router.route(
            path: [method.string] + pathComponents,
            parameters: &request.parameters
        )
    }

    /// Records the requests metrics.
    private func updateMetrics(
        for request: Request,
        startTime: UInt64,
        statusCode: UInt
    ) {
        let pathForMetrics: String
        if let route = request.route {
            // We don't use route.description here to avoid duplicating the method in the path
            pathForMetrics = "/\(route.path.map { "\($0)" }.joined(separator: "/"))"
        } else {
            // If the route is undefined (i.e. a 404 and not something like /users/:userID
            // We rewrite the path and the method to undefined to avoid DOSing the
            // application and any downstream metrics systems. Otherwise an attacker
            // could spam the service with unlimited requests and exhaust the system
            // with unlimited timers/counters
            pathForMetrics = "vapor_route_undefined"
        }
        let methodForMetrics: String
        if request.route == nil {
            methodForMetrics = "undefined"
        } else {
            methodForMetrics = request.method.string
        }
        let dimensions = [
            ("method", methodForMetrics),
            ("path", pathForMetrics),
            ("status", statusCode.description),
        ]
        Counter(label: "http_requests_total", dimensions: dimensions).increment()
        if statusCode >= 500 {
            Counter(label: "http_request_errors_total", dimensions: dimensions).increment()
        }
        Timer(
            label: "http_request_duration_seconds",
            dimensions: dimensions,
            preferredDisplayUnit: .seconds
        ).recordNanoseconds(DispatchTime.now().uptimeNanoseconds - startTime)
    }
}

private struct NotFoundResponder: Responder {
    func respond(to request: Request) -> EventLoopFuture<Response> {
        request.eventLoop.makeFailedFuture(RouteNotFound())
    }
}

struct RouteNotFound: Error { }

extension RouteNotFound: AbortError {
    static var typeIdentifier: String {
        "Abort"
    }
    
    var status: HTTPResponseStatus {
        .notFound
    }
}

extension RouteNotFound: DebuggableError {
    var logLevel: Logger.Level { 
        .debug
    }
}
