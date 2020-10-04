local amqp = require('amqp')

local conn = amqp.new({
  host = 'rabbitmq',
  port = 5672,
  username = "guest",
  password = "guest",
  vhost ="/"
})

local msg = "this is a test"
local route_key = "test_route_key"
local exchange_name = "test_ex_1"
local queue_name = "test_queue_1"

local channel = conn:open_channel()
local queue = channel:queue_declare(queue_name)
local exchange = channel:exchange_declare(exchange_name)

queue:bind(exchange_name, route_key)
exchange:publish_message(route_key, msg)

local new_msg, tag = queue:consume_message()

if (msg ~= new_msg) then
  error("test failed ! wrong msg:" .. new_msg)
end

-- acking the msg
channel:ack(tag)

-- close the connection
conn:close()
