/**
 * @file   Patrol.cpp
 * @brief  敵の巡回行動処理クラス
 */
#include "pch.h"
#include "Game/EnemyAi/Patrol.h"

/// <summary>
/// コンストラクタ
/// </summary>
Patrol::Patrol()
	:
	m_stateID{StateID::PATROL},
	m_patrolPoints{},
	m_patrolPointVelue{},
	m_currentPoint{},
	m_tank{},
	m_targetTank{},
	m_otherTanks{},
	m_time{}
{
}

/// <summary>
/// デストラクタ
/// </summary>
Patrol::~Patrol()
{
}

/// <summary>
/// 初期化処理
/// </summary>
/// <param name="tank">戦車情報</param>
void Patrol::Initialize(Tank* tank)
{
	// 自機の取得
	m_tank = tank;
}

/// <summary>
/// 更新処理
/// </summary>
/// <param name="elapsedTime">フレーム間の経過時間</param>
void Patrol::Update(float elapsedTime)
{
	using namespace DirectX::SimpleMath;

	// 速度の受け取り
	float speed = Parameter::GetInstance()->GetEnemySpeed();
	speed *= elapsedTime;
	// 探している挙動
	m_time += elapsedTime;
	m_tank->GetTurret()->RotateTurret(sinf(m_time) / 2.0f);
	// 進行方向ベクトル
	Vector3 heading = Vector3::Transform(Vector3::Forward * speed, m_tank->GetRotation());
	// ゴールへ向かうベクトル
	Vector3 toGoal = m_patrolPoints[m_currentPoint] - m_tank->GetPosition();
	// 移動処理
	m_tank->GetBody()->Move(heading);

	//　自機をターゲットの方向へ徐々に回転する
	// 「自機の進行方向ベクトル」と「ターゲットの方向」からcosθを計算する
	float cosTheta = heading.Dot(toGoal) / (toGoal.Length() * heading.Length());
	// acosの引数で指定できる範囲は「-1～1」なので、値を補正する
	cosTheta = std::max(-1.0f, std::min(cosTheta, 1.0f));
	// cosθからθを計算する
	// acosの結果は「0～π」
	float theta = std::acos(cosTheta);
	//１フレームでの回転角を制限値以内に補正する
	theta = std::min(10.0f, theta);
	// 右側に行きたい場合は角度の符号を付け替える
	if (heading.Cross(toGoal).y < 0.0f)
	{
		theta *= (-1.0f);
	}
	// 角度を更新する
	m_tank->GetBody()->Rotate(Quaternion::CreateFromYawPitchRoll(DirectX::XMConvertToRadians(theta), 0.0f, 0.0f));

	// ゴールに達したら、ゴール情報を更新する
	if (toGoal.Length() < 0.5f)
	{
		m_currentPoint++;
		m_currentPoint %= m_patrolPointVelue;
	}

	// 壁に当たっていたら壁の回避行動の遷移
	if (m_tank->GetAvoidWall())
	{
		Messenger::GetInstance()->Dispatch(m_tank->GetTankNumber(), Message::AVOIDWALL);
	}

	// 索敵
	this->ScoutOtherTank();
}

/// <summary>
/// 行動状態遷移をした際に呼び出される関数
/// </summary>
void Patrol::Enter()
{
}

/// <summary>
/// 巡回地点の追加
/// </summary>
/// <param name="point">巡回地点</param>
void Patrol::AddPatrolPoint(const DirectX::SimpleMath::Vector3& point)
{
	m_patrolPoints.push_back(point);
	m_patrolPointVelue++;
}

/// <summary>
/// 巡回ルートの削除
/// </summary>
void Patrol::ClearPatrolPoints()
{
	m_patrolPoints.clear();
}

/// <summary>
/// 巡回ルートの登録
/// </summary>
/// <param name="patrolPoints">巡回ルート</param>
void Patrol::SetPatrolPoints(const std::vector<DirectX::SimpleMath::Vector3>& patrolPoints)
{
	// 巡回地点の削除
	this->ClearPatrolPoints();

	// 巡回地点の登録
	m_patrolPoints = patrolPoints;

	// 巡回地点数の取得
	m_patrolPointVelue = static_cast<int>(m_patrolPoints.size());
}

/// <summary>
/// 他戦車の索敵
/// </summary>
void Patrol::ScoutOtherTank()
{
	// 追跡中の戦車がいるなら早期リターン
	if (m_tank->GetTargetTank() != nullptr) { return; }

	// 他の戦車が一定範囲に存在するなら追跡対象にして追跡行動に遷移
	for (auto& otherTank : m_otherTanks)
	{
		// 自機の場合か相手が破壊されている場合は処理しない
		if (otherTank->GetTankNumber() == m_tank->GetTankNumber() ||
			otherTank->GetHp() <= 0)
		{
			continue;
		}

		// 距離の確認
		float distance = (otherTank->GetPosition() - m_tank->GetPosition()).LengthSquared();
		if (distance <= Parameter::GetInstance()->GetScoutRadius())
		{
			// 追跡対象の設定
			m_targetTank = otherTank;
			// 追跡行動にする
			Messenger::GetInstance()->Dispatch(m_tank->GetTankNumber(), Message::TRACKING);
			break;
		}
	}
}


