#include "pch.h"
#include "Game/UserInterface/HpGauge.h"
#include "Framework/Resources.h"

// コンストラクタ
HpGauge::HpGauge()
	:
	m_graphics{ Graphics::GetInstance() },
	m_position{},
	m_isDead{}
{
}

// 初期化処理
void HpGauge::Initialize(DirectX::SimpleMath::Vector2 position)
{
	using namespace DirectX;

	// スプライトバッチを作成する
	m_spriteBatch = m_graphics->GetSpriteBatch();

	// テクスチャを読み込む
	m_texture = Resources::GetInstance()->GetBoxTexture();

	// ゲージの初期値を設定する
	m_value = DEFAULT_VALUE;

	// ゲージの初期座標の設定
	m_position = position;

	// 体力ゲージの設定
	m_hpGaugePosition = HP_POSITION;

}

// 描画処理
void HpGauge::Render(float hpRatio)
{
	// 死亡判定
	if (m_value <= 0.0f)
	{
		m_isDead = true;
	}

	using namespace DirectX;
	using namespace DirectX::SimpleMath;

	// 全体の長さ
	float hpWidth = HP_POSITION.right - HP_POSITION.left;
	m_hpGaugePosition.right = HP_POSITION.right - (hpWidth * hpRatio);

	// 描画位置のオフセット値や緑ゲージの幅を計算する
	LONG offset = static_cast<LONG>(m_position.x - (MAX_WIDTH / 2));
	LONG width  = static_cast<LONG>(offset + MAX_WIDTH * (m_value / static_cast<float>(DEFAULT_VALUE)));

	// ゲージの矩形を決める
	RECT back { offset, m_position.y - 20, offset + MAX_WIDTH, m_position.y + 20 };
	RECT front{ offset, m_position.y - 20, width,              m_position.y + 20 };
	//** left,top,right,bottom

	// スプライトバッチを開始する
	m_spriteBatch->Begin();

	// ゲージを描画する
	m_spriteBatch->Draw(m_texture, back,  Colors::Black);	// 背景
	m_spriteBatch->Draw(m_texture, front, Colors::Green);	// 表面

	//m_spriteBatch->Draw(m_texture, STETAS_FRAME, Colors::Gray);
	m_spriteBatch->Draw(m_texture, HP_POSITION, Colors::Black);
	m_spriteBatch->Draw(m_texture, m_hpGaugePosition, Colors::Green);
	

	// スプライトバッチを終了する
	m_spriteBatch->End();
}
