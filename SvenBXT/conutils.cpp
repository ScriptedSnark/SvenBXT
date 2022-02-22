#include <Windows.h>
#include <iostream>
#include <string>
#include <mutex>
#include "conutils.hpp"
#include "stdio.h"

static FILE* logfile = nullptr;
/* Dev Messages */
static void Log(const char* prefix, const char* msg)
{
	if (logfile)
	{
		auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		auto ltime = std::localtime(&time);
		fprintf(logfile, "[%02d:%02d:%02d] [%s] %s", ltime->tm_hour, ltime->tm_min, ltime->tm_sec, prefix, msg);
		fflush(logfile);
	}
}

void PrintMessage(const char* format, ...)
{
	va_list args;
	va_start(args, format);

	char temp[1024];
	vsprintf_s(temp, format, args);

	va_end(args);

	ConUtils::Log(temp);
	Log("Msg", temp);
}

void PrintDevMessage(const char* format, ...)
{
	va_list args;
	va_start(args, format);

	char temp[1024];
	vsprintf_s(temp, format, args);

	va_end(args);

	ConUtils::Log(temp, FOREGROUND_RED | FOREGROUND_GREEN);
	Log("DevMsg", temp);
}

void PrintWarning(const char* format, ...)
{
	va_list args;
	va_start(args, format);

	char temp[1024];
	vsprintf_s(temp, format, args);

	va_end(args);

	ConUtils::Log(temp, FOREGROUND_RED | FOREGROUND_INTENSITY);
	Log("Warning", temp);
}

void PrintDevWarning(const char* format, ...)
{
	va_list args;
	va_start(args, format);

	char temp[1024];
	vsprintf_s(temp, format, args);

	va_end(args);

	ConUtils::Log(temp, FOREGROUND_RED);
	Log("DevWarning", temp);
}

namespace ConUtils
{
	HANDLE hConsoleOutput = INVALID_HANDLE_VALUE;
	WORD wStandartAttributes;

	void Init()
	{
		AllocConsole();

		hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
		if (hConsoleOutput == INVALID_HANDLE_VALUE)
			return;

		CONSOLE_SCREEN_BUFFER_INFO coninfo;
		GetConsoleScreenBufferInfo(hConsoleOutput, &coninfo);
		wStandartAttributes = coninfo.wAttributes;
		coninfo.dwSize.X = 150;
		coninfo.dwSize.Y = 500;
		SetConsoleScreenBufferSize(hConsoleOutput, coninfo.dwSize);

		SetConsoleTitleA("SvenBXT Debug Console");

		ShowWindow(GetConsoleWindow(), SW_MAXIMIZE);
		ShowWindow(GetConsoleWindow(), SW_SHOWMINNOACTIVE);
	}

	void Free()
	{
		FreeConsole();
	}

	void Log(const char* szText)
	{
		if (hConsoleOutput != INVALID_HANDLE_VALUE)
			WriteConsoleA(hConsoleOutput, szText, strlen(szText), NULL, NULL);
	}

	void Log(const char* szText, WORD wAttributes)
	{
		if (hConsoleOutput != INVALID_HANDLE_VALUE) {
			SetConsoleTextAttribute(hConsoleOutput, wAttributes);
			WriteConsoleA(hConsoleOutput, szText, strlen(szText), NULL, NULL);
			SetConsoleTextAttribute(hConsoleOutput, wStandartAttributes);
		}
	}

	void Log(const WCHAR* szText)
	{
		if (hConsoleOutput != INVALID_HANDLE_VALUE)
			WriteConsoleW(hConsoleOutput, szText, wcslen(szText), NULL, NULL);
	}

	void Log(const WCHAR* szText, WORD wAttributes)
	{
		if (hConsoleOutput != INVALID_HANDLE_VALUE) {
			SetConsoleTextAttribute(hConsoleOutput, wAttributes);
			WriteConsoleW(hConsoleOutput, szText, wcslen(szText), NULL, NULL);
			SetConsoleTextAttribute(hConsoleOutput, wStandartAttributes);
		}
	}
}