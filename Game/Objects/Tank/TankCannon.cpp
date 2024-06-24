#include "pch.h"
#include "Game/Objects/Tank/TankCannon.h"
#include "Framework/Resources.h"

// コンストラクタ
TankCannon::TankCannon(
	ITankComponent* parent,
	const DirectX::SimpleMath::Vector3& initialPosition,
	const float& initialAngleRL,
	TankType type
)
	:
	TankBase(parent, initialPosition, initialAngleRL, type),
	m_graphics{Graphics::GetInstance()},
	m_currentPosition{},
	m_currentAngleRL{},
	m_tankParts{},
	m_model{},
	m_worldMatrix{},
	m_currentAngleUD{},
	m_shotBulletNumber{},
	m_shotTimer(SHOT_INTERVAL),
	m_tankType{ type }
{
	// 砲塔へのポインタを取得する
	m_tank = dynamic_cast<Tank*>(parent->GetParent()->GetParent());
}

// デストラクタ
TankCannon::~TankCannon()
{
	Finalize();
}

// 初期化処理
void TankCannon::Initialize()
{
	// モデル情報の受け取り
	m_model = Resources::GetInstance()->GetTankCannonModel();

	// モデルをセットする
	TankBase::SetModel(m_model);
}

// 更新処理
void TankCannon::Update(
	float elapsedTime,
	const DirectX::SimpleMath::Vector3& currentPosition,
	const float& currentAngleRL
)
{
	UNREFERENCED_PARAMETER(elapsedTime);
	using namespace DirectX;

	// 現在の位置を更新する
	m_currentPosition = currentPosition;
	// 現在の回転角を更新する
	m_currentAngleRL = currentAngleRL;

	// キーボードステートの取得
	DirectX::Keyboard::State keyboardState = DirectX::Keyboard::Get().GetState();

	if (m_tankType == TankType::Player)
	{
		// 砲身の上下
		if (keyboardState.Up)
		{
			m_currentAngleUD += DirectX::XMConvertToRadians(0.2f);
		}
		else if (keyboardState.Down)
		{
			m_currentAngleUD -= DirectX::XMConvertToRadians(0.2f);
		}

		// 砲身の向きを制限する
		m_currentAngleUD = TankBase::Clamp(m_currentAngleUD, CANON_ANGLEUD_MIN, CANON_ANGLEUD_MAX);

		// 「砲身」の回転させる
		if (keyboardState.Space)
		{
			// 発射タイマーが0.0より大きい場合は発射タイマーを減らす
			if (m_shotTimer > 0.0f)
			{
				// タイマーを減らす
				m_shotTimer -= elapsedTime;
			}
			else
			{
				// 「砲弾」を発射する
				for (auto& bullet : m_tank->GetBullets())
				{
					// 使用されていない砲弾は発射できる
					if (bullet->GetBulletState() == IBullet::UNUSED)
					{
						// 「砲弾」を発射する
						Shoot(bullet.get());
						// 発射砲弾数をインクリメントする
						m_shotBulletNumber++;
						break;
					}
				}
				// 初期値を設定する
				m_shotTimer = SHOT_INTERVAL;
			}
		}
	}
}

// 自身を描画しない描画処理(Tank用)
void TankCannon::Render()
{
	using namespace DirectX::SimpleMath;

	// ワールド行列を生成する
	m_worldMatrix = Matrix::CreateScale(1.0f);
	m_worldMatrix *= Matrix::CreateRotationX(m_currentAngleUD);
	m_worldMatrix *= Matrix::CreateTranslation(Vector3(0.0f, 0.0f, -0.6f));
	m_worldMatrix *= Matrix::CreateRotationY(m_currentAngleRL + GetInitialAngleRL());
	m_worldMatrix *= Matrix::CreateTranslation(m_currentPosition + GetInitialPosition());
	
	// 描画を行う
	TankBase::Render(m_worldMatrix);
}

// 終了処理
void TankCannon::Finalize()
{
}

void TankCannon::Shoot(IBullet* bullet)
{
	// 「砲弾」位置を設定する
	bullet->SetPosition(m_currentPosition);
	// 「砲弾」初期左右角を設定する
	bullet->SetAngleRL(m_currentAngleRL);
	// 「砲弾」初期上下角を設定する
	bullet->SetAngleUD(m_currentAngleUD);
	// 「砲弾」を発射する
	bullet->SetBulletState(IBullet::FLYING);
}
