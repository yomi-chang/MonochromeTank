#include "pch.h"
#include "Game/Objects/NewTank/NewTankBase/NewTank.h"
#include "Game/Objects/NewTank/NewTankBase/NewTankBody.h"

//---------------------------------------------------------
// コンストラクタ
//---------------------------------------------------------
NewTank::NewTank(
	const DirectX::SimpleMath::Vector3& initialPosition,
	const float& initialAngle
)
	:
	m_graphics{ Graphics::GetInstance() },
	m_initialPosition{initialPosition},
	m_initialAngle{ DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(DirectX::SimpleMath::Vector3::Up, initialAngle) },
	m_currentPosition{},
	m_currentAngle{},
	m_tankParts{},
	m_worldMatrix{},
	m_vertices{},
	m_primitiveBatch{},
	m_basicEffect{}
{
}

//---------------------------------------------------------
// デストラクタ
//---------------------------------------------------------
NewTank::~NewTank()
{
}

//---------------------------------------------------------
// 初期化処理
//---------------------------------------------------------
void NewTank::Initialize()
{
	using namespace DirectX::SimpleMath;
	using namespace DirectX;

	// 車体の生成(地面から0.5f高い座標に生成)
	Attach(std::make_unique<NewTankBody>(this,Vector3(0.0f, 0.5f, 0.0f), 0.0f));

	// 現在位置の更新
	m_currentPosition = m_initialPosition;
	m_currentAngle = m_initialAngle;

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
void NewTank::Update(float elapsedTime)
{
	// 部品の更新
	for (auto& part : m_tankParts)
	{
		part->Update(elapsedTime, m_currentPosition, m_currentAngle);
	}
}

//---------------------------------------------------------
// 描画処理
//---------------------------------------------------------
void NewTank::Render()
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
	float radius = 1.0f;
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

	// 部品の描画
	for (auto& part : m_tankParts)
	{
		part->Render();
	}
}

//---------------------------------------------------------
// 終了処理
//---------------------------------------------------------
void NewTank::Finalize()
{
}

//---------------------------------------------------------
// パーツの追加
//---------------------------------------------------------
void NewTank::Attach(std::unique_ptr<IObject> part)
{
	// パーツの初期化
	part->Initialize();
	// パーツの追加
	m_tankParts.emplace_back(std::move(part));
}

//---------------------------------------------------------
// パーツの削除
//---------------------------------------------------------
void NewTank::Detach(std::unique_ptr<IObject> part)
{
}
