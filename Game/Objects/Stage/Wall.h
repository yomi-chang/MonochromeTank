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
	void SetPlayer(Tank* player) { m_player = player; }

	void SetCamera(mylib::FollowCamera* camera) { m_camera = camera; }

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
	// ジオメトリックプリミティブ
	std::unique_ptr<DirectX::GeometricPrimitive> m_model;

	// ワールド座標
	DirectX::SimpleMath::Matrix m_world;

	// ボックスコライダー
	std::unique_ptr<BoxCollider> m_collider;

	// プレイヤー
	Tank* m_player;

	// カメラ
	mylib::FollowCamera* m_camera;

	// グラフィックス
	Graphics* m_graphics;

	// カラー
	DirectX::SimpleMath::Vector4 m_color;
};