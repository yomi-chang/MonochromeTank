#pragma once
#include"Libraries/Microsoft/RenderTexture.h"
//#include <Windows.h>

// 前方宣言
class Graphics;
class InputManager;
class PlayerTank;

class MagazineUi
{
private:
	// UI座標の定数
	const RECT BULLET_UI_POS = { 1050,510,1150,560 };
	const RECT CANNON_UI_POS = { 1050,610,1150,660 };
	const RECT SELECT_BULLET = { 1030,500,1260,600 };
	const RECT SELECT_CANNON = { 1030,600,1260,700 };
	const RECT BULLET_RELOAD_GAUGE_BACK = { 1050,570,1150,590 };
	const RECT BULLET_RELOAD_GAUGE_FRONT = { 1055,575,1145,585 };
	const RECT CANNON_RELOAD_GAUGE_BACK = { 1050,670,1150,690 };
	const RECT CANNON_RELOAD_GAUGE_FRONT = { 1055,675,1145,685 };



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
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_frameTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_reloadTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_gaugeTexture;


	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTargetView;

	std::unique_ptr<DX::RenderTexture> m_backBuffer;

	// スプライトバッチ
	DirectX::SpriteBatch* m_spriteBatch;

	// スプライトフォント
	DirectX::SpriteFont* m_spriteFont;

	// 弾選択フレーム座標
	RECT m_framePos;
	
	// ゲージ座標
	RECT m_cannonReloadPos;
	RECT m_bulletReloadPos;

	
public:
	// コンストラクタ
	MagazineUi();
	// デストラクタ
	~MagazineUi() = default;
	// 初期化処理
	void Initialize();
	// 描画処理
	void Render();

private:
	// シェーダの読み込み
	void LoadShader();

	// テクスチャの読み込み
	void LoadTexture();

public:
	void SetPlayer(PlayerTank* player) { m_player = player; }
};