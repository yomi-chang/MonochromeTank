/*
	@file	PlayScene.h
	@brief	プレイシーンクラス
*/
#pragma once
#include "IScene.h"

#include "Framework/Graphics.h"
#include "Game/Objects/Tank/Tank.h"
#include "Game/Objects/Other/SkySphere.h"
#include "Game/Objects/Stage/Wall.h"

namespace mylib
{
	class DebugCamera;
	class FollowCamera;
	class CollisionMesh;
}

class PlayScene final :
	public IScene
{
private:
	enum CameraType
	{
		TPS,
		DEBUG
	};

private:
	// グラフィックス
	Graphics* m_graphics;

	// デバッグカメラ
	std::unique_ptr<mylib::DebugCamera> m_debugCamera;

	// 追従カメラ
	std::unique_ptr<mylib::FollowCamera> m_tpsCamera;

	// カメラタイプ
	CameraType m_cameraType;

	// 射影行列
	DirectX::SimpleMath::Matrix m_projection;

	// シーンチェンジフラグ
	bool m_isChangeScene;

	DirectX::SimpleMath::Vector3 m_velocity;

	// ゲームオブジェクト
	std::unique_ptr<SkySphere> m_skySphere;
	std::unique_ptr<Tank> m_playerTank;
	std::vector<std::unique_ptr<Tank>> m_enemyTanks;
	
	// コリジョンメッシュ
	std::unique_ptr<mylib::CollisionMesh> m_collisionMesh;

	// ステージ
	std::vector<std::unique_ptr<Wall>> m_walls;

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
