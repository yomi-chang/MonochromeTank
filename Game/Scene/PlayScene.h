/*
	@file	PlayScene.h
	@brief	プレイシーンクラス
*/
#pragma once
#include "IScene.h"

class Graphics;
class PlayerTank;
class SimpleTank;
class StageManager;

class Magazine;

namespace mylib
{
	class DebugCamera;
	class FollowCamera;
}

class PlayScene final :
	public IScene
{
private:
	// カメラの種類
	enum CameraType
	{
		TPS,
		DEBUG
	};

private:
	Graphics* m_graphics;									// グラフィックス

	bool m_isChangeScene;									// シーンチェンジフラグ

	// カメラ関係
	std::unique_ptr<mylib::DebugCamera> m_debugCamera;		// デバッグカメラ
	std::unique_ptr<mylib::FollowCamera> m_tpsCamera;		// 追従カメラ
	CameraType m_cameraType;								// カメラタイプ

	// オブジェクト
	std::unique_ptr<PlayerTank> m_player;					// プレイヤー
	//std::unique_ptr<SimpleTank> m_enemy;					// 敵戦車

	std::vector<std::unique_ptr<SimpleTank>> m_enemies;		// 敵戦車(配列管理)

	// マネージャー
	std::unique_ptr<StageManager> m_stageManager;			// ステージマネージャー

	// 残弾数UI
	std::unique_ptr<Magazine> m_magazine;
public:
	PlayScene();
	~PlayScene() override;

	void Initialize() override;
	void Update(float elapsedTime)override;
	void Render() override;
	void Finalize() override;

	SceneID GetNextSceneID() const;

private:
	// デバッグカメラと切り替える
	void ChangeCameraType();
};
