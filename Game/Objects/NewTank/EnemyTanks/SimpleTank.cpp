#include"pch.h"
#include"Game/Objects/NewTank/EnemyTanks/SimpleTank.h"

#include "Game/Objects/NewTank/NewTankBase/NewTank.h"
#include "Game/Objects/NewTank/NewTankBase/NewTankBody.h"
#include "Game/Objects/NewTank/NewTankBase/NewTankTurret.h"
#include "Game/Objects/NewTank/NewTankBase/NewTankCannon.h"

#include "Game/Objects/NewTank/PlayerTank.h"

#include "Game/UserInterface/EnemyHpGauge.h"

#include "Framework/InputManager.h"
#include "Libraries/MyLib/Math.h"
#include "Libraries/MyLib/DebugLog.h"

SimpleTank::SimpleTank(
	DirectX::SimpleMath::Vector3 position
)
	:
	m_patrolPoint{},
	m_currentPoint{}
{
	// 座標の受け取り
	m_position = position;
}

SimpleTank::~SimpleTank()
{
}

void SimpleTank::Initialize()
{
	using namespace DirectX::SimpleMath;

	// 戦車の生成
	m_tank = std::make_unique<NewTank>(m_position, DirectX::XMConvertToRadians(180.0f));
	m_tank->Initialize();

	// コライダーの作成
	m_collider = std::make_unique<BoxCollider>();
	m_collider->CreateBoundingBox(m_tank->GetPosition(), Vector3(1.2f, 1.2f, 1.2f));

	// 敵体力ゲージを生成
	m_hpGauge = std::make_unique<EnemyHpGauge>();
	m_hpGauge->SetMaxHp(10.0f);

	m_patrolPoint.emplace_back(Vector3{ 10.0f, 0.0f,  10.0f });
	m_patrolPoint.emplace_back(Vector3{ -10.0f, 0.0f,  10.0f });
	m_patrolPoint.emplace_back(Vector3{ -10.0f, 0.0f, -10.0f });
	m_patrolPoint.emplace_back(Vector3{ 10.0f, 0.0f, -10.0f });

	// デバッグ用モデルの描画
	auto context = Graphics::GetInstance()->GetDeviceResources()->GetD3DDeviceContext();
	m_box = DirectX::GeometricPrimitive::CreateBox(context, DirectX::SimpleMath::Vector3(1.0f, 1.0f, 1.0f));

}

void SimpleTank::Update(float elapsedTime)
{
	using namespace DirectX::SimpleMath;

	// プレイヤーの方向を向く処理 ToDo：今はすぐプレイヤーの方向に向いてしまうので徐々に向けるようにする
	Vector3 delta = m_playerTank->GetPosition() - m_position;
	float angleRadians = atan2(delta.x, delta.z);
	m_tank->GetTurret()->RotateTurret(angleRadians);

	// 戦車の更新
	m_tank->Update(elapsedTime);

	// 座標と回転角の更新
	m_position = m_tank->GetPosition();
	m_angle = m_tank->GetAngle();

	// コライダーの更新
	m_collider->Update(m_position);

	// ダメージの初期化
	m_damage = 0.0f;
	// 衝突判定
	DetectCollisionTankAndBullets();
	DetectCollisionTankAndOtherTanks();
	// ダメージ処理
	m_hpGauge->Damage(m_damage);

	Patrol(elapsedTime);
}

void SimpleTank::Render()
{
	// 戦車の描画
	m_tank->Render();

	// コライダーの描画
	m_collider->Render();

	// HPゲージ
	m_hpGauge->Render(m_position);

	auto view = Graphics::GetInstance()->GetViewMatrix();
	auto proj = Graphics::GetInstance()->GetProjectionMatrix();
	DirectX::SimpleMath::Matrix boxMatrix;
	switch (m_currentPoint)
	{
		case 0:
			boxMatrix = DirectX::SimpleMath::Matrix::CreateTranslation(10.0f, 0.5f, 10.0f);
			break;
		case 1:
			boxMatrix = DirectX::SimpleMath::Matrix::CreateTranslation(-10.0f, 0.5f, 10.0f);
			break;
		case 2:
			boxMatrix = DirectX::SimpleMath::Matrix::CreateTranslation(-10.0f, 0.5f, -10.0f);
			break;
		case 3:
			boxMatrix = DirectX::SimpleMath::Matrix::CreateTranslation(10.0f, 0.5f, -10.0f);
			break;
		default:
			break;
	}
	
	m_box->Draw(boxMatrix, view, proj, DirectX::Colors::Red);
}

void SimpleTank::Finalize()
{
}

void SimpleTank::Attach(std::unique_ptr<IObject> parts)
{
}

void SimpleTank::Detach(std::unique_ptr<IObject> parts)
{
}

