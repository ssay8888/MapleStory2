#pragma once

/*---------------
	ConsoleLog
----------------*/

enum class kColor
{
	kBlack,
	kWhite,
	kRed,
	kGreen,
	kBlue,
	kYellow,
};

class ConsoleLog
{
	enum { BUFFER_SIZE = 4096 };

private:
	ConsoleLog();
	~ConsoleLog();

public:
	static auto GetInstance() -> ConsoleLog&
	{
		static ConsoleLog instance;
		return instance;
	}

public:
	auto WriteStdOut(kColor color, const WCHAR* str, ...) -> void;
	auto WriteStdErr(kColor color, const WCHAR* str, ...) -> void;

protected:
	auto SetColor(bool stdOut, kColor color) -> void;

private:
	HANDLE		_std_out;
	HANDLE		_std_err;
};