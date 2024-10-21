#pragma once

// 前方宣言
class Graphics;
class InputManager;
class PlayerTank;

class Magazine
{
private:
	// グラフィックス
	Graphics* m_graphics;
	// プレイヤー
	PlayerTank* m_player;
	// 画面サイズ
	int m_windowWidth, m_windowHeight;
	// ピクセルシェーダ
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
	// テクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;
	// プリミティブバッチ 
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionTexture>> m_batch;

	// スプライトバッチ
	DirectX::SpriteBatch* m_spriteBatch;

public:
	// コンストラクタ
	Magazine();
	// デストラクタ
	~Magazine() = default;
	// 初期化処理
	void Initialize();
	// 更新処理
	void Update();
	// 描画処理
	void Render();

private:
	// シェーダの読み込み
	void LoadShader();
};