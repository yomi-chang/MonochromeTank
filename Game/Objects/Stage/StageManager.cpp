#include"pch.h"

#include<iostream>
#include<fstream>
#include<sstream>
#include<regex>

#include "Framework/Resources.h"

#include "Game/Objects/Stage/StageManager.h"
#include "Game/Objects/Stage/Wall.h"
#include "Game/Objects/Stage/Floor.h"
#include "Game/Objects/Other/SkySphere.h"

#include "Game/Objects/Tank/TankBase/Tank.h"

//---------------------------------------------------------
// コンストラクタ
//---------------------------------------------------------
StageManager::StageManager()
	:
	m_walls{},
	m_floor{},
	m_skySphere{}
{
}

//---------------------------------------------------------
// 初期化処理
//---------------------------------------------------------
void StageManager::Initialize()
{
	// 天球の生成
	m_skySphere = std::make_unique<SkySphere>();

	// CSVファイルの読み込み
	LoadFile();

	// ステージの生成
	CreateStage();
}

//---------------------------------------------------------
// 描画処理
//---------------------------------------------------------
void StageManager::Render()
{
	// 天球の描画
	m_skySphere->Render();

	// ステージの描画
	for (auto& wall : m_walls)
	{
		wall->Render();
	}
	m_floor->Render();
}

// 壁を上げる処理
void StageManager::UpWall()
{
}

//---------------------------------------------------------
// CSVファイルの読み込み
//---------------------------------------------------------
void StageManager::LoadFile()
{
	std::string str[STAGESIZE];

	// ファイルの読み込み
	std::ifstream file("Resources\\Stage\\Stage1.csv");

	// 正常に読み込めたかの確認
	if (!file.is_open())
		return;

	for (int y = 0; y < STAGESIZE; y++)
	{
		// 一行分のデータの読み込み
		file >> str[y];

		std::string tmp = std::regex_replace(str[y], std::regex(","), " ");

		std::istringstream stream(tmp);

		for (int x = 0; x < STAGESIZE; x++)
		{
			int num = -1;

			stream >> num;

			m_data[y][x] = num;
		}
	}
}

//---------------------------------------------------------
// ステージの生成
//---------------------------------------------------------
void StageManager::CreateStage()
{
	using namespace DirectX::SimpleMath;

	// 土台の作成
	//m_walls.emplace_back(std::make_unique<Wall>(Vector3(40.0f, 1.0f, 40.0f), Vector3::Zero));
	m_floor = std::make_unique<Floor>(STAGESIZE);
	m_floor->SetTexture(Resources::GetInstance()->GetFloorTexture());

	for (int y = 0; y < STAGESIZE; y++)
	{
		for (int x = 0; x < STAGESIZE; x++)
		{
			// 土台の描画
			//m_walls.emplace_back(std::make_unique<Wall>(Vector3::One, Vector3(x - (STAGESIZE / 2), 0.0f, y - (STAGESIZE / 2))));

			// 数値に応じたオブジェクトの生成
			switch (m_data[y][x])
			{
				case 0:		// 情報がない場合
					m_walls.emplace_back(std::make_unique<Wall>(Vector3::One, Vector3(x - (STAGESIZE / 2), -1.0f, y - (STAGESIZE / 2))));
					continue;
				case 1:		// 壁
					m_walls.emplace_back(std::make_unique<Wall>(Vector3::One, Vector3(x - (STAGESIZE / 2), 0.5f, y - (STAGESIZE / 2))));
				case 2:		// ギミック用壁
					m_walls.emplace_back(std::make_unique<Wall>(Vector3::One, Vector3(x - (STAGESIZE / 2), -1.0f, y - (STAGESIZE / 2))));
				default:
					break;
			}
		}
	}
}

//---------------------------------------------------------
// オブジェクトデータの受け渡し
//---------------------------------------------------------
void StageManager::SetObjectData(
	std::vector<Tank*> tanks,
	mylib::FollowCamera* camera
)
{
	for (auto& wall : m_walls)
	{
		wall->SetTanks(tanks);
		wall->SetCamera(camera);
	}
}
