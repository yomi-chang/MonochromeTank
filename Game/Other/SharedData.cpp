#include "pch.h"
#include "Game/Other/SharedData.h"

std::unique_ptr<SharedData> SharedData::m_sharedData = nullptr;

// グラフィックスのインスタンスを取得する
SharedData* const SharedData::GetInstance()
{
	if (m_sharedData == nullptr)
	{
		// グラフィックスのインスタンスを生成する
		m_sharedData.reset(new SharedData());
	}
	// グラフィックスのインスタンスを返す
	return m_sharedData.get();
}

// コンストラクタ
SharedData::SharedData()
	:
	m_winnerTank{},
	m_soundManager{}
{
}

// デストラクタ
SharedData::~SharedData()
{
}

// 初期化する
void SharedData::Initialize()
{
	
}