#pragma once

#include "../globals.h"

#include "../features/lua.h"
#include "../features/gui/gui.h"

#include "../ext/memory.h"

#include <string>

typedef bool(__thiscall* _RunStringEx)(CLuaInterface*, const char*, const char*, const char*, bool, bool, bool, bool);
_RunStringEx oRunStringEx = nullptr;

bool __fastcall hkRunStringEx(CLuaInterface* _this, const char* filename, const char* path, const char* stringToRun, bool run, bool printErrors, bool dontPushErrors, bool noReturns)
{
	if (settings::lua::scripthook::dumpServer)
	{
        SaveScript(filename, stringToRun);
	}

	return oRunStringEx(_this, filename, path, stringToRun, run, printErrors, dontPushErrors, noReturns);
}

typedef int(__thiscall* _CloseLuaInterfaceFn)(CLuaShared*, CLuaInterface*);
_CloseLuaInterfaceFn oCloseLuaInterfaceFn;

int __fastcall hkCloseInterfaceLuaFn(CLuaShared* _this, CLuaInterface* luaInterface)
{
    if (luaInterface == Lua) Lua = nullptr;
    return oCloseLuaInterfaceFn(_this, luaInterface);
}


typedef CLuaInterface* (__thiscall* _CreateLuaInterfaceFn)(CLuaShared*, LuaInterfaceType, bool);
_CreateLuaInterfaceFn oCreateLuaInterfaceFn;

CLuaInterface* __fastcall hkCreateLuaInterfaceFn(CLuaShared* _this, LuaInterfaceType luaState, bool renew) 
{
    auto luaInterface = oCreateLuaInterfaceFn(_this, luaState, renew);
    globals::lua::state = (int)luaState;

    if (luaState != LuaInterfaceType::LUA_CLIENT) return luaInterface;

    Lua = luaInterface;

    if (!oRunStringEx) oRunStringEx = VMTHook<_RunStringEx>((PVOID**)Lua, (PVOID)hkRunStringEx, 111, true);

    return Lua;
}