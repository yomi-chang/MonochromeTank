/*
	@file	SelectScene.h
	@brief	セレクトシーンクラス
*/
#pragma once
#include "Interface/IScene.h"
#include "Framework/Graphics.h"

namespace mylib
{
	class LockOnCamera;
}

class Graphics;
class Resources;
class Floor;
class Fade;
class Button;

class SelectScene final :
    public IScene
{
private:
	static constexpr int FLOOR_SIZE = 15;
	static constexpr float CURSOR_SPEED = 2.0f;

	static constexpr DirectX::SimpleMath::Vector2 SELECT_POS1 = { 110,300 };
	static constexpr DirectX::SimpleMath::Vector2 SELECT_POS2 = { 110,445 };
	static constexpr DirectX::SimpleMath::Vector2 SELECT_POS3 = { 150,580 };

	static constexpr RECT STAGE1 = { 0,0,450,100 };
	static constexpr RECT STAGE2 = { 0,115,450,215 };
	static constexpr RECT STAGE3 = { 0,230,450,330 };

	static constexpr RECT TANK_COUNT1 = { 0,0,90,150 };
	static constexpr RECT TANK_COUNT2 = { 90,0,180,150 };
	static constexpr RECT TANK_COUNT3 = { 180,0,260,150 };

public:
	// コンストラクタ
	SelectScene();
	// デストラクタ
	~SelectScene() override;
	// 初期化処理
	void Initialize() override;
	// 更新処理
	void Update(float elapsedTime) override;
	// 描画処理
	void Render() override;
	// 終了処理
	void Finalize() override;
	// 次のシーンIDの取得
	SceneID GetNextSceneID() const;

private:
	// グラフィックス
	Graphics* m_graphics;
	Resources* m_resources;
	// シーンチェンジフラグ
	bool m_isChangeScene;
	// カメラ
	std::unique_ptr<mylib::LockOnCamera> m_camera;
	// 床
	std::unique_ptr<Floor> m_floor;
	// シーン遷移
	std::unique_ptr<Fade> m_fade;
	// カーソルの回転
	float m_selectAngle;
	DirectX::SimpleMath::Vector2 m_selectPos;
	RECT m_stageTexturePos;
	RECT m_tankCountTexturePos;
	// テクスチャ
	ID3D11ShaderResourceView* m_manualTex;
	ID3D11ShaderResourceView* m_settingFrameTex;
	ID3D11ShaderResourceView* m_stageTextTex;
	ID3D11ShaderResourceView* m_countTextTex;
	ID3D11ShaderResourceView* m_cursorTex;


	// ボタン
	std::vector<std::unique_ptr<Button>> m_buttons;

private:
	// 画像の設定
	void SetTextures();
	// ボタンの作成
	void CreateButton();
	// ステージ設定の設定
	void SetStageSetting();
	// UIの描画
	void DrawUi();
};
