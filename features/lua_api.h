#pragma once

#include "../globals.h"

#include "../features/gui/gui.h"

#include "../headers/sdk/luajit.h"

namespace lua_api {
	LUA_FUNCTION(load_bytecode) // doesnt' seem to work?
	{
		LUA->CheckString(1);

		const char* buf = LUA->GetString();
		int ptr = luajit::call_luaL_loadbufferx(LUA->GetLuaState(), buf, sizeof(buf), "loaded bytecode", NULL);

		if (ptr != 0)
		{
			logger::AddLog("[error] luaL_loadbufferx @ failed");
			return 0;
		}

		LUA->Push(1);

		logger::AddLog("[debug] luaL_loadbufferx @ returning value...");

		return 1;
	}

	LUA_FUNCTION(log)
	{
		LUA->CheckString(1);

		logger::AddLog(LUA->GetString(-1));
	
		return 1;
	}

	LUA_FUNCTION(get_player_info_s)
	{
		LUA->CheckNumber(1);

		player_info_s info;
		EngineClient->GetPlayerInfo((int)LUA->GetNumber(1), &info);

		Lua->CreateTable();
		{
			Lua->PushString(info.name);
			Lua->SetField(-2, "name");

			Lua->PushNumber(info.userID);
			Lua->SetField(-2, "userID");

			Lua->PushString(info.guid);
			Lua->SetField(-2, "guid");

			Lua->PushNumber((int)info.friendsid);
			Lua->SetField(-2, "friendsid");

			Lua->PushString(info.friendsname);
			Lua->SetField(-2, "friendsname");

			Lua->PushBool(info.fakeplayer);
			Lua->SetField(-2, "fakeplayer");

			Lua->PushBool(info.ishltv);
			Lua->SetField(-2, "ishltv");

			Lua->PushNumber((int)info.customfiles);
			Lua->SetField(-2, "customfiles");

			Lua->PushNumber(info.filesdownloaded);
			Lua->SetField(-2, "filesdownloaded");
		}

		return 1;
	}

	LUA_FUNCTION(get_latency)
	{
		LUA->CheckNumber(1);

		int flow = LUA->GetNumber(1);

		CNetChan* netchannel = EngineClient->GetNetChannelInfo();
		float latency = netchannel->GetLatency(flow);

		LUA->PushNumber(latency);

		return 1;
	}

	LUA_FUNCTION(clientcmd)
	{
		LUA->CheckString(1);

		EngineClient->ClientCmd_Unrestricted(LUA->GetString(1));

		return 1;
	}

	LUA_FUNCTION(in_screenshot)
	{
		LUA->PushBool(globals::lua::api::in_screenshot); // doesnt' seem to work?

		return 1;
	}

	void init(int state) {
		// _ENV
		Lua->CreateTable();

		// _ENV metatable
		Lua->CreateTable();
		{
			Lua->PushSpecial(SPECIAL_GLOB);
			Lua->SetField(-2, "__index");
		}
		Lua->SetMetaTable(-2);

		// asuna table
		Lua->CreateTable();
		{
			if (state == (int)LuaInterfaceType::LUA_CLIENT)
			{
				// lua bytecode loader
				Lua->PushCFunction(load_bytecode);
				Lua->SetField(-2, "load_bytecode");

				// asuna - logger
				// [debug], [warning], [error]
				Lua->PushCFunction(log);
				Lua->SetField(-2, "log");

				// globalvars from [C] to Lua
				Lua->CreateTable();
				{
					Lua->PushNumber(GlobalVars->absoluteframetime);
					Lua->SetField(-2, "absoluteframetime");

					Lua->PushNumber(GlobalVars->curtime);
					Lua->SetField(-2, "curtime");

					Lua->PushNumber(GlobalVars->framecount);
					Lua->SetField(-2, "framecount");

					Lua->PushNumber(GlobalVars->frametime);
					Lua->SetField(-2, "frametime");

					Lua->PushNumber(GlobalVars->interpolation_amount);
					Lua->SetField(-2, "interpolation_amount");

					Lua->PushNumber(GlobalVars->interval_per_tick);
					Lua->SetField(-2, "interval_per_tick");

					Lua->PushNumber(GlobalVars->realtime);
					Lua->SetField(-2, "realtime");

					Lua->PushNumber(GlobalVars->tickcount);
					Lua->SetField(-2, "tickcount");
				}
				Lua->SetField(-2, "globalvars");

				// playerinfo_s from [C] to Lua
				Lua->CreateTable();
				{
					player_info_s info;
					EngineClient->GetPlayerInfo(EngineClient->GetLocalPlayer(), &info);

					Lua->PushString(info.name);
					Lua->SetField(-2, "name");

					Lua->PushNumber(info.userID);
					Lua->SetField(-2, "userID");

					Lua->PushString(info.guid);
					Lua->SetField(-2, "guid");

					Lua->PushNumber((int)info.friendsid);
					Lua->SetField(-2, "friendsid");

					Lua->PushString(info.friendsname);
					Lua->SetField(-2, "friendsname");

					Lua->PushBool(info.fakeplayer);
					Lua->SetField(-2, "fakeplayer");

					Lua->PushBool(info.ishltv);
					Lua->SetField(-2, "ishltv");

					Lua->PushNumber((int)info.customfiles);
					Lua->SetField(-2, "customfiles");

					Lua->PushNumber(info.filesdownloaded);
					Lua->SetField(-2, "filesdownloaded");
				}
				Lua->SetField(-2, "playerinfo_s");

				// playerinfo_s targetted on someone
				Lua->PushCFunction(get_player_info_s);
				Lua->SetField(-2, "get_player_info_s");

				// simple get latency
				Lua->PushCFunction(get_latency);
				Lua->SetField(-2, "get_latency"); // requires flow to be known

				// unrestricted clientcmd for lua
				Lua->PushCFunction(clientcmd);
				Lua->SetField(-2, "clientcmd");

				// returns true if current frame inside
				// 'render.Capture' or has screenshot (untested)
				Lua->PushCFunction(in_screenshot);
				Lua->SetField(-2, "in_screenshot");
			}
			else
			{
				// asuna - logger
				// [debug], [warning], [error]
				Lua->PushCFunction(log);
				Lua->SetField(-2, "log");
			}
		}
		Lua->SetField(-2, "asuna");

		luajit::call_lua_setfenv(Lua->GetLuaState(), -2);
	}
}