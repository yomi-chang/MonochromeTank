#include "pch.h"
#include "Game/UserInterface/EnemyHpGauge.h"
#include <PrimitiveBatch.h> 
#include <VertexTypes.h> 
#include <WICTextureLoader.h> 

using namespace DirectX;

EnemyHpGauge::EnemyHpGauge()
	:
	m_alpha{},
	m_elapsedTime{},
	m_maxHp{},
	m_hp{},
	m_isDead{},
	m_graphics{Graphics::GetInstance()}
{
	auto device = m_graphics->GetDeviceResources()->GetD3DDevice();

	//	エフェクトの作成 
	m_basicEffect = std::make_unique<BasicEffect>(device);
	m_basicEffect->SetVertexColorEnabled(true);

	// 入力レイアウトを作成する
	DX::ThrowIfFailed(
		CreateInputLayoutFromEffect<VertexPositionColor>(
			device,
			m_basicEffect.get(),
			m_inputLayout.ReleaseAndGetAddressOf()
		)
	);

	// 初期不透明度を設定
	m_alpha = 1.0f;
}

EnemyHpGauge::~EnemyHpGauge()
{
}

void EnemyHpGauge::Render(
	DirectX::SimpleMath::Vector3 position,
	float hpRatio
)
{
	// 死亡判定
	if (m_hp <= 0.0f)
	{
		m_isDead = true;
	}

	auto context = m_graphics->GetDeviceResources()->GetD3DDeviceContext();
	auto view = m_graphics->GetViewMatrix();
	auto proj = m_graphics->GetProjectionMatrix();
	auto states = m_graphics->GetCommonStates();

	//	プリミティブバッチの作成
	m_primitiveBatch = std::make_unique<PrimitiveBatch<VertexPositionColor>>(context);

	// 最大体力の半分（座標指定で使用)
	float halfMaxHp = m_maxHp / 2.0f;

	VertexPositionColor backVertex[4] =
	{
		VertexPositionColor(SimpleMath::Vector3(-halfMaxHp / 5.0f,1.4f, 0.0f),Colors::Black),
		VertexPositionColor(SimpleMath::Vector3(halfMaxHp / 5.0f,1.4f, 0.0f),Colors::Black),
		VertexPositionColor(SimpleMath::Vector3(-halfMaxHp / 5.0f, 1.2f, 0.0f),Colors::Black),
		VertexPositionColor(SimpleMath::Vector3(halfMaxHp / 5.0f, 1.2f, 0.0f),Colors::Black)
	};

	// ダメージの計算
	float damage = m_maxHp - m_hp;

	/*VertexPositionColor frontVertex[4] =
	{
		VertexPositionColor(SimpleMath::Vector3(-halfMaxHp / 5.0f,1.5f, 0.0f),Colors::LightGreen),
		VertexPositionColor(SimpleMath::Vector3(halfMaxHp / 5.0f - damage / 5.0f,1.5f, 0.0f),Colors::LightGreen),
		VertexPositionColor(SimpleMath::Vector3(-halfMaxHp / 5.0f, 1.2f, 0.0f),Colors::LightGreen),
		VertexPositionColor(SimpleMath::Vector3(halfMaxHp / 5.0f - damage / 5.0f, 1.2f, 0.0f),Colors::LightGreen)
	};*/

	float width = 2.0f;
	float left = -width / 2.0f;
	float right = left + width;
	float f = right - (width * hpRatio);


	VertexPositionColor frontVertex[4] =
	{
		VertexPositionColor(SimpleMath::Vector3(left,1.4f, 0.0f),Colors::LightGreen),
		VertexPositionColor(SimpleMath::Vector3(f,1.4f, 0.0f),Colors::LightGreen),
		VertexPositionColor(SimpleMath::Vector3(left, 1.2f, 0.0f),Colors::LightGreen),
		VertexPositionColor(SimpleMath::Vector3(f, 1.2f, 0.0f),Colors::LightGreen)
	};

	// 座標の指定
	DirectX::SimpleMath::Matrix mat = DirectX::SimpleMath::Matrix::CreateTranslation(position);

	//ビルボード
	DirectX::SimpleMath::Matrix billboard = view.Invert();
	billboard._41 = 0;
	billboard._42 = 0;
	billboard._43 = 0;
	billboard *= mat;

	//	テクスチャサンプラーの設定（クランプテクスチャアドレッシングモード） 
	ID3D11SamplerState* samplers[1] = { states->PointWrap() };
	context->PSSetSamplers(0, 1, samplers);

	//	深度バッファに書き込み参照する 
	context->OMSetDepthStencilState(states->DepthDefault(), 0);

	//	カリングなし　
	context->RSSetState(states->CullNone());

	//	不透明のみ描画する設定 
	m_basicEffect->SetAlpha(m_alpha);

	context->OMSetBlendState(states->AlphaBlend(), nullptr, 0xFFFFFFFF);
	m_basicEffect->SetWorld(billboard);
	m_basicEffect->SetView(view);
	m_basicEffect->SetProjection(proj);
	m_basicEffect->Apply(context);
	context->IASetInputLayout(m_inputLayout.Get());

	//	半透明部分を描画 
	m_primitiveBatch->Begin();
	m_primitiveBatch->DrawQuad(backVertex[0], backVertex[1], backVertex[3], backVertex[2]);
	m_primitiveBatch->DrawQuad(frontVertex[0], frontVertex[1], frontVertex[3], frontVertex[2]);
	m_primitiveBatch->End();
}
