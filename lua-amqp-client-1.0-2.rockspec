package = "lua-amqp-client"
version = "1.0-2"
source = {
  url = "git://github.com/galtet/amqp-client.git",
  tag = "v1.0-2"
}
description = {
  summary = "Lua amqp driver",
  detailed = [[
  Lua based amqp client, based around the official C library of amqp (librabbitmq)
  ]],
  homepage = "https://github.com/galtet/amqp-client",
  license = "MIT/X11" -- or whatever you like
}
dependencies = {
  "lua >= 5.1, < 5.4",
}
build = {
  type = "make",
  install_target = "",
  variables = {
  },
  build_variables = {
   DRIVER_LIBS = "",
  },
  install = {
    lib = {
      ["amqp"] = "bin/amqp.so"
    }
  }
}
