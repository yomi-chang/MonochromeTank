#pragma once

class Tank;

class Patrol
{
public:
	Patrol();
	~Patrol() = default;

	void Initialize(
		std::vector<DirectX::SimpleMath::Vector3> patrolPoints,
		Tank* tank
	);
	void Update(float elapsedTime);

private:
	// 巡回地点
	std::vector<DirectX::SimpleMath::Vector3> m_patrolPoints;

	// 巡回地点の数
	int m_patrolPointVelue;

	// 現在の巡回番号
	unsigned int m_currentPoint;

	// 自機の情報
	Tank* m_tank;

	// 戦車の速度
	const float TANK_SPEED = 2.0f;

	// タイマー
	float m_time;

public:
	// 巡回地点の登録
	void SetPatrolPoints(std::vector<DirectX::SimpleMath::Vector3> patrolPoints)
	{
		m_patrolPoints = patrolPoints;
	}

	// 巡回地点の追加
	void AddPatrolPoint(DirectX::SimpleMath::Vector3 point)
	{
		m_patrolPoints.push_back(point);
		m_patrolPointVelue++;
	}

	// 巡回地点の削除
	void ClearPatrolPoints()
	{
		m_patrolPoints.clear();
	}
};