package = "lua-amqp-client"
version = "1.0-1"
source = {
  url = "https://github.com/galtet/amqp-client",
  tag = "v1.0"
}
description = {
  summary = "Lua amqp driver",
  detailed = [[
  Lua based amqp client, based around the official C library of amqp (librabbitmq)
  ]],
  license = "MIT/X11" -- or whatever you like
}
dependencies = {
  "lua >= 5.1, < 5.4",
}
build = {
   type = "builtin",
   modules = {
      amqp = {
         sources = {"src/channel.c", "src/helpers.c", "src/lua_amqp.c", "src/queue.c", "src/session.c", "src/utils.c"},
         libraries = {"rabbitmq"},
         incdir = "/src"
      }
   }
}
