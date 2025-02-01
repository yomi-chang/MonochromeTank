#pragma once
#include "Game/Objects/Tank/TankBase/Tank.h"

class Tank;

namespace mylib
{
	class SoundManager;
}

class SharedData final
{
public:
	// Graphicsクラスのインスタンスを取得する
	static SharedData* const GetInstance();

public:
	// デストラクタ
	~SharedData();
	// 初期化する
	void Initialize();
	
private:
	// コンストラクタ
	SharedData();

	// 代入は許容しない
	void operator=(const SharedData& object) = delete;
	// コピーコンストラクタは許容しない
	SharedData(const SharedData& object) = delete;

public:
	// 勝利した戦車情報の設定
	void SetWinnerTank(std::unique_ptr<Tank> tank) {
		m_winnerTank = std::move(tank);
	}
	// 勝利した戦車情報の取得
	Tank* GetWinnerTank() { return m_winnerTank.get(); }

	// セレクトのデータの設定
	void SetSelectData(int tankCount, std::string stageName)
	{
		m_tankCount = tankCount;
		m_stageName = stageName;
	}
	// 戦車の数の取得
	int GetTankCount() { return m_tankCount; }

	// ステージ名
	std::string GetStageName() { return m_stageName; }

	// サウンドマネージャの設定
	void SetSoundManager(mylib::SoundManager* soundManager) { m_soundManager = soundManager; }

	// サウンドマネージャの取得
	mylib::SoundManager* GetSoundManager() { return m_soundManager; }

private:
	// SharedDataクラスのインスタンスへのポインタ
	static std::unique_ptr<SharedData> m_sharedData;

	// サウンドマネージャー
	mylib::SoundManager* m_soundManager;

	// 勝利した戦車情報
	std::unique_ptr<Tank> m_winnerTank;

	// 戦車の数
	int m_tankCount;

	// ステージ
	std::string m_stageName;
};

