#pragma once

#include "../globals.h"

#include "../features/gui/gui.h"
#include "../features/lua.h"

#include "../headers/sdk/luajit.h"

#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

namespace lua_api {
	// doesnt' seem to work?
	LUA_FUNCTION(load_bytecode)
	{
		return 0;
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

	LUA_FUNCTION(is_in_game)
	{
		LUA->PushBool(EngineClient->IsInGame());

		return 1;
	}

	LUA_FUNCTION(get_local_player)
	{
		LUA->PushBool(EngineClient->GetLocalPlayer());

		return 1;
	}

	LUA_FUNCTION(get_view_angles)
	{
		QAngle viewAngles;

		EngineClient->GetViewAngles(viewAngles);

		LUA->CreateTable();
		{
			LUA->PushNumber(viewAngles.x);
			LUA->SetField(-2, "x");

			LUA->PushNumber(viewAngles.y);
			LUA->SetField(-2, "y");

			LUA->PushNumber(viewAngles.z);
			LUA->SetField(-2, "z");
		}

		return 1;
	}

	LUA_FUNCTION(set_view_angles)
	{
		LUA->CheckNumber(1);
		LUA->CheckNumber(2);
		
		// x, y
		QAngle viewAngles{ (vec_t)LUA->GetNumber(1), (vec_t)LUA->GetNumber(2), 0.f };

		EngineClient->SetViewAngles(viewAngles);
		
		return 0;
	}

	LUA_FUNCTION(change_name)
	{
		LUA->CheckString(1);

		const char* name = LUA->GetString(1);
		static uint8_t packet[256 + 2 + sizeof(name)];

		CNetChan* netchannel = EngineClient->GetNetChannelInfo();
		bf_write write;
		write.StartWriting(packet, sizeof(packet));
		write.WriteUInt(static_cast<uint32_t>(NetMessage::net_SetConVar), NET_MESSAGE_BITS);
		write.WriteByte(1);
		write.WriteString("name");
		write.WriteString(name);

		netchannel->SendData(&write, true);

		LUA->PushBool(true);

		return 1;
	}

	LUA_FUNCTION(custom_disconnect)
	{
		LUA->CheckString(1);

		const char* reason = LUA->GetString(1);
		static uint8_t packet[256 + 2 + sizeof(reason)];

		CNetChan* netchannel = EngineClient->GetNetChannelInfo();
		bf_write write;
		write.StartWriting(packet, sizeof(packet));
		write.WriteUInt(static_cast<uint32_t>(NetMessage::net_Disconnect), NET_MESSAGE_BITS);
		write.WriteByte(1);
		write.WriteString(reason);

		netchannel->SendData(&write, true);
		netchannel->Transmit(false);

		return 0;
	}

	// doesn't work? requires research
	LUA_FUNCTION(exploit_sourcenet)
	{
		int data[23] = { 0, 83, 101, 110, 100, 83, 101, 114, 118, 101, 114, 77, 97, 112, 67, 121, 99, 108, 101, 0, 8, 8, 0 };
		static uint8_t packet[256 + 2 + sizeof(data)];

		CNetChan* netchannel = EngineClient->GetNetChannelInfo();
		bf_write write;
		write.StartWriting(packet, sizeof(packet));
		write.WriteUInt(static_cast<uint32_t>(NetMessage::clc_CmdKeyValues), NET_MESSAGE_BITS);
		write.WriteLong(22);

		for (int i = 0; i < sizeof(data) / sizeof(int); i++)
		{
			write.WriteChar(data[i]);
		}

		netchannel->SendData(&write, true);
		netchannel->Transmit(false);

		return 0;
	}
	
	// doesn't work? requires research
	LUA_FUNCTION(exploit_achievement)
	{
		LUA->CheckNumber(1);
		LUA->CheckNumber(2);

		int data[22] = { 0, 65, 99, 104, 105, 101, 118, 101, 109, 101, 110, 116, 69, 97, 114, 110, 101, 100, 0, 8, 8, 0 };
		static uint8_t packet[256 + 2 + sizeof(data)];

		CNetChan* netchannel = EngineClient->GetNetChannelInfo();
		bf_write write;
		write.StartWriting(packet, sizeof(packet));
		write.WriteUInt(static_cast<uint32_t>(NetMessage::clc_CmdKeyValues), NET_MESSAGE_BITS);
		write.WriteLong(LUA->GetNumber(1));
		write.WriteSignedInt(LUA->GetNumber(2), 16);

		for (int i = 0; i < sizeof(data) / sizeof(int); i++)
		{
			write.WriteChar(data[i]);
		}

		netchannel->SendData(&write, true);

		return 0;
	}

	LUA_FUNCTION(exploit_requestfile)
	{
		LUA->CheckString(1);

		CNetChan* netchannel = EngineClient->GetNetChannelInfo();
		const char* fileName = LUA->GetString(1);

		// https://github.com/perilouswithadollarsign/cstrike15_src/blob/f82112a2388b841d72cb62ca48ab1846dfcc11c8/engine/net_chan.cpp#L572
		netchannel->RequestFile(&fileName, false);

		return 0;
	}

	LUA_FUNCTION(exploit_sendfile)
	{
		LUA->CheckString(1);
		LUA->CheckNumber(2);

		CNetChan* netchannel = EngineClient->GetNetChannelInfo();
		const char* fileName = LUA->GetString(1);
		unsigned int transferID = LUA->GetNumber(2);

		// https://github.com/perilouswithadollarsign/cstrike15_src/blob/f82112a2388b841d72cb62ca48ab1846dfcc11c8/engine/net_chan.cpp#L611
		netchannel->SendFile(fileName, transferID);

		return 0;
	}

	// FIXME: i don't know why it doesn't work
	//		  and i don't want to deal with it now
	// 
	// 11:40 PM / 19.02.2024 / shockpast
	LUA_FUNCTION(load_script)
	{
		LUA->CheckString(1);

		std::string pathName("C:\\asuna\\lua\\");
		std::string fileName(LUA->GetString(1));
		std::string rootPath = pathName + fileName;

		if (!fs::exists(rootPath.c_str()))
		{
			logger::AddLog("[warning] %s doesn't exist!", fileName);
			return 0;
		}

		std::ifstream file(rootPath.c_str());
		std::string content((std::istreambuf_iterator<char>(file)),
							(std::istreambuf_iterator<char>()));
		if (content.length() <= 0)
		{
			logger::AddLog("[warning] %s is empty!", fileName);
			return 0;
		}

		try
		{
			std::thread(RunScript, content).detach();
		}
		catch (std::exception& e)
		{
			logger::AddLog("[error] %s", e.what());
		}

		return 0;
	}

	void init()
	{
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
			// asuna - logger
			// [debug], [warning], [error]
			Lua->PushCFunction(log);
			Lua->SetField(-2, "log");

			// unrestricted clientcmd for lua
			Lua->PushCFunction(clientcmd);
			Lua->SetField(-2, "clientcmd");

			// returns true if player currently in game
			Lua->PushCFunction(is_in_game);
			Lua->SetField(-2, "is_in_game");

			Lua->PushCFunction(load_script);
			Lua->SetField(-2, "load_script");

			if (EngineClient->IsInGame())
			{
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
				Lua->SetField(-2, "player_info_s");

				//
				Lua->CreateTable();
				{
					// lags client movement, only visual (uneffective)
					// previously it could crash some servers
					Lua->PushCFunction(exploit_sourcenet);
					Lua->SetField(-2, "sourcenet");

					// notifies about fake achievement earning
					Lua->PushCFunction(exploit_achievement);
					Lua->SetField(-2, "achievement");

					Lua->PushCFunction(exploit_requestfile);
					Lua->SetField(-2, "requestfile");

					Lua->PushCFunction(exploit_sendfile);
					Lua->SetField(-2, "sendfile");
				}
				Lua->SetField(-2, "exploits");

				// playerinfo_s targetted on someone
				Lua->PushCFunction(get_player_info_s);
				Lua->SetField(-2, "get_player_info_s");

				// simple get latency
				Lua->PushCFunction(get_latency);
				Lua->SetField(-2, "get_latency"); // requires flow to be known

				// returns true if current frame inside
				// 'render.Capture' or has screenshot
				Lua->PushCFunction(in_screenshot); // always returns false
				Lua->SetField(-2, "in_screenshot");

				// changes name thru netchannel programatically
				Lua->PushCFunction(change_name);
				Lua->SetField(-2, "change_name");

				// disconnects localplayer with custom reason
				Lua->PushCFunction(custom_disconnect);
				Lua->SetField(-2, "custom_disconnect");

				// retrieves localplayer from engine interface
				Lua->PushCFunction(get_local_player);
				Lua->SetField(-2, "get_local_player");

				// retrieves viewangles of localplayer from engine interface
				Lua->PushCFunction(get_view_angles);
				Lua->SetField(-2, "get_view_angles");

				// sets (x, y) for viewangles of localplayer
				Lua->PushCFunction(set_view_angles);
				Lua->SetField(-2, "set_view_angles");
			}
		}
		Lua->SetField(-2, "asuna");

		luajit::call_lua_setfenv(Lua->GetLuaState(), -2);
	}
}