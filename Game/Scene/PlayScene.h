/*
	@file	PlayScene.h
	@brief	プレイシーンクラス
*/
#pragma once
#include "Interface/IScene.h"
#include <PostProcess.h>

class Graphics;
class PlayerTank;
class EnemyTank;
class StageManager;
class CollisionManager;

class MagazineUi;
class Fade;
class DamageEffect;
class StageEffect;

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

	// ダメージ演出時間
	const float DAMAGE_EFFECT_TIME = 1.0f;

	const RECT SKIP_UI_POS = { 1050,580,1260,700 };

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

	// スキップUI
	ID3D11ShaderResourceView* m_skipTexture;

	// ダメージエフェクト
	std::unique_ptr<DamageEffect> m_damageEffect;

	// ステージエフェクト
	std::unique_ptr<StageEffect> m_stageEffect;

	// 時間
	float m_time;

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

	void CreateTanks();
};
