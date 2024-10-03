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

SimpleTank::SimpleTank()
	:
	m_isDead{}
{
}

SimpleTank::~SimpleTank()
{
}

void SimpleTank::Initialize()
{
	using namespace DirectX::SimpleMath;

	// 戦車の生成
	Vector3 initialPosition = Vector3{ 0.0f, 0.0f, -10.0f };
	m_tank = std::make_unique<NewTank>(initialPosition, DirectX::XMConvertToRadians(180.0f));
	m_tank->Initialize();

	// コライダーの作成
	m_collider = std::make_unique<BoxCollider>();
	m_collider->CreateBoundingBox(m_tank->GetBody()->GetPosition(), Vector3(1.2f, 1.2f, 1.2f));

	// 敵体力ゲージを生成
	m_hpGauge = std::make_unique<EnemyHpGauge>();
	m_hpGauge->SetMaxHp(10.0f);
}

void SimpleTank::Update(float elapsedTime)
{
	// 戦車の更新
	m_tank->Update(elapsedTime);

	// 座標と回転角の更新
	m_position = m_tank->GetBody()->GetPosition();
	m_angle = m_tank->GetBody()->GetAngle();

	// コライダーの更新
	m_collider->Update(m_position);

	// ダメージの初期化
	m_damage = 0.0f;
	// 衝突判定
	DetectCollisionTankAndBullets();
	DetectCollisionTankAndOtherTanks();
	// ダメージ処理
	m_hpGauge->Damage(m_damage);
}

void SimpleTank::Render()
{
	// 戦車の描画
	m_tank->Render();

	// コライダーの描画
	m_collider->Render();

	// HPゲージ
	m_hpGauge->Render(m_position);
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
