#include "properties.h"

/**
* :lua_amqp_properties_headers
*
* pushing the properties headers into the stack as a table of key-val
*
* @params[1] properties
* @returns headers table
*/
LUALIB_API int lua_amqp_properties_headers(lua_State *L) {
  properties_t *properties = (properties_t *)luaL_checkudata(L, 1, "properties");
  amqp_basic_properties_t *amqp_props = &properties->envelope->message.properties;

  amqp_table_t *table = &(amqp_props->headers);
  int num_of_entries = table->num_entries;
  char* res_buff;
  int str_size;
  int i;

  lua_newtable(L);
  for (i=0; i < num_of_entries; i++) {
    if (table->entries[i].value.kind == AMQP_FIELD_KIND_UTF8) { // at the moment - support only string type. TODO: support the rest
      str_size = table->entries[i].key.len;
      res_buff = (char*)malloc(str_size + 1);
      if (!res_buff) {
        die(L, "Malloc for key failed");
      }
      extract_envelope_message(table->entries[i].key.bytes, str_size, res_buff);
      lua_pushstring(L, res_buff);
      str_size = table->entries[i].value.value.bytes.len;
      res_buff = (char*)realloc(res_buff, str_size + 1);
      if (!res_buff) {
        free(res_buff);
        die(L, "Malloc for value failed");
      }
      extract_envelope_message(table->entries[i].value.value.bytes.bytes, str_size, res_buff);
      lua_pushstring(L, res_buff);
      free(res_buff);
      lua_rawset(L, -3);
    }
  }

  return 1;
}

LUALIB_API int lua_amqp_properties_free(lua_State *L) {
  properties_t *properties = (properties_t *)luaL_checkudata(L, 1, "properties");
  amqp_destroy_envelope(properties->envelope);
  free(properties->envelope);
  return 0;
}

static luaL_Reg properties_module[] = {
  { NULL, NULL }
};

LUALIB_API int lua_amqp_properties_open(lua_State *L) {
  createmeta(L, "properties", properties_reg);
  luaL_setfuncs(L, properties_module, 0);
  return 0;
}
