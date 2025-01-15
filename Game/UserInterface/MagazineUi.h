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
	const RECT MAIN_BULLET_FRAME = { 1000,600,1195,700 };
	const RECT SUB_BULLET_FRAME = { 1140,580,1270,650 };
	const RECT MAIN_BULLET_UI = { 1010,610,1100,650 };
	const RECT SUB_BULLET_UI = { 1200,600,1265,630 };
	const RECT RELOAD_GAUGE_BACK = { 1010,660,1185,690 };



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
	ID3D11ShaderResourceView* m_bulletTexture;
	ID3D11ShaderResourceView* m_cannonTexture;
	ID3D11ShaderResourceView* m_frameTexture;
	ID3D11ShaderResourceView* m_reloadTexture;
	ID3D11ShaderResourceView* m_gaugeTexture;


	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTargetView;

	std::unique_ptr<DX::RenderTexture> m_backBuffer;

	// スプライトバッチ
	DirectX::SpriteBatch* m_spriteBatch;

	// スプライトフォント
	DirectX::SpriteFont* m_spriteFont;
	
	// ゲージ座標
	RECT m_reloadPos;

	
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

	// 選択されている弾のUI
	void MainBulletUi();

	// 選択されていない弾のUI
	void SubBulletUi();

	// 残弾数の確認
	int CheckBulletValue();
};