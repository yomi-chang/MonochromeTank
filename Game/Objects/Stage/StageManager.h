#pragma once
#include "Game/Objects/Stage/Floor.h"

class Wall;
class Floor;
class SkySphere;
class PlayerTank;
class EnemyTank;

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

	// 描画処理
	void Render();

private:
	std::vector<std::unique_ptr<Wall>> m_walls;		// 壁
	std::unique_ptr<Floor> m_floor;
	std::unique_ptr<SkySphere> m_skySphere;			// 天球

	// 配列サイズの宣言
	static const int STAGESIZE = 40;

	// 土台
	int m_base[STAGESIZE][STAGESIZE];

	// データを入れておくサイズ
	int m_data[STAGESIZE][STAGESIZE];

public:
	// プレイヤー情報のセット
	void SetObjectData(
		PlayerTank* playerTank,
		mylib::FollowCamera* camera,
		std::vector<EnemyTank*> enemyTanks
	);

private:
	// 外部ファイルの読み込み
	void LoadFile();

	// ステージの生成
	void CreateStage();
};