#pragma once
#include"Libraries/Microsoft/RenderTexture.h"

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
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_bulletTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_cannonTexture;

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTargetView;

	std::unique_ptr<DX::RenderTexture> m_backBuffer;
	
public:
	// コンストラクタ
	Magazine();
	// デストラクタ
	~Magazine() = default;
	// 初期化処理
	void Initialize();
	// 描画処理
	void Render();

private:
	// シェーダの読み込み
	void LoadShader();

	// テクスチャの読み込み
	void LoadTexture();
};