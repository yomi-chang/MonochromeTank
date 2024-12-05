#include "pch.h"
#include "Game/Objects/Tank/TankBase/Tank.h"

//---------------------------------------------------------
// コンストラクタ
//---------------------------------------------------------
Tank::Tank(
	const int& tankNumber,
	const DirectX::SimpleMath::Vector3& initialPosition,
	const float& initialAngle
)
	:
	m_tankNumber{tankNumber},
	m_graphics{ Graphics::GetInstance() },
	m_initialPosition{initialPosition},
	m_initialAngle{ DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(DirectX::SimpleMath::Vector3::Up, initialAngle) },
	m_currentPosition{},
	m_currentAngle{},
	m_tankParts{},
	m_worldMatrix{},
	m_vertices{},
	m_primitiveBatch{},
	m_basicEffect{},
	m_collider{},
	m_otherTanks{}
{
}

//---------------------------------------------------------
// デストラクタ
//---------------------------------------------------------
Tank::~Tank()
{
}

//---------------------------------------------------------
// 初期化処理
//---------------------------------------------------------
void Tank::Initialize()
{
	using namespace DirectX::SimpleMath;
	using namespace DirectX;

	// 車体の生成(地面から0.5f高い座標に生成)
	Attach(std::make_unique<TankBody>(this,Vector3(0.0f, 0.0f, 0.0f), 0.0f));

	// 現在位置の更新
	m_currentPosition = m_initialPosition;
	m_currentAngle = m_initialAngle;

	// コライダーの生成
	m_collider = std::make_unique<BoxCollider>();
	m_collider->CreateBoundingBox(m_currentPosition, COLLIDER_SIZE);

	// 影用のポリゴンを設定する
	// ここではUV座標を指定している
	m_vertices[0].textureCoordinate = Vector2(0.0f, 0.0f);	// 左上
	m_vertices[1].textureCoordinate = Vector2(1.0f, 0.0f);	// 右上
	m_vertices[2].textureCoordinate = Vector2(0.0f, 1.0f);	// 左下
	m_vertices[3].textureCoordinate = Vector2(1.0f, 1.0f);	// 右下

	// ベーシックエフェクトの設定
	auto device = m_graphics->GetDeviceResources()->GetD3DDevice();
	m_basicEffect = std::make_unique<BasicEffect>(device);
	m_basicEffect->SetTextureEnabled(true);

	// 入力レイアウトを設定する
	DX::ThrowIfFailed(
		CreateInputLayoutFromEffect<VertexPositionTexture>(
			device,
			m_basicEffect.get(),
			m_inputLayout.ReleaseAndGetAddressOf()
		)
	);

	// プリミティブバッチを生成する
	auto context = m_graphics->GetDeviceResources()->GetD3DDeviceContext();
	m_primitiveBatch = std::make_unique<PrimitiveBatch<VertexPositionTexture>>(context);
}

//---------------------------------------------------------
// 更新処理
//---------------------------------------------------------
void Tank::Update(float elapsedTime)
{
	// 部品の更新
	for (auto& part : m_tankParts)
	{
		part->Update(elapsedTime, m_currentPosition, m_currentAngle);
	}

	// コライダー座標の更新
	DirectX::SimpleMath::Vector3 colliderPos = m_body->GetPosition();
	colliderPos.y += COLLIDER_POSITION;
	m_collider->Update(colliderPos);
}

