#include <iostream>
#include <fstream>
#include <algorithm>
#include "callbacks.hpp"
#include "events.hpp"
#include "amxhandler.hpp"
#include "node.hpp"
#include <sampgdk.h>
#include "common.hpp"
#include "config.hpp"

logprintf_t logprintf;

PLUGIN_EXPORT bool PLUGIN_CALL OnPublicCall(AMX* amx, const char* name, cell* params, cell* retval)
{
	if (sampnode::js_calling_public)
		return true;

	auto iter = sampnode::events.find(name);
	if (iter != sampnode::events.end())
		iter->second->call(amx, params, retval);

	return true;
}

PLUGIN_EXPORT void PLUGIN_CALL ProcessTick()
{
	sampgdk::ProcessTick();

	sampnode::node_tick();

	return;
}

PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports()
{
	return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES | SUPPORTS_PROCESS_TICK;
}

PLUGIN_EXPORT bool PLUGIN_CALL Load(void** ppData)
{
	logprintf = (logprintf_t)(ppData[PLUGIN_DATA_LOGPRINTF]);

	pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];

	if(!sampnode::Config::Get()->ParseFile())
	{	
		L_ERROR << "Unable to load samp-node config file, you need to have samp-node.json or samp-node.yml in root directory" 
			<< std::endl << "\tPlease read about samp-node config file in our wiki pages at https://github.com/AmyrAhmady/samp-node/wiki";
		return false;
	}

	Log::Init();
	sampgdk::Load(ppData);

	sampnode::callback::init();

	sampnode::node_init();

	return true;
}

PLUGIN_EXPORT int PLUGIN_CALL AmxLoad(AMX* amx)
{
	sampnode::amx::load(amx);

	return 1;
}

PLUGIN_EXPORT void PLUGIN_CALL Unload()
{
	sampgdk::Unload();

	sampnode::node_stop();

	return;
}

PLUGIN_EXPORT int PLUGIN_CALL AmxUnload(AMX* amx)
{
	sampnode::amx::unload(amx);

	return 1;
}