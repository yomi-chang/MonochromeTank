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
		FixedTurret* fixedTurret,
		std::vector<Wall*> walls,
		WallGimmick* wallGimmick
	)
	{
		m_tanks = tanks;
		m_camera = camera;
		m_fixedTurret = fixedTurret;
		m_walls = walls;
		m_wallGimmick = wallGimmick;
	}

private:
	// 各オブジェクト
	std::vector<Tank*> m_tanks;					// 戦車
	mylib::FollowCamera* m_camera;				// カメラ		
	FixedTurret* m_fixedTurret;				// 固定砲台
	std::vector<Wall*> m_walls;					// 壁
	WallGimmick* m_wallGimmick;					// 壁ギミック

public:

};