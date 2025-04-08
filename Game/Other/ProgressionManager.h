/*
	@file	ProgressionManager.h
	@brief	ゲームの進行管理クラス
*/
#pragma once

class Tank;

class ProgressionManager
{
public:
	// コンストラクタ
	ProgressionManager();
	// デストラクタ
	~ProgressionManager();
	// 初期化処理
	void Initialize(std::vector<Tank*> tanks);
	// 更新処理
	void Update();

private:
	// 戦車
	std::vector<Tank*> m_tanks;
};

