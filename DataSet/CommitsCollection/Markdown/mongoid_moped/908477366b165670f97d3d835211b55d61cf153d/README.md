Moped is a MongoDB driver for Ruby, which exposes a simple, elegant, and fast
API.

```ruby
session = Moped::Session.new %w[127.0.0.1:27017]
session.use "echo_test"

session.with(safe: true) do |safe|
  safe[:artists].insert(name: "Syd Vicious"
end

session[:artists].find(name: "Syd Vicious").
  update(
    :$push => { instruments: { name: "Bass" } }
  )
```

# Project Breakdown

Moped is composed of three parts: an implementation of the [BSON
specification](http://bsonspec.org/), an implementation of the [Mongo Wire
Protocol](http://www.mongodb.org/display/DOCS/Mongo+Wire+Protocol), and the
driver itself. An overview of the first two follows now, and after that more
information about the driver.

## Moped::BSON

`Moped::BSON` is the namespace for Moped's BSON implementation. It's
implemented in pure (but fast) ruby. The public entry point into the BSON
module is `BSON::Document`, which is just subclass of `Hash`, but exposes two
class methods: `serialize` and `deserialize`. `serialize` accepts a
BSON::Document (or Hash) and returns the serialized BSON representation.
`deserialize` does the opposite: it reads data from an IO-like input and
returns a deserialized BSON::Document.

### Moped::BSON::ObjectId

The `ObjectId` class is used for generating and interacting with Mongo's ids.

```ruby
id = Moped::BSON::ObjectId.new # => 4f8583b5e5a4e46a64000002
id.generation_time # => 2012-04-11 13:14:29 UTC
id == Moped::BSON::ObjectId.from_string(id.to_s) # => true
```

### Moped::BSON::Code

The `Code` class is used for working with javascript on the server.

```ruby
Moped::BSON::Code.new("function () { return this.name }")
Moped::BSON::Code.new("function (s) { return s.prefix + this.name }",
  prefix: "_"
)
```

### Moped::BSON::Binary

The `Binary` class allows you to persist binary data to the server, and
supports the following types: `:generic`, `:function`, `:old`, `:uuid`, `:md5`,
and `:user`. Note that `:old` is deprecated, but still present to support
legacy data.

```ruby
Moped::BSON::Binary.new(:md5, Digest::MD5.digest(__FILE__))
```

## Moped::Protocol

`Moped::Protocol` is the namespace for Moped's implementation of the Mongo Wire
Protocol. Its public API consists of classes representing each type of message
in the protocol: `Delete`, `GetMore`, `Insert`, `KillCursors`, `Query`,
`Reply`, `Update`, and a convenience class `Command`.

You should never have to worry about protocol objects, but more details can be
found in the API documentation if you're interested.

# Driver API

This is the core, public API for Moped. It lives almost entirely in four classes:

* `Session`: the root object for all interactions with mongo (c.f., `db` in the
  mongo shell).
* `Collection`: for working with collections in the context of a session
* `Indexes`: for manipulating and inspecting a collection's indexes
* `Query`: for querying, as well as modifying existing data.

What follows is a "whirlwind" overview of the Moped driver API. For details on
additional options, and more examples, use the [generated API
docs](http://rubydoc.info/github/mongoid/moped/master/frames).

## Session

### Example
```ruby
session = Moped::Session.new %w[127.0.0.1:27017 127.0.0.1:27018 127.0.0.1:27019]
session.use :moped_test
session.command ping: 1 # => {"ok"=>1.0}

session.with(safe: { w: 2, wtimeout: 5 }) do |safe_session|
  safe_session[:users].find.remove_all
end

session.with(database: "important_db", consistency: :strong) do |session|
  session[:users].find.one
end
```

### API
<table><tbody>

<tr><th>use</th>
<td>Set the current database<br>
<code>session.use :my_app_test</code></td></tr>

<tr><th>with</th>
<td>Return or yield a copy of session with different options.<br>
<code>session.with(safe: true) { |s| ... }</code><br>
<code>session.with(database: "admin").command(...)</code></td></tr>

<tr><th>[]</th>
<td>Choose a collection in the current database.<br>
<code>session[:people]</code></td></tr>

<tr><th>drop</th>
<td>Drop the current database<br>
<code>session.drop</code></td></tr>

<tr><th>command</th>
<td>Run a command on the current database.<br>
<code>session.command(ping: 1)</code></td></tr>

<tr><th>login</th>
<td>Log in to the current database.<br>
<code>session.login(username, password)</code></td></tr>

<tr><th>logout</th>
<td>Log out from the current database.<br>
<code>session.logout</code></td></tr>

</tbody></table>

## Collection

### Example
```ruby
users = session[:users]
users.drop
users.find.count # => 0.0

users.indexes.create({name: 1}, {unique: true})

users.insert(name: "John")
users.find.count # => 1.0

users.insert(name: "John")
users.find.count # => 1.0

session.with(safe: true) do |session|
  session[:users].insert(name: "John")
end # raises Moped::Errors::OperationFailure
```

### API
<table><tbody>

<tr><th>drop</th>
<td>Drop the collection<br>
<code>users.drop</code></td></tr>

<tr><th>indexes</th>
<td>Access information about this collection's indexes<br>
<code>users.indexes</code></td></tr>

<tr><th>find</th>
<td>Build a query on the collection<br>
<code>users.find(name: "John")</code></td></tr>

<tr><th>insert</th>
<td>Insert one or multiple documents.<br>
<code>users.insert(name: "John")</code><br>
<code>users.insert([{name: "John"}, {name: "Mary"}])</code></td></tr>

</tbody></table>

## Index

### Example
```ruby
session[:users].indexes.create(name: 1)
session[:users].indexes.create(
  { name: 1, location: "2d" },
  { unique: true }
)
session[:users].indexes[name: 1]
# => {"v"=>1, "key"=>{"name"=>1}, "ns"=>"moped_test.users", "name"=>"name_1" }

session[:users].indexes.drop(name: 1)
session[:users].indexes[name: 1] # => nil
```

### API
<table><tbody>

<tr><th>[]</th>
<td>Get an index by its spec.<br>
<code>indexes[id: 1]</code></td></tr>

<tr><th>create</th>
<td>Create an index<br>
<code>indexes.create({name: 1}, {unique: true})</code></td></tr>

<tr><th>drop</th>
<td>Drop one or all indexes<br>
<code>indexes.drop</code><br>
<code>indexes.drop(name: 1)</code></td></tr>

<tr><th>each</th>
<td>Yield each index<br>
<code>indexes.each { |idx| }</code></td></tr>

</tbody></table>

## Query

### Example
```ruby
users = session[:users]

users.insert(name: "John")
users.find.count # => 1

users.find(name: "Mary").upsert(name: "Mary")
users.find.count # => 2

users.find.skip(1).limit(1).sort(name: -1).one
# => {"_id" => <...>, "name" => "John" }

scope = users.find(name: "Mary").select(_id: 0, name: 1)
scope.one # => {"name" => "Mary" }
scope.remove
scope.one # nil
```

### API
<table><tbody>

<tr><th>limit</th>
<td>Set the limit for this query.<br>
<code>query.limit(5)</code></td></tr>

<tr><th>skip</th>
<td>Set the offset for this query.<br>
<code>query.skip(5)</code></td></tr>

<tr><th>sort</th>
<td>Sort the results of the query<br>
<code>query.sort(name: -1)</code></td></tr>

<tr><th>distinct</th>
<td>Get the distinct values for a field.<br>
<code>query.distinct(:name)</code></td></tr>

<tr><th>select</th>
<td>Select a set of fields to return.<br>
<code>query.select(_id: 0, name: 1)</code></td></tr>

<tr><th>one/first</th>
<td>Return the first result from the query.<br>
<code>query.one</code></td></tr>

<tr><th>each</th>
<td>Iterate through the results of the query.<br>
<code>query.each { |doc| }</code></td></tr>

<tr><th>count</th>
<td>Return the number of documents matching the query.<br>
<code>query.count</code></td></tr>

<tr><th>update</th>
<td>Update the first document matching the query's selector.<br>
<code>query.update(name: "John")</code></td></tr>

<tr><th>update_all</th>
<td>Update all documents matching the query's selector.<br>
<code>query.update_all(name: "John")</code></td></tr>

<tr><th>upsert</th>
<td>Create or update a document using query's selector.<br>
<code>query.upsert(name: "John")</code></td></tr>

<tr><th>remove</th>
<td>Remove a single document matching the query's selector.<br>
<code>query.remove</code></td></tr>

<tr><th>remove_all</th>
<td>Remove all documents matching the query's selector.<br>
<code>query.remove_all</code></td></tr>

</tbody></table>

# Thread-Safety

Moped is thread-safe -- depending on your definition of thread-safe. For Moped,
it means that there's no shared, global state between multiple sessions. What
it doesn't mean is that a single `Session` instance can be interacted with
across threads.

Why not? Because threading is hard. Well, it's more than that -- though the
public API for Moped is quite simple, MongoDB requires a good deal of
complexity out of the internal API, specifically around replica sets and
failover. We've decided that, for now, it's not worth making the replica set
code thread-safe.

**TL;DR**: use one `Moped::Session` instance per thread.

# Compatibility

Moped is tested against MRI 1.9.2, 1.9.3, 2.0.0, and JRuby (1.9).

<img src="https://secure.travis-ci.org/mongoid/moped.png?branch=master&.png"/>

[Build History](http://travis-ci.org/mongoid/moped)
