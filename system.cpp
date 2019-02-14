#include "system.h"

TaskManager TM;
IO io;
Network network;
LuaWrapper lua;


extern "C" {
  static int lua_wrapper_get_pressed(lua_State *L) {
    int row = luaL_checkinteger(L,1);
    int column = luaL_checkinteger(L,2);

    lua_pushboolean(L, io.matrix.state[row-1][column-1]);
    return 1;
  }
  static int lua_wrapper_lcd_backlight(lua_State *L) {
    bool state = lua_toboolean(L, 1);
    if (state) {
      io.lcd.backlight();
    } else {
      io.lcd.noBacklight();
    }
    return 0;
  }
  static int lua_wrapper_lcd_display(lua_State *L) {
    bool state = lua_toboolean(L, 1);
    if (state) {
      io.lcd.display();
    } else {
      io.lcd.noDisplay();
    }
    return 0;
  }
  static int lua_wrapper_lcd_cursor(lua_State *L) {
    int row = luaL_checkinteger(L,1);
    int column = luaL_checkinteger(L,2);

    io.lcd.setCursor(column-1, row-1);
    return 0;
  }
  static int lua_wrapper_lcd_print (lua_State *L) {
    int n = lua_gettop(L);  /* number of arguments */
    int i;
    lua_getglobal(L, "tostring");
    for (i=1; i<=n; i++) {
      const char *s;
      size_t l;
      lua_pushvalue(L, -1);  /* function to be called */
      lua_pushvalue(L, i);   /* value to print */
      lua_call(L, 1, 1);
      s = lua_tolstring(L, -1, &l);  /* get result */
      if (s == NULL)
        return luaL_error(L, "'tostring' must return a string to 'print'");
      if (i>1) io.lcd.print(" ");
      io.lcd.print(s);
      lua_pop(L, 1);  /* pop result */
    }
    return 0;
}
}


void updateIO_task()
{
  io.update();
}

String loop_func;
void lua_task() {
  String error = lua.Lua_dostring(&loop_func);
  if (error) {
    Serial.println();
  }
}

void boot_system() {
  pinMode(LED_BUILTIN, OUTPUT);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  Serial.begin(115200);
  SPIFFS.begin();
  Serial.println("\n");
  Serial.println("Begin booting...");
  io.lcd.begin(16,2);

  SPIFFS.begin();

  auto main_lua = SPIFFS.open("/main.lua", "r");
  lua.Lua_register("get_pressed", &lua_wrapper_get_pressed);
  lua.Lua_register("lcd_backlight", &lua_wrapper_lcd_backlight);
  lua.Lua_register("lcd_display", &lua_wrapper_lcd_display);
  lua.Lua_register("lcd_print", &lua_wrapper_lcd_print);
  lua.Lua_register("lcd_set_cursor", &lua_wrapper_lcd_cursor);
  if (main_lua)
  {
    loop_func = main_lua.readString();
    main_lua.close();
  } else {
    loop_func = "";
    Serial.println("main.lua not found!");
  }
  
  network.load_aps();

  unsigned int t = millis();

  TM.add_task(&updateIO_task, 50, t);
  TM.add_task(&lua_task, 100, t);
}
