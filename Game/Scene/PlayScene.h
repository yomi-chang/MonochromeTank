/*
	@file	PlayScene.h
	@brief	プレイシーンクラス
*/
#pragma once
#include "IScene.h"
#include <PostProcess.h>

class Graphics;
class PlayerTank;
class EnemyTank;
class StageManager;
class CollisionManager;

class MagazineUi;
class Fade;

namespace mylib
{
	class DebugCamera;
	class FollowCamera;
}

namespace DX
{
	class RenderTexture;
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

	std::vector<std::unique_ptr<EnemyTank>> m_enemies;		// 敵戦車(配列管理)

	// マネージャー
	std::unique_ptr<StageManager> m_stageManager;			// ステージマネージャー
	std::unique_ptr<CollisionManager> m_collisonManager;	// コリジョンマネージャー

	// 残弾数UI
	std::unique_ptr<MagazineUi> m_magazine;

	// シーン遷移
	std::unique_ptr<Fade> m_fade;
	bool m_isStart; 

	// レンダーテクスチャ
	std::unique_ptr<DX::RenderTexture> m_renderTexture;
	// ポストプロセス
	std::unique_ptr<DirectX::BasicPostProcess> m_postProcess;


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
