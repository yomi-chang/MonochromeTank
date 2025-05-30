/*
	@file	MagazineUi.h
	@brief	弾関係UIクラス
*/
#pragma once

// 前方宣言
class Graphics;
class InputManager;
class PlayerTank;

// 残弾数UIクラス
class MagazineUi
{
private:
	// UI座標の定数
	static constexpr RECT MAIN_BULLET_FRAME = { 1000,625,1195,700 };
	static constexpr RECT SUB_BULLET_FRAME = { 1140,590,1270,645 };
	static constexpr RECT MAIN_BULLET_UI = { 1010,635,1100,675 };
	static constexpr RECT SUB_BULLET_UI = { 1200,600,1265,630 };
	static constexpr RECT RELOAD_GAUGE_BACK = { 1010,685,1170,695 };

	static constexpr int FONT_SIZE = 12;
	static constexpr int BASE_POS_X = 15;
	static constexpr float FONT_SCALE = 3.0f;

public:
	// コンストラクタ
	MagazineUi();
	// デストラクタ
	~MagazineUi();
	// 初期化処理
	void Initialize();
	// 描画処理
	void Render();

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
	// スプライトバッチ
	DirectX::SpriteBatch* m_spriteBatch;
	// ゲージ座標
	RECT m_reloadPos;

public:
	// プレイヤー設定
	void SetPlayer(PlayerTank* player) { m_player = player; }
	// 選択されている弾のUI
	void MainBulletUi();
	// 選択されていない弾のUI
	void SubBulletUi();
	// 残弾数の確認
	int CheckBulletValue();

private:
	// テクスチャの読み込み
	void LoadTexture();
};