#include <Windows.h>
#include <iostream>
#include <vector>
#include <string>

#include "common.hpp"
#include "utils.hpp"
#include "types.hpp"
#include "ManagedConsole/ManagedConsole.hpp"
#include "Mono/Mono.hpp"
#include "ValueManager/ValueManager.hpp"

void processHack(const std::vector<std::string>& args)
{
	if (args.size() == 1)
	{
		std::cout << "Please provide a hack argument!" << '\n';
		return;
	}

	if (strToHack.find(args[1]) == strToHack.cend()) return;

	switch (strToHack.at(args[1]))
	{
	case HACKS_COMMAND::TEST: {
		std::cout << "hello from hacks processor!" << '\n';

		ValueManager::Instance().test = !ValueManager::Instance().test;
	
		break;
	}
	}
}

void process(const std::string& command)
{
	if (command.empty()) return;

	const auto& args = utils::splitString(command, " ");

	if (strToCommand.find(args[0]) == strToCommand.cend()) return;

	switch (strToCommand.at(args[0]))
	{
	case CLI_COMMAND::QUIT: break;

	case CLI_COMMAND::HELP: {
		std::cout << "help..." << '\n';

		// example
		/*auto* setTimeScale = Mono::Instance().getMethod("Time", "set_timeScale", 1, "UnityEngine", "UnityEngine");
		if (setTimeScale)
		{
			std::cout << "setting the time scale..." << '\n';

			float scale = .5f;
			void* args[1] = { &scale };
			Mono::Instance().invoke(setTimeScale, nullptr, args);
		}*/
	
		break;
	}

	case CLI_COMMAND::HACKS: {
		processHack(args);
		
		break;
	}
	}
}

void mainSubroutine()
{
	if (
		auto procPath = utils::getProcessName(GetCurrentProcessId()).value_or("null");
		procPath.ends_with("example.exe") && Mono::Instance().isLoaded()
	) {
		std::cout << R"(
+-----------------------------------------------------------------------+
|                                                                       |
|          Welcome to MCMB (CLI)! Made with love by @SpaghettDev        |
|                     Type 'help' for more info!                        |
|                                                                       |
+-----------------------------------------------------------------------+
)" << '\n';

		for (std::string line; std::cout << "MCMB > " && std::getline(std::cin, line); )
		{
			process(line);

			if (line == "quit") break;
		}
	}
}


BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID)
{
	common::hmod = hModule;

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hModule);
		CreateThread(nullptr, 0,
			[](PVOID) -> DWORD {
				ManagedConsole::attach("MCMB");

				mainSubroutine();

				ManagedConsole::detach();
				FreeLibraryAndExitThread(common::hmod, 0);
				CloseHandle(common::hmod);

				return 0;
			}, nullptr, 0, nullptr);
	}

	return true;
}
