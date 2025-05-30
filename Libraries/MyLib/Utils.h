/*
	@file	Utils.h
	@brief	雑多な便利関数
*/
#pragma once

namespace mylib
{
	// クランプ関数
	float Clamp(float value, float min, float max);

	// ランダム関数
	float Random(float min,float max);

	// テクスチャの中心座標を取得する
	DirectX::SimpleMath::Vector2 GetTextureCenter(ID3D11ShaderResourceView* tex);

	// テクスチャのサイズ取得
	DirectX::SimpleMath::Vector2 GetTextureSize(ID3D11ShaderResourceView* tex, float scale);

	// テクスチャの矩形取得
	RECT GetTextureRect(ID3D11ShaderResourceView* tex, float scale, const DirectX::SimpleMath::Vector2 pos);

	// デバッグログを「出力」に表示
	void DebugLog(std::string text, float value);
	void DebugLog(std::string text, int value);
	void DebugLog(std::string text);
	void DebugLog(const DirectX::SimpleMath::Vector3& position);
}