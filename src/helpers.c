#include "helpers.h"

LUALIB_API void luaL_setfuncs (lua_State *L, const luaL_Reg *l, int nup) {
  luaL_checkstack(L, nup+1, "too many upvalues");
  for (; l->name != NULL; l++) {  /* fill the table with given functions */
    int i;
    lua_pushstring(L, l->name);
    for (i = 0; i < nup; i++)  /* copy upvalues to the top */
      lua_pushvalue(L, -(nup+1));
    lua_pushcclosure(L, l->func, nup);  /* closure with those upvalues */
    lua_settable(L, -(nup + 3));
  }
  lua_pop(L, nup);  /* remove upvalues */
}

LUALIB_API void setmeta(lua_State *L, const char *name) {
  luaL_getmetatable(L, name);
  lua_setmetatable(L, -2);
}

LUALIB_API int createmeta(lua_State *L, const char *name, const luaL_Reg *methods) {
  if (!luaL_newmetatable(L, name)) {
    return 0;
  }

  lua_pushstring(L, "__index");
  lua_newtable(L);
  lua_pushstring(L, "class");
  lua_pushstring(L, name);
  lua_rawset(L, -3);
  for (; methods->name; methods++) {
    lua_pushstring(L, methods->name);
    lua_pushcfunction(L, methods->func);
    lua_rawset(L, methods->name[0] == '_' ? -5: -3);
  }
  lua_rawset(L, -3);
  lua_pop(L, 1);
  return 1;
}

int luaL_optboolean(lua_State *L, int narg, int def) {
		return lua_isboolean(L, narg) ? lua_toboolean(L, narg) : def;
}

/* returns the num of keys in a table */
LUALIB_API int get_num_of_keys(lua_State *L, int index) {
  int res = 0;
  lua_pushnil(L);  /* first key */
  while (lua_next(L, index-1) != 0) {
    lua_pop(L, 1);
    res++;
  }

  return res;
}

/*
 *  AMQP SPECIFIC HELPERS
 */

void extract_envelope_message(void const *buffer, size_t len, char* res) {
  memcpy(res, buffer, len);
  res[len] = '\0';
}

void create_amqp_table(lua_State *L, int index, amqp_table_t *table) {
  int num_of_entries = get_num_of_keys(L,index);
  int entry_i = 0;

  table->num_entries = num_of_entries;
  table->entries = calloc(num_of_entries, sizeof(amqp_table_entry_t));

  if (lua_type(L, index) == LUA_TTABLE) {
    /* table is in the stack at index */
    lua_pushnil(L);  /* first key */
    while (lua_next(L, index-1) != 0) {
      /* uses 'key' (at index -1) and 'value' (at index) */
      table->entries[entry_i].key = amqp_cstring_bytes(luaL_checkstring(L, index-1));
      table->entries[entry_i].value.kind = AMQP_FIELD_KIND_UTF8;
      table->entries[entry_i].value.value.bytes = amqp_cstring_bytes(luaL_checkstring(L, index));
    //  amqp_cstring_bytes(luaL_checkstring(L, index));

      lua_pop(L, 1);
      entry_i++;
    }
  } else {
      die(L, "Properties table must be of type table");
  }
}

// setting headers from the stack into amqp properties
void create_amqp_properties(lua_State *L, int index, amqp_basic_properties_t *props) {
    memset(props, 0, sizeof *props);
    amqp_table_t *table = &(props->headers);
    create_amqp_table(L, -1, table);
    props->_flags |= AMQP_BASIC_HEADERS_FLAG;
}

/*
 *  END OF AMQP SPECIFIC HELPERS
 */
