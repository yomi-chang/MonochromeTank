/*
	@file	EnemyHpGauge.cpp
	@brief	敵体力クラス
*/
#include "pch.h"
#include "Game/UserInterface/EnemyHpGauge.h"
#include <PrimitiveBatch.h> 
#include <VertexTypes.h> 
#include <WICTextureLoader.h> 

using namespace DirectX;

//-------------------------------------------------------------------
// コンストラクタ
//-------------------------------------------------------------------
EnemyHpGauge::EnemyHpGauge()
	:
	m_alpha{},
	m_elapsedTime{},
	m_graphics{Graphics::GetInstance()}
{
	auto context = m_graphics->GetDeviceResources()->GetD3DDeviceContext();
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

	//	プリミティブバッチの作成
	m_primitiveBatch = std::make_unique<PrimitiveBatch<VertexPositionColor>>(context);
}

//-------------------------------------------------------------------
// デストラクタ
//-------------------------------------------------------------------
EnemyHpGauge::~EnemyHpGauge()
{
}

//-------------------------------------------------------------------
// 描画処理
//-------------------------------------------------------------------
void EnemyHpGauge::Render(
	const DirectX::SimpleMath::Vector3& position,
	float hpRatio
)
{	
	auto view = m_graphics->GetViewMatrix();
	auto proj = m_graphics->GetProjectionMatrix();
	auto states = m_graphics->GetCommonStates();
	auto context = m_graphics->GetDeviceResources()->GetD3DDeviceContext();

	float width = 1.0f;
	float left = -width / 2.0f;
	float right = left + width;
	float f = right - (width * hpRatio);

	VertexPositionColor backVertex[4] =
	{
		VertexPositionColor(SimpleMath::Vector3(left, 1.1f, 0.0f),Colors::Black),
		VertexPositionColor(SimpleMath::Vector3(right,1.1f, 0.0f),Colors::Black),
		VertexPositionColor(SimpleMath::Vector3(left, 1.0f, 0.0f),Colors::Black),
		VertexPositionColor(SimpleMath::Vector3(right,1.0f, 0.0f),Colors::Black)
	};

	VertexPositionColor frontVertex[4] =
	{
		VertexPositionColor(SimpleMath::Vector3(left, 1.1f, 0.0f),Colors::LightGreen),
		VertexPositionColor(SimpleMath::Vector3(f	, 1.1f, 0.0f),Colors::LightGreen),
		VertexPositionColor(SimpleMath::Vector3(left, 1.0f, 0.0f),Colors::LightGreen),
		VertexPositionColor(SimpleMath::Vector3(f	, 1.0f, 0.0f),Colors::LightGreen)
	};

	// 座標の指定
	DirectX::SimpleMath::Matrix mat = DirectX::SimpleMath::Matrix::CreateTranslation(position);

	//ビルボード
	DirectX::SimpleMath::Matrix billboard = view.Invert();
	billboard._41 = 0;
	billboard._42 = 0;
	billboard._43 = 0;
	billboard *= mat;

	// テクスチャサンプラーの設定（クランプテクスチャアドレッシングモード） 
	ID3D11SamplerState* samplers[1] = { states->PointWrap() };
	context->PSSetSamplers(0, 1, samplers);

	// 深度バッファ設定(一番手前に表示)
	context->OMSetDepthStencilState(states->DepthNone(), 0);

	// カリングなし　
	context->RSSetState(states->CullNone());

	// 不透明のみ描画する設定 
	m_basicEffect->SetAlpha(m_alpha);

	context->OMSetBlendState(states->AlphaBlend(), nullptr, 0xFFFFFFFF);
	m_basicEffect->SetWorld(billboard);
	m_basicEffect->SetView(view);
	m_basicEffect->SetProjection(proj);
	m_basicEffect->Apply(context);
	context->IASetInputLayout(m_inputLayout.Get());

	// 半透明部分を描画 
	m_primitiveBatch->Begin();
	m_primitiveBatch->DrawQuad(backVertex[0], backVertex[1], backVertex[3], backVertex[2]);
	m_primitiveBatch->DrawQuad(frontVertex[0], frontVertex[1], frontVertex[3], frontVertex[2]);
	m_primitiveBatch->End();
}
