#pragma once
#include <string>
#include <optional>
#include <vector>
#include <Windows.h>

namespace utils
{
#define MEMBERBYOFFSET(type, class, offset) *reinterpret_cast<type*>(reinterpret_cast<uintptr_t>(class) + offset)

	std::optional<std::string> getProcessName(DWORD processId)
	{
		std::optional<std::string> ret;
		HANDLE handle = OpenProcess(
			PROCESS_QUERY_LIMITED_INFORMATION,
			FALSE,
			processId
		);

		if (handle)
		{
			DWORD buffSize = 1024;
			CHAR buffer[1024];

			if (QueryFullProcessImageNameA(handle, 0, buffer, &buffSize))
				ret.emplace(buffer);
			// else
			// 	printf("Error GetModuleBaseNameA : %lu", GetLastError());

			CloseHandle(handle);
		}
		// else
		// 	printf("Error OpenProcess : %lu", GetLastError());

		return ret;
	}


	static bool writeBytes(const std::uintptr_t address, const std::vector<uint8_t>& bytes)
	{
		return WriteProcessMemory(
			GetCurrentProcess(),
			reinterpret_cast<LPVOID>(address),
			bytes.data(), bytes.size(),
			nullptr
		);
	}

	inline std::vector<std::string> splitString(const std::string& str, const std::string_view delimiter)
	{
		std::size_t pos_start = 0, pos_end, delim_len = delimiter.length();
		std::string token;
		std::vector<std::string> res;

		while ((pos_end = str.find(delimiter, pos_start)) != std::string::npos)
		{
			token = str.substr(pos_start, pos_end - pos_start);
			pos_start = pos_end + delim_len;
			res.push_back(token);
		}

		res.push_back(str.substr(pos_start));
		return res;
	}
}
