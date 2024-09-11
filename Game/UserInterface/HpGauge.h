#pragma once
#include "Framework/Graphics.h"

class HpGauge
{
public:
	// コンストラクタ
	HpGauge();

	// デストラクタ
	~HpGauge() = default;

	void Initialize(DirectX::SimpleMath::Vector2 position);
	void Render();

	/*
		ゲージは外部から直接値を設定するか、
		ダメージ値などを受け取って、内部でゲージの表示量を計算する仕様にする
	*/
	void SetValue(int value) { m_value = value; }

	const int GetDefaultValue() const { return DEFAULT_VALUE; }

	// ダメージ
	void Damage(float damage) { m_value -= static_cast<int>(damage); }

	// 死亡しているかどうか
	bool GetDead() { return m_isDead; }



private:
	// グラフィックス
	Graphics* m_graphics;

	// スプライトバッチ
	DirectX::SpriteBatch* m_spriteBatch;

	// スプライトフォント
	DirectX::SpriteFont* m_spriteFont;

	// ゲージ画像
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;

	// ゲージの最大値
	const int MAX_WIDTH = 300;
	
	// ゲージの初期値
	const int DEFAULT_VALUE = 20;

	// ゲージの現在値
	int m_value;

	// 座標
	DirectX::SimpleMath::Vector2 m_position;

	// 死亡しているかどうか
	bool m_isDead;
};