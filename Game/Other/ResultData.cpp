#include "pch.h"
#include "Game/Other/ResultData.h"

std::unique_ptr<ResultData> ResultData::m_resultData = nullptr;

// グラフィックスのインスタンスを取得する
ResultData* const ResultData::GetInstance()
{
	if (m_resultData == nullptr)
	{
		// グラフィックスのインスタンスを生成する
		m_resultData.reset(new ResultData());
	}
	// グラフィックスのインスタンスを返す
	return m_resultData.get();
}

// コンストラクタ
ResultData::ResultData()
	:
	m_winnerTank{}
{
}

// デストラクタ
ResultData::~ResultData()
{
}

// 初期化する
void ResultData::Initialize()
{
	
}