/*
	@file	Fade.h
	@brief	シーン遷移フェードクラス
*/
#pragma once

class Fade
{
public:
	enum FadeType { NONE, FADEIN, FADEOUT };

private:
	static constexpr float FADE_SPEED = 2.0f;

public:
	Fade(float alpha);
	~Fade() = default;
	void Update(float elapsedTime);
	void Render();

private:
	// フェードの種類
	FadeType m_fadeType;
	// 不透明度
	float m_alpha;
	// 色
	DirectX::SimpleMath::Vector4 m_color;

public:
	// フェードイン
	void FadeIn(){ m_fadeType = FADEIN; }
	// フェードアウト
	void FadeOut(){ m_fadeType = FADEOUT; }
	// フェードが終わったかどうか
	bool FinishFade();
	// フェードタイプの取得
	FadeType GetFadeType() { return m_fadeType; }
}; 