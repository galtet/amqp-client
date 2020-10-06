local amqp = require("amqp")

-- Start a communication session with RabbitMQ
local conn = amqp.new({
  host = 'rabbitmq',
  port = 5672,
  username = "guest",
  password = "guest",
  vhost ="/"
})

local msg = "This is a test for args!"
local exchange_name = "test_exchange_22"
local queue_name = "test_queue_22"
local channel = conn:open_channel()

local queue = channel:queue_declare(queue_name)
local exchange = channel:exchange_declare(exchange_name, "headers")

queue:bind(exchange_name, "", { w11 = "65" })
exchange:publish_message("", msg, { w11 = "65" })

local new_msg, tag = queue:consume_message()
assert(new_msg == msg)

-- acking the msg
channel:ack(tag)

-- close the connection
conn:close()
