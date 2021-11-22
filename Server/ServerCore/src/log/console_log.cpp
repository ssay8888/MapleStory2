#include "pch.h"
#include "console_log.h"

ConsoleLog::ConsoleLog()
{
	_std_out = ::GetStdHandle(STD_OUTPUT_HANDLE);
	_std_err = ::GetStdHandle(STD_ERROR_HANDLE);
}

ConsoleLog::~ConsoleLog()
{
}

auto ConsoleLog::WriteStdOut(const kColor color, const WCHAR* format, ...) -> void
{
	if (format == nullptr)
		return;

	SetColor(true, color);

	va_list ap;
	va_start(ap, format);
	::vwprintf(format, ap);
	va_end(ap);

	fflush(stdout);

	SetColor(true, kColor::kWhite);
}

auto ConsoleLog::WriteStdErr(const kColor color, const WCHAR* format, ...) -> void
{
	WCHAR buffer[BUFFER_SIZE];

	if (format == nullptr)
		return;

	SetColor(false, color);

	va_list ap;
	va_start(ap, format);
	::vswprintf_s(buffer, BUFFER_SIZE, format, ap);
	va_end(ap);

	::fwprintf_s(stderr, buffer);
	fflush(stderr);

	SetColor(false, kColor::kWhite);
}

auto ConsoleLog::SetColor(const bool stdOut, kColor color) -> void
{
	static WORD SColors[]
	{
		0,
		FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
		FOREGROUND_RED | FOREGROUND_INTENSITY,
		FOREGROUND_GREEN | FOREGROUND_INTENSITY,
		FOREGROUND_BLUE | FOREGROUND_INTENSITY,
		FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY
	};

	::SetConsoleTextAttribute(stdOut ? _std_out : _std_err, SColors[static_cast<int32>(color)]);
}