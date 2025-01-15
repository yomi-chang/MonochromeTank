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

	// スプライトフォントを作成する
	m_spriteFont = m_graphics->GetFont();

	// テクスチャを読み込む
	m_texture = Resources::GetInstance()->GetBoxTexture();

	// ゲージの初期値を設定する
	m_value = DEFAULT_VALUE;

	// ゲージの初期座標の設定
	m_position = position;
}

// 描画処理
void HpGauge::Render()
{
	// 死亡判定
	if (m_value <= 0.0f)
	{
		m_isDead = true;
	}

	using namespace DirectX;
	using namespace DirectX::SimpleMath;

	// ゲージのパラメータ文字列を準備する
	//wchar_t buf[64];											// バッファ
	//swprintf_s(buf, 64, L"%d / %d", m_value, DEFAULT_VALUE);	// 文字列
	//Vector2 origin{ m_spriteFont->MeasureString(buf) / 2.0f };	// 文字列の中心位置

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

	// 文字列を描画する
	//m_spriteFont->DrawString(
	//	m_spriteBatch,									// スプライトバッチのポインタ
	//	buf,											// 表示する文字
	//	Vector2(m_graphics->GetScreenWidth() / 2, 20),	// 描画位置
	//	Colors::White,									// 文字色
	//	0.0f,											// 回転
	//	origin											// originを描画位置に合わせる
	//);

	// スプライトバッチを終了する
	m_spriteBatch->End();
}
