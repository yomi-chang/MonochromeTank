#include "pch.h"
#include "Libraries/MyLib/DebugLog.h"

// デバッグログを「出力」に表示
void mylib::DebugLog(std::string text, float value)
{
#ifdef _DEBUG
	std::string str = text + " : " + std::to_string(value) + "\n";
	LPCSTR lpcstr = str.c_str();
	OutputDebugStringA(lpcstr);
#endif
}

void mylib::DebugLog(std::string text, int value)
{
#ifdef _DEBUG
	std::string str = text + " : " + std::to_string(value) + "\n";
	LPCSTR lpcstr = str.c_str();
	OutputDebugStringA(lpcstr);
#endif
}

void mylib::DebugLog(std::string text)
{
#ifdef _DEBUG
	std::string str = text + "\n";
	LPCSTR lpcstr = str.c_str();
	OutputDebugStringA(lpcstr);
#endif
}
