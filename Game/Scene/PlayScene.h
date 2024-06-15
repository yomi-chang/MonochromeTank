/*
	@file	PlayScene.h
	@brief	プレイシーンクラス
*/
#pragma once
#include "IScene.h"

#include "Framework/Graphics.h"
#include "Game/Objects/Tank/Tank.h"

// 前方宣言
class CommonResources;

namespace mylib
{
	class DebugCamera;
	class GridFloor;
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

	// モデル
	DirectX::Model* bodyModel;
	DirectX::Model* canonModel;
	DirectX::Model* turretModel;

	DirectX::Model* m_skyModel;

	// 回転角(度)
	float m_angle;

	float m_canonAngle;
	float m_turretAngle;

	DirectX::SimpleMath::Vector3 m_bodyPosition;
	DirectX::SimpleMath::Vector3 m_turretPosition;
	DirectX::SimpleMath::Vector3 m_canonPosition;

	DirectX::SimpleMath::Vector3 m_velocity;

	float m_speed;

	// ゲームオブジェクト
	std::unique_ptr<Tank> m_tank;

public:
	PlayScene();
	~PlayScene() override;

	void Initialize(CommonResources* resources) override;
	void Update(float elapsedTime)override;
	void Render() override;
	void Finalize() override;

	SceneID GetNextSceneID() const;

private:
	void KeyBoardEvent();
};
