#include "pch.h"
#include "Game/EnemyAi/RetreatAttack.h"

/// <summary>
/// コンストラクタ
/// </summary>
RetreatAttack::RetreatAttack()
    : 
    m_stateID(StateID::RETREAT_ATTACK),
    m_tank(),
    m_targetTank(),
    m_moveTimer(),
    m_shotTimer()
{
}

/// <summary>
/// デストラクタ
/// </summary>
RetreatAttack::~RetreatAttack() 
{
}

/// <summary>
/// 初期化処理
/// </summary>
/// <param name="tank">戦車情報</param>
void RetreatAttack::Initialize(Tank* tank)
{
    m_tank = tank;
    m_moveTimer = BACK_MOVE_TIME;
    m_shotTimer = 0.0f;
}

/// <summary>
/// 行動状態遷移をした際に呼び出される関数
/// </summary>
void RetreatAttack::Enter()
{
    m_moveTimer = BACK_MOVE_TIME;
    m_shotTimer = 0.0f;
}

/// <summary>
/// 更新処理
/// </summary>
/// <param name="elapsedTime"></param>
void RetreatAttack::Update(float elapsedTime)
{
    if (m_targetTank == nullptr) return;

    // 追跡対象の方向に向く処理
    LookAtTarget(elapsedTime);
    // 射撃行動
    ShotAction(elapsedTime);
    // 後退行動
    RetreatMove(elapsedTime);

    m_moveTimer -= elapsedTime;
    if (m_moveTimer <= 0.0f)
    {
        // 通常の追跡に戻す
        Messenger::GetInstance()->Dispatch(m_tank->GetTankNumber(), Message::TRACKING);
    }
}

/// <summary>
/// 追跡対象の方向に向く処理
/// </summary>
/// <param name="elapsedTime"></param>
void RetreatAttack::LookAtTarget(float elapsedTime)
{
    using namespace DirectX::SimpleMath;

    // パラメータの受け取り
    const auto& parameter = Parameter::GetInstance();
    // 敵の方向ベクトルの計算
    Vector3 delta = m_tank->GetPosition() - m_targetTank->GetPosition();
    float angleRadians = atan2(delta.x, delta.z);
    // 車体の回転を考慮して目標の角度を計算
    float targetAngle = angleRadians - m_tank->GetRotation().ToEuler().y;
    // 砲塔回転の制限
    float min = parameter->GetTurretAngleMin();
    float max = parameter->GetTurretAngleMax();
    targetAngle = mylib::Clamp(targetAngle, min, max);
    // 現在の砲塔の回転角度
    float currentAngle = m_tank->GetTurret()->GetTurretRotation().ToEuler().y;
    // 目標角度と現在の角度との差を求め、Lerp補間で回転
    float angleDifference = targetAngle - currentAngle;
    // ゆっくり回転するための速度制御
    float rotationSpeed = parameter->GetEnemyRotationSpeed() * elapsedTime;
    // 補間後の回転角度
    float newAngle = currentAngle + angleDifference * rotationSpeed;
    // 砲塔の回転
    m_tank->GetTurret()->RotateTurret(newAngle);
}

/// <summary>
/// 後退攻撃処理
/// </summary>
/// <param name="elapsedTime"></param>
void RetreatAttack::RetreatMove(float elapsedTime)
{
    using namespace DirectX::SimpleMath;

    // 速度の設定
    float speed = Parameter::GetInstance()->GetEnemySpeed() * elapsedTime * 0.6f; // 後退は少し遅く
    Vector3 backward = Vector3::Backward * speed;
    Vector3 velocity = Vector3::Transform(backward, m_tank->GetRotation());

    // 移動処理
    m_tank->GetBody()->Move(velocity);
}

void RetreatAttack::ShotAction(float elapsedTime)
{
    m_shotTimer += elapsedTime;

    if (m_shotTimer >= SHOT_INTERVAL)
    {
        m_shotTimer = 0.0f;
        m_tank->GetCannon()->StartReload();
        m_tank->GetCannon()->Shoot();
        m_tank->GetCannon()->FinishShoot();
    }
}