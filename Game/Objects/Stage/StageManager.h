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

class StageManager
{
public:
	// コンストラクタ
	StageManager();

	// デストラクタ
	~StageManager() = default;

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
	std::unique_ptr<FixedTurret> m_fixedTurret;						// 固定砲台

	// 配列サイズの宣言
	static const int STAGESIZE = 20;

	// 土台
	int m_base[STAGESIZE][STAGESIZE];

	// データを入れておくサイズ
	int m_data[STAGESIZE][STAGESIZE];

public:
	// プレイヤー情報のセット
	void SetObjectData(
		std::vector<Tank*> tanks,
		mylib::FollowCamera* camera
	);

	// ステージのデータを渡す
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
	FixedTurret* GetFixedTurret() { return m_fixedTurret.get(); }

	// 壁を上げる処理
	void MoveWall();

private:
	// 外部ファイルの読み込み
	void LoadFile();

	// ステージの生成
	void CreateStage();
};