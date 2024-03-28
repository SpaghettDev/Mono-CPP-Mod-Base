#include <Windows.h>

#include "ManagedConsole.hpp"

void ManagedConsole::attach(const std::string_view& consoleName)
{
	if (AllocConsole())
	{
		freopen_s(&m_oldStdout, "CONOUT$", "w", stdout);
		freopen_s(&m_oldStdin, "CONIN$", "r", stdin);

		SetConsoleTitleA(consoleName.data());
		SetConsoleCP(CP_UTF8);
		SetConsoleOutputCP(CP_UTF8);
		SetConsoleCtrlHandler(nullptr, TRUE);
		EnableMenuItem(GetSystemMenu(GetConsoleWindow(), FALSE), SC_CLOSE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);

		m_ownsConsole = true;
	}
}

void ManagedConsole::detach()
{
	if (m_ownsConsole)
	{
		fclose(stdin);
		fclose(stdout);

		*stdin = *m_oldStdin;
		*stdout = *m_oldStdout;

		SetConsoleCtrlHandler(nullptr, FALSE);
		FreeConsole();
	}
}
