#pragma once
#include <stdio.h>
#include <string_view>

#include "../common.hpp"

class ManagedConsole
{
public:
	static void attach(const std::string_view&);
	static void detach();

private:
	inline static FILE* m_oldStdin = nullptr;
	inline static FILE* m_oldStdout = nullptr;

	inline static bool m_ownsConsole = false;
};
