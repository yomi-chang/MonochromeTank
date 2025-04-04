/*
	@file	DebugLog.h
	@brief	デバッグログ表示
*/
#pragma once

namespace mylib
{
	// デバッグログを「出力」に表示
	void DebugLog(std::string text, float value);
	void DebugLog(std::string text, int value);
	void DebugLog(std::string text);
	void DebugLog(DirectX::SimpleMath::Vector3 position);
}