#pragma once

class Tank;
class FixedTurret;
class Wall;
class WallGimmick;

namespace mylib
{
	class FollowCamera;
}


class CollisionManager
{
public:
	CollisionManager();
	~CollisionManager() = default;
	// 更新処理
	void Update();

public:
	// オブジェクトの設定
	void SetObjectData(
		std::vector<Tank*> tanks,
		mylib::FollowCamera* camera,
		std::vector<FixedTurret*> fixedTurrets,
		std::vector<Wall*> walls,
		WallGimmick* wallGimmick
	)
	{
		m_tanks = tanks;
		m_camera = camera;
		m_fixedTurrets = fixedTurrets;
		m_walls = walls;
		m_wallGimmick = wallGimmick;
	}
private:
	// 各オブジェクト
	std::vector<Tank*> m_tanks;					// 戦車
	mylib::FollowCamera* m_camera;				// カメラ		
	std::vector<FixedTurret*> m_fixedTurrets;	// 固定砲台
	std::vector<Wall*> m_walls;					// 壁
	WallGimmick* m_wallGimmick;					// 壁ギミック

private:
	// 戦車と連射弾の当たり判定
	void DetectCollisionTankAndNomalBullets();
	// 戦車と砲弾の当たり判定
	void DetectCollisionTankAndCannonBall();
	// 戦車同士の当たり判定
	void DetectCollisionTankAndOtherTanks();
	// 戦車と壁の当たり判定
	void DetectCollisionTankAndWalls();
	// 弾と壁の当たり判定
	void DetectCollisionBulletsAndWalls();
	// Rayと壁の当たり判定
	void DetectCollisionRayAndWalls();
	// 壁の回避
	void DetectCollisionWallAndAvoidCollider();
};