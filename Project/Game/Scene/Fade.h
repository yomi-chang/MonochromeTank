/**
 * @file   Fade.h
 * @brief  シーン遷移フェードクラス
 */
#pragma once

class Fade
{
public:
	enum FadeType { NONE, FADEIN, FADEOUT };

public:
	// コンストラクタ
	Fade(float alpha);
	// デストラクタ
	~Fade();
	// 更新処理
	void Update(float elapsedTime);
	// 描画処理
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