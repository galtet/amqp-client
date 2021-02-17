# Lua AMQP client

[![Build Status](https://travis-ci.com/galtet/amqp-client.svg?branch=master)](https://travis-ci.com/galtet/amqp-client) 
![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)


> Lua C wrapper for the amqp library.

A Lua C wrapper for the amqp library. based on: [rabbitmq-c](https://github.com/alanxz/rabbitmq-c)

![](header.png)

## Installation

This rock is relying on the librabbitmq library.

In order to install it, install the following RPMs (>= 0.8):
* librabbitmq
* librabbitmq-devel

Linux:

```sh
luarocks install lua-amqp-client 
```

## Usage example

Very convinient to use, since the C AMQP API is already implemented, so it is just a wrapper.

No official or convinient lua rock exists for communicating with rabbitmq - hopefully it wil help.

## Build from source

It can be built using luarocks builtin method or with Makefile.

Using make - just run make and copy the library (bin/amqp.so) to the wanted location:

```sh
make
```

## Quick Start

Below is a small snippet that demonstrates how to publish
and synchronously consume messages with lua-amqp.

``` lua
local amqp = require("amqp")

-- Start a communication session with RabbitMQ
local conn = amqp.new({})

-- open a channel
local channel = conn:open_channel()

-- get an existing queue
local queue = channel:queue('test_queue')

-- publish a message to the default exchange which then gets routed to this queue
queue:publish_message("This is a test !")

-- fetch a message from the queue
local msg, tag = queue:consume_message()

print("This is the message: " .. msg)

-- acking the msg
channel:ack(tag)

-- close the connection
conn:close()
```

#### Specific connection params

``` lua
local amqp = require("amqp")

-- Start a communication session with RabbitMQ
local conn = amqp.new({ 
  host = '127.0.0.1',
  port = 5672,
  username = "admin",
  password = "admin",
  vhost ="/"
})

-- close the connection
conn:close()
```

#### Open SSL connection

``` lua
local amqp = require("amqp")

-- Start a secured communication with RabbitMQ
local conn = amqp.new({ 
  ssl = true, 
  key = "/path/to/key.pem",
  cert = "/path/to/cert.pem",
  cacert = "/path/to/cert.pem"
  })

-- close the connection
conn:close()
```

## Contributing

1. Fork it (<https://github.com/galtet/amqp-client/fork>)
2. Create your feature branch (`git checkout -b feature/fooBar`)
3. Commit your changes (`git commit -am 'Add some fooBar'`)
4. Push to the branch (`git push origin feature/fooBar`)
5. Create a new Pull Request
