#pragma once
#include "Framework/Graphics.h"

class HpGauge
{
private:
	const RECT HP_POSITION = { 50,620,350,650 };
	const RECT STATUS_FRAME = { 10,600,360,670 };

public:
	// コンストラクタ
	HpGauge();

	// デストラクタ
	~HpGauge() = default;

	void Initialize(DirectX::SimpleMath::Vector2 position);
	void Render(float hpRatio);

	void SetValue(int value) { m_value = value; }

	const int GetDefaultValue() const { return DEFAULT_VALUE; }

	// ダメージ
	void Damage(float damage) { m_value -= static_cast<int>(damage); }

	// 死亡しているかどうか
	bool GetDead() { return m_isDead; }

	// 体力ゲージ
	RECT m_hpGaugePosition;



private:
	// グラフィックス
	Graphics* m_graphics;

	// スプライトバッチ
	DirectX::SpriteBatch* m_spriteBatch;

	// ゲージ画像
	ID3D11ShaderResourceView* m_texture;

	// ゲージの最大値
	const int MAX_WIDTH = 300;
	
	// ゲージの初期値
	const int DEFAULT_VALUE = 10;

	// ゲージの現在値
	int m_value;

	// 座標
	DirectX::SimpleMath::Vector2 m_position;

	// 死亡しているかどうか
	bool m_isDead;
};