// 死亡情報を渡す
bool SimpleTank::GetDead()
{
	// 体力が0なら死亡判定
	if (m_hpGauge->GetHp() <= 0.0f)
	{
		return true;
	}
	return false;
}

// 戦車と弾の衝突判定
void SimpleTank::DetectCollisionTankAndBullets()
{
	// 弾丸と戦車の当たり判定
	// 連射弾
	for (auto& bullet : m_playerTank->GetTankCannon()->GetBullets())
	{
		// 弾丸が飛んでいる、かつ当たっているなら
		if (bullet->GetBulletState() == IBullet::FLYING &&
			m_collider->CheckTriggerCollider(bullet->GetBoundingSphere()))
		{
			bullet->SetBulletState(IBullet::USED);
			m_damage += 0.5f;
		}
	}
	// 砲弾
	if (m_playerTank->GetTankCannon()->GetCannonBall()->GetBulletState() == IBullet::FLYING &&
		m_collider->CheckTriggerCollider(m_playerTank->GetTankCannon()->GetCannonBall()->GetBoundingSphere()))
	{
		m_playerTank->GetTankCannon()->GetCannonBall()->SetBulletState(IBullet::USED);
		m_damage += 3.0f;
	}
}

// 戦車と戦車の衝突判定を行う
void SimpleTank::DetectCollisionTankAndOtherTanks()
{
	// プレイヤーの戦車を押し戻す
	m_playerTank->SetPosition(m_collider->CheckCollisionCollider(m_playerTank->GetBoundingBox()));
}


void SimpleTank::Patrol(float elapsedTime)
{
	using namespace DirectX::SimpleMath;

	// ティーポットの進行方向ベクトル
	Vector3 heading = Vector3::Transform(Vector3::Forward * 0.25, m_tank->GetAngle());

	// ティーポットからターゲットへ向かうベクトル
	//Vector3 toTarget = m_spherePosition - m_teapotPosition;

	// ティーポットからゴールへ向かうベクトル
	Vector3 toGoal = m_patrolPoint[m_currentPoint] - m_position;
	Vector3 toTarget = toGoal;

	//// 視界に入るまでの最短距離
	//float distance = 0.5f + 3.0f;
	//// 球とティーポット間の距離の平方
	//float distSqSphereToTeapot = (m_spherePosition - m_teapotPosition).LengthSquared();

	//// 視界内のフラグをリセットする
	//m_isInside = false;

	//// 視界の範囲内か？
	//if (distSqSphereToTeapot < distance * distance)
	//{
	//	m_isInside = true;
	//}
	//else
	//{
	//	// ターゲットが視界に入っていないときは、ゴールを目指す
	//	toTarget = toGoal;
	//}

	// ティーポットとターゲットとの距離が近すぎなければ
	/*if (toTarget.LengthSquared() > TEAPOT_SPEED * TEAPOT_SPEED)
	{*/
		// ティーポットを移動する
		m_position += (heading * TANK_SPEED * elapsedTime);
		m_tank->GetBody()->SetPosition(m_position);
		//m_boundingSphere.Center = m_teapotPosition;

		/*
			ティーポットがターゲットの方向へ徐々に回転する
		*/
		// 「ティーポットの進行方向ベクトル」と「ターゲットの方向」からcosθを計算する
		float cosTheta = heading.Dot(toTarget) / (toTarget.Length() * heading.Length());

		// acosの引数で指定できる範囲は「-1～1」なので、値を補正する
		cosTheta = std::max(-1.0f, std::min(cosTheta, 1.0f));

		// cosθからθを計算する
		// acosの結果は「0～π」
		float theta = std::acos(cosTheta);

		//１フレームでの回転角を制限値以内に補正する
		theta = std::min(10.0f, theta);

		// 右側に行きたい場合は角度の符号を付け替える
		// ZX平面上にあるベクトルの向きはYのプラスマイナスで判断する
		if (heading.Cross(toTarget).y < 0.0f)
		{
			theta *= (-1.0f);
		}
		/*
			■ティーポットとターゲットの位置関係
			・heading.Cross(toTorus).y > 0.0f：ターゲットはティーポットの左側
			・heading.Cross(toTorus).y < 0.0f：ターゲットはティーポットの右側
			・heading.Cross(toTorus).y == 0.0f：ティーポットとターゲットは同一線上
		*/

		// ティーポットの角度を更新する
		//m_teapotAngle += theta;
		m_tank->GetBody()->Rotate(Quaternion::CreateFromYawPitchRoll(DirectX::XMConvertToRadians(theta), 0.0f, 0.0f));

		// ゴールに達したら、ゴール情報を更新する
		if (/*m_isInside == false &&*/ toTarget.Length() < 1.0f)
		{
			m_currentPoint++;
			m_currentPoint %= 4;
		}
	//}
}
