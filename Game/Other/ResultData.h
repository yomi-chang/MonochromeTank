#pragma once
#include "Game/Objects/Tank/TankBase/Tank.h"

class Tank;

class ResultData final
{
public:
	// Graphicsクラスのインスタンスを取得する
	static ResultData* const GetInstance();

public:
	// デストラクタ
	~ResultData();
	// 初期化する
	void Initialize();
	
private:
	// コンストラクタ
	ResultData();

	// 代入は許容しない
	void operator=(const ResultData& object) = delete;
	// コピーコンストラクタは許容しない
	ResultData(const ResultData& object) = delete;

public:
	// 勝利した戦車情報の設定
	void SetWinnerTank(std::unique_ptr<Tank> tank) {
		m_winnerTank = std::move(tank);
	}
	// 勝利した戦車情報の取得
	Tank* GetWinnerTank() { return m_winnerTank.get(); }

private:
	// Graphicsクラスのインスタンスへのポインタ
	static std::unique_ptr<ResultData> m_resultData;

	// 勝利した戦車情報
	std::unique_ptr<Tank> m_winnerTank;
};

