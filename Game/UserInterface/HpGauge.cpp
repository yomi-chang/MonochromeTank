/*
	@file	HpGauge.cpp
	@brief	自機体力クラス
*/
#include "pch.h"
#include "Game/UserInterface/HpGauge.h"
#include "Framework/Resources.h"

//-------------------------------------------------------------------
// コンストラクタ
//-------------------------------------------------------------------
HpGauge::HpGauge()
	:
	m_graphics{ Graphics::GetInstance() },
	m_position{},
	m_isDead{}
{
}

//-------------------------------------------------------------------
// デストラクタ
//-------------------------------------------------------------------
HpGauge::~HpGauge()
{
}

//-------------------------------------------------------------------
// 初期化処理
//-------------------------------------------------------------------
void HpGauge::Initialize(DirectX::SimpleMath::Vector2 position)
{
	using namespace DirectX;

	// スプライトバッチを作成する
	m_spriteBatch = m_graphics->GetSpriteBatch();

	// テクスチャを読み込む
	m_texture = Resources::GetInstance()->GetBoxTexture();

	// ゲージの初期座標の設定
	m_position = position;

	// 体力ゲージの設定
	m_hpGaugePosition = HP_POSITION;

}

//-------------------------------------------------------------------
// 描画処理
//-------------------------------------------------------------------
void HpGauge::Render(float hpRatio)
{
	using namespace DirectX;
	using namespace DirectX::SimpleMath;

	// 全体の長さ
	float hpWidth = static_cast<float>(HP_POSITION.right - HP_POSITION.left);
	m_hpGaugePosition.right = HP_POSITION.right - (hpWidth * hpRatio);

	// スプライトバッチを開始する
	m_spriteBatch->Begin();

	// ゲージを描画する
	m_spriteBatch->Draw(m_texture, HP_POSITION, Colors::Black);
	m_spriteBatch->Draw(m_texture, m_hpGaugePosition, Colors::Green);

	// スプライトバッチを終了する
	m_spriteBatch->End();
}
