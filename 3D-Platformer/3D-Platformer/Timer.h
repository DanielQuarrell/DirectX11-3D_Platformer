#pragma once
#include <chrono>
#include <Windows.h>
#include <string>

class Timer
{
public:
	Timer() noexcept;
	float Mark() noexcept;
	float Peek() const noexcept;
private:
	std::chrono::steady_clock::time_point last;
};
