/**
 * @file   ProgressionManager.h
 * @brief  ゲームの進行管理クラス
 */
#pragma once

class EnemyTank;
class PlayerTank;
class Messenger;

class ProgressionManager
{
public:
	// コンストラクタ
	ProgressionManager();
	// デストラクタ
	~ProgressionManager();
	// 初期化処理
	void Initialize(std::vector<EnemyTank*> tanks,PlayerTank* playerTank);
	// 更新処理
	void Update();

private:
	// 全敵戦車
	std::vector<EnemyTank*> m_tanks;
	// プレイヤー戦車
	PlayerTank* m_playerTank;
	// メッセンジャー
	Messenger* m_messenger;
	// 残り戦車数
	int m_tankCount;
	// 巡回範囲を縮小したかどうか
	bool m_flag;
	// プレイヤーがやられているか
	bool m_playerAlive;

public:
	// 残り戦車情報の設定
	void SetTankCount(int count){m_tankCount = count;}

	// プレイヤーがやられているかの情報を受け取る
	void PlayerBreak() { m_playerAlive = false; }

private:
	// 巡回範囲を狭める
	void NarrowPatrolPoints();

	// ゲーム終盤
	void HandleEndgamePhase();
};