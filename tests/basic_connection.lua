local amqp = require("amqp")

-- Start a communication session with RabbitMQ
local conn = amqp.new({
  host = 'rabbitmq',
  port = 5672,
  username = "guest",
  password = "guest",
  vhost ="/"
})

conn:close()
