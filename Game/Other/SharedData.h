#pragma once
#include "Game/Objects/Tank/TankBase/Tank.h"

class Tank;

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

private:
	// Graphicsクラスのインスタンスへのポインタ
	static std::unique_ptr<SharedData> m_sharedData;

	// 勝利した戦車情報
	std::unique_ptr<Tank> m_winnerTank;
};

