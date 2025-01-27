#include "pch.h"
#include "Game/Scene/Fade.h"
#include "Framework/Graphics.h"
#include "Framework/Resources.h"

Fade::Fade(float alpha)
	:
	m_alpha{},
	m_color{},
	m_fadeType{NONE}
{
	// 色の設定
	m_color = static_cast<DirectX::SimpleMath::Vector4>(DirectX::Colors::Black);

	// 初期不透明度の設定
	m_alpha = alpha;
}

void Fade::Update(float elapsedTime)
{
	// 不透明度の設定
	switch (m_fadeType)
	{
		case Fade::FADEIN:
			m_alpha += elapsedTime;
			break;
		case Fade::FADEOUT:
			m_alpha -= elapsedTime;
			break;
		default:
			break;
	}
	m_color.w = m_alpha;
}

void Fade::Render()
{
	auto spriteBatch = Graphics::GetInstance()->GetSpriteBatch();

	RECT rect = { 0,0,1280,720 };

	// 描画
	spriteBatch->Begin();
	spriteBatch->Draw(Resources::GetInstance()->GetBoxTexture(), rect, m_color);
	spriteBatch->End();
}

bool Fade::FinishFade()
{
	// フェード処理が終了していたらtrueを返す
	if ((m_fadeType == FADEIN && m_alpha >= 1.0f) ||
		(m_fadeType == FADEOUT && m_alpha <= 0.0f)||
		m_fadeType == NONE)
	{
		m_fadeType = NONE;
		return true;
	}
	return false;
}
