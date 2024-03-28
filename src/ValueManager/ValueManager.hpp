#pragma once

class ValueManager
{
public:
	inline static ValueManager& Instance()
	{
		static ValueManager instance;

		return instance;
	}

private:
	ValueManager();
	bool init();

public:
	ValueManager(const ValueManager&) = delete;
	void operator=(const ValueManager&) = delete;

public:
	bool test;
};
