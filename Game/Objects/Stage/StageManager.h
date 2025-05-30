/**
 * @file   StageManager.h
 * @brief  ステージの管理クラス
 */
#pragma once
#include "Game/Objects/Stage/Floor.h"
#include "Game/Objects/Stage/Gimmick/WallGimmick.h"
#include "Game/Objects/FixedTurret/FixedTurret.h"

class Wall;
class Floor;
class SkySphere;
class WallGimmick;
class FixedTurret;
class Tank;

namespace mylib
{
	class FollowCamera;
}

// ステージ管理クラス
class StageManager
{
public:
	// コンストラクタ
	StageManager();
	// デストラクタ
	~StageManager();
	// 初期化処理
	void Initialize();
	// 更新処理
	void Update(float elapsedTime);
	// 描画処理
	void Render();

private:
	std::vector<std::unique_ptr<Wall>> m_walls;						// 壁
	std::unique_ptr<Floor> m_floor;									// 床
	std::unique_ptr<SkySphere> m_skySphere;							// 天球
	std::unique_ptr<WallGimmick> m_wallGimmick;						// 壁ギミック
	std::vector<std::unique_ptr<FixedTurret>> m_fixedTurrets;		// 固定砲台
	// 配列サイズの宣言
	static const int STAGESIZE = 20;
	// データを入れておくサイズ
	int m_data[STAGESIZE][STAGESIZE];

public:
	// プレイヤー情報のセット
	void SetObjectData(
		std::vector<Tank*> tanks,
		mylib::FollowCamera* camera
	);
	// 壁の情報を渡す
	std::vector<Wall*> GetWalls() 
	{
		std::vector<Wall*> wallPointers;
		for (auto& wall : m_walls)
		{
			wallPointers.push_back(wall.get());
		}
		return wallPointers;
	}
	// 壁ギミックの情報を渡す
	WallGimmick* GetWallGimmick() { return m_wallGimmick.get(); }
	// 固定砲台の情報を渡す
	std::vector<FixedTurret*> GetFixedTurrets()
	{
		std::vector<FixedTurret*> turretPointers;
		for (auto& fixedTurret : m_fixedTurrets)
		{
			turretPointers.push_back(fixedTurret.get());
		}
		return turretPointers;
	}
	// 壁を上げる処理
	void MoveWall();

private:
	// 外部ファイルの読み込み
	void LoadFile();
	// ステージの生成
	void CreateStage();
};