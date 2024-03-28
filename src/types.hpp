#pragma once
#include <map>
#include <string_view>

enum class CLI_COMMAND
{
	QUIT = -1,
	HELP,
	HACKS
};

enum class HACKS_COMMAND
{
	TEST
};

const std::map<std::string_view, CLI_COMMAND> strToCommand
{
	{ "quit", CLI_COMMAND::QUIT },
	{ "help", CLI_COMMAND::HELP },
	{ "hacks", CLI_COMMAND::HACKS },
};

const std::map<std::string_view, HACKS_COMMAND> strToHack{
	{ "test", HACKS_COMMAND::TEST }
};
