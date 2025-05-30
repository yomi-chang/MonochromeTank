/**
 * @file   ProgressionManager.h
 * @brief  ゲームの進行管理クラス
 */
#pragma once

class EnemyTank;
class Messenger;

class ProgressionManager
{
public:
	// コンストラクタ
	ProgressionManager();
	// デストラクタ
	~ProgressionManager();
	// 初期化処理
	void Initialize(std::vector<EnemyTank*> tanks);
	// 更新処理
	void Update();

private:
	// 全敵戦車
	std::vector<EnemyTank*> m_tanks;
	// メッセンジャー
	Messenger* m_messenger;
	// 残り戦車数
	int m_tankCount;
	// 一騎打ちになったかのフラグ
	bool m_duelFlag;

public:
	// 残り戦車情報の設定
	void SetTankCount(int count){m_tankCount = count;}

private:
	// 巡回範囲を狭める
	void NarrowPatrolPoints();

	// ゲーム終盤
	void HandleEndgamePhase();
};