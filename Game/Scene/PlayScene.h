/*
	@file	PlayScene.h
	@brief	プレイシーンクラス
*/
#pragma once
#include "IScene.h"

#include "Framework/Graphics.h"
#include "Game/Objects/Tank/Tank.h"
#include "Game/Objects/Other/SkySphere.h"
//#include "Libraries/MyLib/FollowCamera.h"

// 前方宣言
class CommonResources;

namespace mylib
{
	class DebugCamera;
	class GridFloor;
	class FollowCamera;
	class CollisionMesh;
}


class PlayScene final :
	public IScene
{
private:
	// 共通リソース
	CommonResources* m_commonResources;

	// デバッグカメラ
	std::unique_ptr<mylib::DebugCamera> m_debugCamera;

	// 格子床
	std::unique_ptr<mylib::GridFloor> m_gridFloor;

	// 射影行列
	DirectX::SimpleMath::Matrix m_projection;

	// シーンチェンジフラグ
	bool m_isChangeScene;

	DirectX::SimpleMath::Vector3 m_velocity;

	// ゲームオブジェクト
	std::unique_ptr<SkySphere> m_skySphere;
	std::unique_ptr<Tank> m_tank;
	std::unique_ptr<Tank> m_enemyTank;
	

	// 追従カメラ
	std::unique_ptr<mylib::FollowCamera> m_tpsCamera;

	// コリジョンメッシュ
	std::unique_ptr<mylib::CollisionMesh> m_collisionMesh;

public:
	PlayScene();
	~PlayScene() override;

	void Initialize(CommonResources* resources) override;
	void Update(float elapsedTime)override;
	void Render() override;
	void Finalize() override;

	SceneID GetNextSceneID() const;

private:
};
