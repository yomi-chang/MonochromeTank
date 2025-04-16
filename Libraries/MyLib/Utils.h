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
}