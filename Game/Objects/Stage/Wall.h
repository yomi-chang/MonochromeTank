#pragma once
#include "Game/Collider/BoxCollider.h"

namespace mylib
{
	class FollowCamera;
}
class PlayerTank;
class EnemyTank;

class Wall
{
public:
	void SetPlayer(PlayerTank* tank) { m_playerTank = tank; }

	void SetCamera(mylib::FollowCamera* camera) { m_camera = camera; }

	void SetEnemyTanks(std::vector<EnemyTank*> enemyTanks) { m_enemyTanks = enemyTanks; }

public:
	// コンストラクタ
	Wall(
		DirectX::SimpleMath::Vector3 scale,
		DirectX::SimpleMath::Vector3 movePosition
	);
	// デストラクタ
	~Wall();
	// 描画処理
	void Render();
	// 終了処理
	void Finalize();
	// コライダーの取得
	DirectX::BoundingBox* GetBoundingBox() { return m_collider->GetBoundingBox(); }

private:
	std::unique_ptr<DirectX::GeometricPrimitive> m_model;		// ジオメトリックプリミティブ
	DirectX::SimpleMath::Matrix m_world;						// ワールド座標	
	std::unique_ptr<BoxCollider> m_collider;					// ボックスコライダー
	PlayerTank* m_playerTank;									// プレイヤー
	mylib::FollowCamera* m_camera;								// カメラ
	std::vector<EnemyTank*> m_enemyTanks;						// 敵戦車
	Graphics* m_graphics;										// グラフィックス	
	DirectX::SimpleMath::Vector4 m_color;						// カラー

private:
	// 衝突判定
	void DetectCollision();
};