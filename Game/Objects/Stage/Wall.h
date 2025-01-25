#pragma once
#include "Game/Collider/BoxCollider.h"

namespace mylib
{
	class FollowCamera;
}

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
	// 更新処理
	void Update(float elapsedTime);
	// 描画処理
	void Render();
	// 終了処理
	void Finalize();
	// コライダーの取得
	DirectX::BoundingBox* GetBoundingBox() { return m_collider->GetBoundingBox(); }
	// コライダーのポインタの取得
	BoxCollider* GetCollider() { return m_collider.get(); }
	// 壁の種類の取得
	WallType GetWallType() { return m_wallType; }
	// 沈んでいる壁を出すかどうか
	void SetActiveWall(bool b) { m_isActive = b; }
	// 壁が上がっているかどうかの取得
	bool GetActiveWall() { return m_isActive; }

private:
	std::unique_ptr<DirectX::GeometricPrimitive> m_model;		// ジオメトリックプリミティブ
	DirectX::SimpleMath::Vector3 m_position;					// 座標	
	std::unique_ptr<BoxCollider> m_collider;					// コライダー
	mylib::FollowCamera* m_camera;								// カメラ
	Graphics* m_graphics;										// グラフィックス	
	DirectX::SimpleMath::Vector4 m_color;						// カラー
	WallType m_wallType;										// 壁の種類
	bool m_isActive;
private:
	// 衝突判定
	void DetectCollision();
	void RaiseWall(float elapsedTime);											// 壁をゆっくり上げる処理
	void LowerWall(float elapsedTime);											// 壁をゆっくり下げる処理
};