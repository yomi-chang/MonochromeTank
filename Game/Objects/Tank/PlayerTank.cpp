#include "pch.h"
#include "Game/Objects/Tank/PlayerTank.h"

#include "Game/Objects/Tank/TankBase/Tank.h"
#include "Game/Objects/Tank/TankBase/TankBody.h"
#include "Game/Objects/Tank/TankBase/TankTurret.h"
#include "Game/Objects/Tank/TankBase/TankCannon.h"

#include "Interface/IBullet.h"
#include "Game/UserInterface/HpGauge.h"

#include "Framework/InputManager.h"
#include "Libraries/MyLib/Math.h"
#include "Libraries/MyLib/DebugLog.h"

//---------------------------------------------------------
// コンストラクタ
//---------------------------------------------------------
PlayerTank::PlayerTank(
	int tankNumber,
	DirectX::SimpleMath::Vector3 position
)
	:
	m_tankNumber{ tankNumber },
	m_position{ position },
	m_angle{},
	m_tank{},
	m_hpGauge{},
	m_camera{}
{
}

//---------------------------------------------------------
// デストラクタ
//---------------------------------------------------------
PlayerTank::~PlayerTank()
{
}

//---------------------------------------------------------
// 初期化処理
//---------------------------------------------------------
void PlayerTank::Initialize()
{
	using namespace DirectX::SimpleMath;

	// 戦車の生成
	Vector3 initialPosition = Vector3::Zero;
	m_tank = std::make_unique<Tank>(m_tankNumber,initialPosition, 0.0f);
	m_tank->Initialize();
	m_tank->SetMaxHp(50);

	// 座標の設定
	m_position = m_tank->GetPosition();

	// HPゲージの作成
	m_hpGauge = std::make_unique<HpGauge>();
	m_hpGauge->Initialize(Vector2{ 200,50 });
}

//---------------------------------------------------------
// 更新処理
//---------------------------------------------------------
void PlayerTank::Update(float elapsedTime)
{
	// 破壊されていたら処理しない
	if (m_tank->GetDead()) { return; }

	// キー入力の処理
	KeyBoardEvent(elapsedTime);
	// 戦車の更新
	m_tank->Update(elapsedTime);

	// 座標の更新
	m_position = m_tank->GetPosition();

	// 座標と回転角の更新
	m_position = m_tank->GetPosition();
	m_angle = m_tank->GetRotation();
}

//---------------------------------------------------------
// 描画処理
//---------------------------------------------------------
void PlayerTank::Render()
{
	// 戦車の描画
	m_tank->Render();

	// HPゲージ
	if (m_tank->GetHp() <= 0) { return; }
	m_hpGauge->Render(m_tank->GetHpRatio());
}

//---------------------------------------------------------
// 終了処理
//---------------------------------------------------------
void PlayerTank::Finalize()
{
}

//---------------------------------------------------------
// キーボードイベント
//---------------------------------------------------------
void PlayerTank::KeyBoardEvent(float elapsedTime)
{
	using namespace DirectX::SimpleMath;
	using namespace DirectX;

	// 破壊されているなら早期リターン
	if (m_tank->GetHp() <= 0) { return; }

	// 移動処理
	Move(elapsedTime);

	// 砲塔と砲身の回転
	RotateTurretCannon();

	// 弾の発射
	const auto& mouseState = InputManager::GetInstance()->GetMouseState();
	if (mouseState.leftButton)
	{
		// 砲弾が発射されたときのみカメラを揺らす
		if (m_tank->GetCannon()->GetCurrentBullet() == TankCannon::BulletType::CANNONBALL &&
			m_tank->GetCannon()->GetCannonBall()->GetBulletState() == IBullet::UNUSED)
			m_camera->StartShakeCamera(25.0f, 0.05f, 0.5f);

		m_tank->GetCannon()->Shoot();
	}

	// 弾の変更
	const auto& keyboardTracker = InputManager::GetInstance()->GetKeyboardTracker();
	if (keyboardTracker->IsKeyPressed(DirectX::Keyboard::Space))
	{
		m_tank->GetCannon()->ChangeBullet();
	}

	// リロード
	const auto& mouseTracker = InputManager::GetInstance()->GetMouseTracker();
	if (mouseTracker->rightButton == mouseTracker->PRESSED)
	{
		m_tank->GetCannon()->StartReload();
	}
}

//---------------------------------------------------------
// 移動処理
//---------------------------------------------------------
void PlayerTank::Move(float elapsedTime)
{
	using namespace DirectX::SimpleMath;
	using namespace DirectX;

	// キーボードステートの取得
	const auto& keyboardState = InputManager::GetInstance()->GetKeyboardState();

	// 変数宣言
	Vector3 velocity = Vector3::Zero;
	float speed = elapsedTime * 3.0f;
	float angle = 0.0f;

	// 左右回転
	if (keyboardState.A)
		angle = DirectX::XMConvertToRadians(0.75f);
	else if (keyboardState.D)
		angle = DirectX::XMConvertToRadians(-0.75f);

	// 前後移動
	if (keyboardState.W)
		velocity = Vector3::Transform(Vector3::Forward * speed, m_tank->GetRotation());
	else if (keyboardState.S)
		velocity = Vector3::Transform(Vector3::Backward * speed, m_tank->GetRotation());

	// 同時押しされていたら停止
	if (keyboardState.A && keyboardState.D)
		angle = 0.0f;
	if (keyboardState.W && keyboardState.S)
		velocity = Vector3::Zero;
		
	// 移動させる
	m_tank->GetBody()->Move(velocity);
	// 回転させる
	m_tank->GetBody()->Rotate(Quaternion::CreateFromYawPitchRoll(angle, 0.0f, 0.0f));
}

//---------------------------------------------------------
// 砲塔と砲身の回転
//---------------------------------------------------------
void PlayerTank::RotateTurretCannon()
{
	// マウスステートの取得
	const auto& mouseState = InputManager::GetInstance()->GetMouseState();

	// 砲塔の回転量
	float rotationY = static_cast<float>(mouseState.x) * 0.001f;
	// 砲塔角度をオイラー角に変換
	float eulerAngle = m_tank->GetTurretRotation().ToEuler().y - rotationY;
	// 砲塔回転の制限
	eulerAngle = mylib::Clamp(eulerAngle, TURRET_ANGLE_MIN, TURRET_ANGLE_MAX);
	// 回転情報を砲塔に伝える
	m_tank->GetTurret()->RotateTurret(eulerAngle);

	// 砲身の回転量
	float rotationX = static_cast<float>(mouseState.y) * 0.001f;
	// 砲身角度をオイラー角に変換(受け取った値を変換)
	eulerAngle = m_tank->GetCannonRotation().ToEuler().x - rotationX;
	// 砲身回転の制限
	eulerAngle = mylib::Clamp(eulerAngle, CANNON_ANGLE_MIN, CANNON_ANGLE_MAX);
	// 回転情報を砲身に伝える
	m_tank->GetCannon()->RotateCannon(eulerAngle);
}

//---------------------------------------------------------
// 座標の受け渡し
//---------------------------------------------------------
void PlayerTank::SetPosition(DirectX::SimpleMath::Vector3 position)
{
	m_tank->GetBody()->SetCollisionVel(position);
}

//---------------------------------------------------------
// 砲身情報の取得
//---------------------------------------------------------
TankCannon* PlayerTank::GetTankCannon() { return m_tank->GetCannon(); }

void PlayerTank::SetOtherTanks(std::vector<Tank*> tanks)
{
	m_tank->SetOtherTanks(tanks);
}

