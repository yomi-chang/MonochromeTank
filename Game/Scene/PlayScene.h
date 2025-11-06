/**
 * @file   PlayScene.h
 * @brief  プレイシーンクラス
 */
#pragma once
#include "Interface/IScene.h"
#include <PostProcess.h>

class Graphics;
class PlayerTank;
class EnemyTank;
class StageManager;
class CollisionManager;
class ProgressionManager;

class MagazineUi;
class Fade;
class DamageEffect;
class StageEffect;
class PauseMenu;
class Button;

namespace mylib
{
	class DebugCamera;
	class LockOnCamera;
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
		DEATH,
		DEBUG
	};

	// UI座標
	static constexpr RECT SKIP_UI_POS = { 1050,580,1260,700 };
	// フォントサイズ
	static constexpr int FONT_SIZE_X = 90;
	static constexpr int FONT_SIZE_Y = 150;

public:
	// コンストラクタ
	PlayScene();
	// デストラクタ
	~PlayScene() override;
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
	Graphics* m_graphics;									// グラフィックス
	bool m_isChangeScene;									// シーンチェンジフラグ

	// カメラ関係
	std::unique_ptr<mylib::LockOnCamera> m_deathCamera;		// デスカメラ
	std::unique_ptr<mylib::DebugCamera> m_debugCamera;		// デスカメラ
	std::unique_ptr<mylib::FollowCamera> m_tpsCamera;		// 追従カメラ
	CameraType m_cameraType;								// カメラタイプ

	// オブジェクト
	std::unique_ptr<PlayerTank> m_player;					// プレイヤー
	std::vector<std::unique_ptr<EnemyTank>> m_enemies;		// 敵戦車(配列管理)
	std::vector<ITank*> m_allTanks;							// 全戦車情報のマップ

	// マネージャー
	std::unique_ptr<StageManager> m_stageManager;				// ステージマネージャー
	std::unique_ptr<CollisionManager> m_collisonManager;		// コリジョンマネージャー
	std::unique_ptr<ProgressionManager> m_progressionManager;	// 進行管理マネージャー

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
	// ポーズ画面
	std::unique_ptr<PauseMenu> m_pauseMenu;
	// 時間
	float m_time;
	// 生存している敵戦車
	int m_surviveEnemyTank;

	// ボタン
	std::vector<std::unique_ptr<Button>> m_buttons;


private:
	// マウスの制限解除
	void UnDisableMouse();
	// オブジェクトの生成
	void CreateObjects();
	// ボタンの作成
	void CreateButton();
	// 戦車の生成
	void CreateTanks();
	// UIの描画
	void DrawUi();
};