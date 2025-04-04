/*
	@file	SkySphere.h
	@brief	天球クラス
*/
#pragma once

class SkySphere
{
public:
	SkySphere();

	// 描画処理
	void Render();

private:
	// 天球のモデル
	DirectX::Model* m_skyModel;
};