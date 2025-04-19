/*
	@file	HpGauge.h
	@brief	自機体力クラス
*/
#pragma once
#include "Framework/Graphics.h"

// 自機体力クラス
class HpGauge
{
private:
	static constexpr RECT HP_POSITION = { 50,620,350,650 };
	static constexpr RECT STATUS_FRAME = { 10,600,360,670 };

public:
	// コンストラクタ
	HpGauge();
	// デストラクタ
	~HpGauge();
	// 初期化処理
	void Initialize(const DirectX::SimpleMath::Vector2& position);
	// 描画処理
	void Render(float hpRatio);

private:
	// グラフィックス
	Graphics* m_graphics;
	// スプライトバッチ
	DirectX::SpriteBatch* m_spriteBatch;
	// ゲージ画像
	ID3D11ShaderResourceView* m_texture;
	// 座標
	DirectX::SimpleMath::Vector2 m_position;
	// 死亡しているかどうか
	bool m_isDead;
	// 体力ゲージ
	RECT m_hpGaugePosition;
};