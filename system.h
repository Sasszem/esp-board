#include <LuaWrapper.h>
#include <FS.h>
#include "tasks.hpp"
#include "IO.h"
extern TaskManager TM;
extern IO io;
#include "network.hpp"
extern Network network;

extern LuaWrapper lua;


void boot_system();
