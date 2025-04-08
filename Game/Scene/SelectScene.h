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

class SelectScene final :
    public IScene
{
private:
	static constexpr DirectX::SimpleMath::Vector2 SELECT_POS1 = { 100,283 };
	static constexpr DirectX::SimpleMath::Vector2 SELECT_POS2 = { 100,427 };
	static constexpr DirectX::SimpleMath::Vector2 SELECT_POS3 = { 140,560 };

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
	// 選択テクスチャ回転
	float m_selectAngle;
	DirectX::SimpleMath::Vector2 m_selectPos;
	RECT m_stageTexturePos;
	RECT m_tankCountTexturePos;

private:
	// ステージ設定の設定
	void SetStageSetting();
};
