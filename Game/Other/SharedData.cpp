/**
 * @file   SharedData.cpp
 * @brief  共有情報管理クラス
 */
#include "pch.h"
#include "Game/Other/SharedData.h"

std::unique_ptr<SharedData> SharedData::m_sharedData = nullptr;

// インスタンスを取得する
SharedData* const SharedData::GetInstance()
{
	if (m_sharedData == nullptr)
	{
		// インスタンスを生成する
		m_sharedData.reset(new SharedData());
	}
	// インスタンスを返す
	return m_sharedData.get();
}

/// <summary>
/// コンストラクタ
/// </summary>
SharedData::SharedData()
	:
	m_winnerTank{},
	m_soundManager{},
	m_tankCount{},
	m_isFullScreen{}
{
}

/// <summary>
/// デストラクタ
/// </summary>
SharedData::~SharedData()
{
}