//---------------------------------------------------------
// 描画処理
//---------------------------------------------------------
void Tank::Render()
{
	using namespace DirectX::SimpleMath;

	// 影の描画
	auto context = m_graphics->GetDeviceResources()->GetD3DDeviceContext();
	auto states = m_graphics->GetCommonStates();

	// アルファブレンドの適応
	context->OMSetBlendState(states->AlphaBlend(),nullptr, 0xffffffff);
	// 描画に深度値を適応
	context->OMSetDepthStencilState(states->DepthRead(), 0);
	// 裏面カリングの設定
	context->RSSetState(states->CullCounterClockwise());
	// テクスチャサンプラにリニアクランプを適用
	ID3D11SamplerState* sampler = states->LinearClamp();
	context->PSSetSamplers(0, 1, &sampler);
	// 入力レイアウトの適用
	context->IASetInputLayout(m_inputLayout.Get());
	// ベーシックエフェクトを設定し適応
	//auto basicEffect = m_graphics->GetBasicEffect();
	//m_basicEffect->SetTextureEnabled(true);
	m_basicEffect->SetWorld(Matrix::Identity);
	m_basicEffect->SetView(m_graphics->GetViewMatrix());
	m_basicEffect->SetProjection(m_graphics->GetProjectionMatrix());
	m_basicEffect->SetTexture(Resources::GetInstance()->GetShadowTexture());
	m_basicEffect->Apply(context);

	// 影のパラメータ
	float radius = 1.1f;
	Vector3 position = GetPosition();

	// 影ポリゴンの頂点情報を設定する：影を床から0.01f浮かせる
	m_vertices[0].position = Vector3(-radius, 0.01f, -radius) + position;
	m_vertices[1].position = Vector3(radius,  0.01f, -radius) + position;
	m_vertices[2].position = Vector3(-radius, 0.01f, radius) + position;
	m_vertices[3].position = Vector3(radius,  0.01f, radius) + position;

	// 影ポリゴンを描画する
	m_primitiveBatch->Begin();
	m_primitiveBatch->DrawQuad(m_vertices[0], m_vertices[1], m_vertices[3], m_vertices[2]);
	m_primitiveBatch->End();

	// コライダーの描画
	m_collider->Render();

	// 部品の描画
	for (auto& part : m_tankParts)
	{
		part->Render();
	}
}

//---------------------------------------------------------
// 終了処理
//---------------------------------------------------------
void Tank::Finalize()
{
}

//---------------------------------------------------------
// パーツの追加
//---------------------------------------------------------
void Tank::Attach(std::unique_ptr<IParts> parts)
{
	// パーツの初期化
	parts->Initialize();
	// パーツの追加
	m_tankParts.emplace_back(std::move(parts));
}

//---------------------------------------------------------
// パーツの削除
//---------------------------------------------------------
void Tank::Detach(std::unique_ptr<IParts> parts)
{
}

//---------------------------------------------------------
// 戦車と通常弾の当たり判定
//---------------------------------------------------------
bool Tank::DetectCollisionTankAndNomalBullets()
{
	for (auto& tank : m_otherTanks)
	{
		// 自機の場合は判定を行わない
		if (tank->GetTankNumber() == m_tankNumber) { continue; }

		for (auto& bullet : tank->GetCannon()->GetBullets())
		{
			// 弾丸が飛んでいる、かつ当たっているなら
			if (bullet->GetBulletState() == IBullet::FLYING &&
				m_collider->CheckTriggerCollider(bullet->GetBoundingSphere()))
			{
				bullet->SetBulletState(IBullet::USED);
				return true;
			}
		}
	}
	return false;
}

//---------------------------------------------------------
// 戦車と砲弾の当たり判定
//---------------------------------------------------------
bool Tank::DetectCollisionTankAndCannonBall()
{
	for (auto& tank : m_otherTanks)
	{
		// 自機の場合は判定を行わない
		if (tank->GetTankNumber() == m_tankNumber) { continue; }

		if (tank->GetCannon()->GetCannonBall()->GetBulletState() == IBullet::FLYING &&
			m_collider->CheckTriggerCollider(tank->GetCannon()->GetCannonBall()->GetBoundingSphere()))
		{
			tank->GetCannon()->GetCannonBall()->SetBulletState(IBullet::USED);
			return true;
		}
	}
	return false;
}

//---------------------------------------------------------
// 戦車同士の当たり判定
//---------------------------------------------------------
void Tank::DetectCollisionTankAndOtherTanks()
{
	for (auto& tank : m_otherTanks)
	{
		// 自機の場合は判定を行わない
		if (tank->GetTankNumber() == m_tankNumber) { continue; }

		tank->GetBody()->SetCollisionVel(m_collider->CheckCollisionCollider(tank->GetBoundingBox()));
	}
}