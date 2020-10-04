local amqp = require("amqp")

-- Start a communication session with RabbitMQ
local conn = amqp.new({
  host = 'rabbitmq',
  port = 5672,
  username = "guest",
  password = "guest",
  vhost ="/"
})

local msg = "This is a test !"
local channel = conn:open_channel()
local queue = channel:queue_declare("test_queue")

queue:publish_message(msg)

local new_msg, tag = queue:consume_message()

if (msg ~= new_msg) then
  error("test failed ! wrong msg:" .. new_msg)
end

-- acking the msg
channel:ack(tag)

-- close the connection
conn:close()
