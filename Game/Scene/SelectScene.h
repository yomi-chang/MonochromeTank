/*
	@file	TitleScene.h
	@brief	タイトルシーンクラス
*/
#pragma once
#include "IScene.h"
#include "Framework/Graphics.h"

namespace mylib
{
	class LockOnCamera;
}

class Graphics;
class Resources;
class Floor;
class Tank;
class Fade;

class SelectScene final :
    public IScene
{
private:
	const DirectX::SimpleMath::Vector2 SELECT_POS1 = { 100,283 };
	const DirectX::SimpleMath::Vector2 SELECT_POS2 = { 100,427 };
	const DirectX::SimpleMath::Vector2 SELECT_POS3 = { 140,560 };

	const RECT STAGE1 = { 0,0,450,100 };
	const RECT STAGE2 = { 0,115,450,215 };
	const RECT STAGE3 = { 0,230,450,330 };

	const RECT TANK_COUNT1 = { 0,0,90,150 };
	const RECT TANK_COUNT2 = { 90,0,180,150 };
	const RECT TANK_COUNT3 = { 180,0,260,150 };

private:
	// グラフィックス
	Graphics* m_graphics;
	Resources* m_resources;

	DirectX::SpriteBatch* m_spriteBatch;

	// シーンチェンジフラグ
	bool m_isChangeScene;


	// カメラ
	std::unique_ptr<mylib::LockOnCamera> m_camera;

	// 床
	std::unique_ptr<Floor> m_floor;

	// 戦車
	std::vector<std::unique_ptr<Tank>> m_tanks;

	// シーン遷移
	std::unique_ptr<Fade> m_fade;

	// 選択テクスチャ回転
	float m_selectAngle;
	DirectX::SimpleMath::Vector2 m_selectPos;

	RECT m_stageTexturePos;
	RECT m_tankCountTexturePos;

public:
	SelectScene();
    ~SelectScene() override;

    void Initialize() override;
    void Update(float elapsedTime)override;
    void Render() override;
    void Finalize() override;

    SceneID GetNextSceneID() const;
};
