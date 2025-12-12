/**
 * @file   Button.h
 * @brief  ボタンクラス
 */
#pragma once
class Button
{
public:
	// コンストラクタ
	Button();
	// デストラクタ
	~Button();
	// 初期化処理
	void Initialize(ID3D11ShaderResourceView* tex, float scale, const DirectX::SimpleMath::Vector2 pos);
	// 描画処理
	void Render(float rotation = 0);

private:
	// テクスチャ情報
	ID3D11ShaderResourceView* m_texture;
	// 画像倍率
	float m_scale;
	// 画像座標
	DirectX::SimpleMath::Vector2 m_buttonPosition;
	// ボタンの矩形
	RECT m_buttonRect;
	// クリック時の処理
	std::function<void()> m_onClick;
	// マウス接触時の処理
	std::function<void()> m_onMouseOver;

public:
	// ボタンのカスタム矩形
	void SetButtonRect(RECT rect) { m_buttonRect = rect; }
	// クリック処理のセット
	void SetOnClick(const std::function<void()>& func) { m_onClick = func; }
	// マウス接触時の処理のセット
	void SetOnMouseOver(const std::function<void()>& func) { m_onMouseOver = func; }
	// ボタンがクリックされかのチェック
	void CheckClickButton();
	// マウスが接触していた場合の処理
	void CheckOnMouseOver();

private:
	// ボタンがマウスと接触しているかどうか
	bool IsMouseOverUI();
};

