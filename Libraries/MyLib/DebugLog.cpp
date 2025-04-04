/*
	@file	DebugLog.cpp
	@brief	デバッグログ表示
*/
#include "pch.h"
#include "Libraries/MyLib/DebugLog.h"

// デバッグログを「出力」に表示
void mylib::DebugLog(std::string text, float value)
{
#ifdef _DEBUG
	std::string str = text + ":" + std::to_string(value) + "\n";
	LPCSTR lpcstr = str.c_str();
	OutputDebugStringA(lpcstr);
#endif
}

void mylib::DebugLog(std::string text, int value)
{
#ifdef _DEBUG
	std::string str = text + ":" + std::to_string(value) + "\n";
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

void mylib::DebugLog(DirectX::SimpleMath::Vector3 position)
{
#ifdef _DEBUG
	DirectX::SimpleMath::Vector3 pos = position;
	std::string str = "Position:("
		+ std::to_string(position.x) + ","
		+ std::to_string(position.y) + ","
		+ std::to_string(position.z) + ")\n";
	LPCSTR lpcstr = str.c_str();
	OutputDebugStringA(lpcstr);
#endif
}
