#include "pch.h"
#include "Game/Objects/Tank/Tank.h"
#include "Game/Objects/Tank/TankBody.h"

// ƒRƒ“ƒXƒgƒ‰ƒNƒ^
Tank::Tank(
	ITankComponent* parent,
	const DirectX::SimpleMath::Vector3& initialPosition,
	const float& initialAngleRL
)
	:
	TankBase(parent, initialPosition, initialAngleRL),
	m_graphics{Graphics::GetInstance()},
	m_currentPosition{},
	m_currentAngleRL{},
	m_tankParts{},
	m_worldMatrix{},
	m_bullets{}
{
}

// ƒfƒXƒgƒ‰ƒNƒ^
Tank::~Tank()
{
	Finalize();
}

// ‰Šú‰»ˆ—
void Tank::Initialize()
{
	using namespace DirectX::SimpleMath;

	// Ô‘Ì‚Ì¶¬
	Attach(std::make_unique<TankBody>(this, Vector3(0.0f, 0.5, 0.0f), 0.0f));

	// –C’e”z—ñ‚ğì¬‚·‚é
	m_bullets.resize(100);
	// ”z—ñ‚É–C’e‚ğŠi”[‚·‚é
	for (int index = 0; index < 100; index++)
	{
		// –C’e‚ğ¶¬‚·‚é
		m_bullets[index] = std::make_unique<Bullet>(IBullet::UNUSED);
		// –C’e‚ğ‰Šú‰»‚·‚é
		m_bullets[index]->Initialize();
	}
}

// XVˆ—
void Tank::Update(
	float elapsedTime,
	const DirectX::SimpleMath::Vector3& currentPosition,
	const float& currentAngleRL
)
{
	using namespace DirectX::SimpleMath;
	UNREFERENCED_PARAMETER(elapsedTime);

	// ƒL[ƒ{[ƒhƒXƒe[ƒg‚Ìæ“¾
	DirectX::Keyboard::State keyboardState = DirectX::Keyboard::Get().GetState();

	// ‘¬“x‚Ì‰Šú‰»
	Vector3 tunkVelocity = Vector3::Zero;

	// ‘OŒãˆÚ“®
	if (keyboardState.W)
	{
		tunkVelocity += Matrix::CreateRotationY(m_currentAngleRL + TankBase::GetInitialAngleRL()).Forward() * 0.1f;
	}
	else if (keyboardState.S)
	{
		tunkVelocity -= Matrix::CreateRotationY(m_currentAngleRL + TankBase::GetInitialAngleRL()).Forward() * 0.1f;
	}

	// ¶‰E‰ñ“]
	if (keyboardState.A)
	{
		m_currentAngleRL += DirectX::XMConvertToRadians(1.0f);
	}
	else if (keyboardState.D)
	{
		m_currentAngleRL -= DirectX::XMConvertToRadians(1.0f);
	}

	m_currentPosition += tunkVelocity;

	// ƒp[ƒc‚ÌXV
	TankBase::Update(elapsedTime, m_currentPosition + GetInitialPosition(), m_currentAngleRL + GetInitialAngleRL());

	// ”ò’e’†‚Ì–C’e‚ğXV‚·‚é
	for (auto& bullet : m_bullets)
	{
		// –C’e‚ª”ò’e‚µ‚Ä‚¢‚éê‡ –C’e‚ğXV‚·‚é
		if (bullet->GetBulletState() == IBullet::FLYING)
		{
			// –C’e‚ğXV‚·‚é
			bullet->Update(elapsedTime);
		}
	}
}

// ©g‚ğ•`‰æ‚µ‚È‚¢•`‰æˆ—(Tank—p)
void Tank::Render()
{
	// ƒp[ƒc‚Ì•`‰æ
	TankBase::Render();

	// ”ò’e’†‚Ì–C’e‚ğ•`‰æ‚·‚é
	for (auto& bullet : m_bullets)
	{
		// ”ò’e’†‚Ì–C’e‚ğ•`‰æ‚·‚é
		if (bullet->GetBulletState() == IBullet::FLYING)
		{
			// –C’e‚ğ•`‰æ‚·‚é
			bullet->Render();
		}
	}
}

// I—¹ˆ—
void Tank::Finalize()
{
	// íœ‚·‚é•”•i‚ğƒŠƒZƒbƒg‚·‚é
	m_tankParts.clear();
}