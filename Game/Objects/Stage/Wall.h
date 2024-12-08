#pragma once
#include "Game/Collider/BoxCollider.h"

namespace mylib
{
	class FollowCamera;
}
class Tank;

class Wall
{
public:
	// 壁の種類
	enum WallType
	{
		FIXED,		// 固定
		MOVE		// 移動
	};

public:
	void SetTanks(std::vector<Tank*> tanks) { m_tanks = tanks; }
	void SetCamera(mylib::FollowCamera* camera) { m_camera = camera; }

public:
	// コンストラクタ
	Wall(
		DirectX::SimpleMath::Vector3 scale,
		DirectX::SimpleMath::Vector3 position,
		WallType type
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
	std::vector<Tank*> m_tanks;									// 戦車情報
	mylib::FollowCamera* m_camera;								// カメラ
	Graphics* m_graphics;										// グラフィックス	
	DirectX::SimpleMath::Vector4 m_color;						// カラー
	WallType m_wallType;										// 壁の種類
private:
	// 衝突判定
	void DetectCollision();
};