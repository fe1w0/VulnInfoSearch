NAN_METHOD(XmlDocument::FromHtml)
{
    Nan::HandleScope scope;

    v8::Local<v8::Object> options = Nan::To<v8::Object>(info[1]).ToLocalChecked();
    v8::Local<v8::Value>  baseUrlOpt  = Nan::Get(options,
        Nan::New<v8::String>("baseUrl").ToLocalChecked()).ToLocalChecked();
    v8::Local<v8::Value>  encodingOpt = Nan::Get(options,
        Nan::New<v8::String>("encoding").ToLocalChecked()).ToLocalChecked();
    v8::Local<v8::Value> excludeImpliedElementsOpt = Nan::Get(options,
        Nan::New<v8::String>("excludeImpliedElements").ToLocalChecked()).ToLocalChecked();

    // the base URL that will be used for this HTML parsed document
    Nan::Utf8String baseUrl_(Nan::To<v8::String>(baseUrlOpt).ToLocalChecked());
    const char * baseUrl = *baseUrl_;
    if (!baseUrlOpt->IsString()) {
        baseUrl = NULL;
    }

    // the encoding to be used for this document
    // (leave NULL for libxml to autodetect)
    Nan::Utf8String encoding_(Nan::To<v8::String>(encodingOpt).ToLocalChecked());
    const char * encoding = *encoding_;

    if (!encodingOpt->IsString()) {
        encoding = NULL;
    }

    v8::Local<v8::Array> errors = Nan::New<v8::Array>();
    xmlResetLastError();
    xmlSetStructuredErrorFunc(reinterpret_cast<void*>(&errors), XmlSyntaxError::PushToArray);

    int opts = (int)getParserOptions(options);
    if (Nan::To<v8::Boolean>(excludeImpliedElementsOpt).ToLocalChecked()->Value())
        opts |= HTML_PARSE_NOIMPLIED | HTML_PARSE_NODEFDTD;

    htmlDocPtr doc;
    if (!node::Buffer::HasInstance(info[0])) {
        // Parse a string
        Nan::Utf8String str(Nan::To<v8::String>(info[0]).ToLocalChecked());
        doc = htmlReadMemory(*str, str.length(), baseUrl, encoding, opts);
    }
    else {
        // Parse a buffer
        v8::Local<v8::Object> buf = Nan::To<v8::Object>(info[0]).ToLocalChecked();
        doc = htmlReadMemory(node::Buffer::Data(buf), node::Buffer::Length(buf),
                            baseUrl, encoding, opts);
    }

    xmlSetStructuredErrorFunc(NULL, NULL);

    if (!doc) {
        xmlError* error = xmlGetLastError();
        if (error) {
            return Nan::ThrowError(XmlSyntaxError::BuildSyntaxError(error));
        }
        return Nan::ThrowError("Could not parse XML string");
    }

    v8::Local<v8::Object> doc_handle = XmlDocument::New(doc);
    Nan::Set(doc_handle, Nan::New<v8::String>("errors").ToLocalChecked(), errors);

    // create the xml document handle to return
    return info.GetReturnValue().Set(doc_handle);
}