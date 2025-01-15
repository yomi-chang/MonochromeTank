#include "pch.h"
#include "Game/EnemyAi/Patrol.h"
#include "Game/Objects/Tank/EnemyTanks/EnemyTank.h"

//-------------------------------------------------------------------
// コンストラクタ
//-------------------------------------------------------------------
Patrol::Patrol()
	:
	m_patrolPoints{},
	m_patrolPointVelue{},
	m_currentPoint{},
	m_tank{},
	m_time{}
{
}

//-------------------------------------------------------------------
// 初期化処理
//-------------------------------------------------------------------
void Patrol::Initialize(
	std::vector<DirectX::SimpleMath::Vector3> patrolPoints,
	Tank* tank
)
{
	// 巡回地点の登録
	m_patrolPoints = patrolPoints;

	// 巡回地点数の取得
	m_patrolPointVelue = m_patrolPoints.size();

	// 自機の取得
	m_tank = tank;
}

//-------------------------------------------------------------------
// 更新処理
//-------------------------------------------------------------------
void Patrol::Update(float elapsedTime)
{
	using namespace DirectX::SimpleMath;

	// 探している挙動
	m_time += elapsedTime;
	m_tank->GetTurret()->RotateTurret(sinf(m_time) / 2.0f);



	// 進行方向ベクトル
	Vector3 heading = Vector3::Transform(Vector3::Forward * TANK_SPEED * elapsedTime, m_tank->GetRotation());

	// ゴールへ向かうベクトル
	Vector3 toGoal = m_patrolPoints[m_currentPoint] - m_tank->GetPosition();

	// 移動処理
	m_tank->GetBody()->Move(heading);

	/*
		自機をターゲットの方向へ徐々に回転する
	*/
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
	// ZX平面上にあるベクトルの向きはYのプラスマイナスで判断する
	if (heading.Cross(toGoal).y < 0.0f)
	{
		theta *= (-1.0f);
	}

	// 角度を更新する
	m_tank->GetBody()->Rotate(Quaternion::CreateFromYawPitchRoll(DirectX::XMConvertToRadians(theta), 0.0f, 0.0f));


	// ゴールに達したら、ゴール情報を更新する
	if (toGoal.Length() < 1.0f)
	{
		m_currentPoint++;
		m_currentPoint %= m_patrolPointVelue;
	}
}


