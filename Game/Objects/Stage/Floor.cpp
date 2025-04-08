/*
	@file	Floor.cpp
	@brief	床クラス
*/
#include "pch.h"
#include "Game/Objects/Stage/Floor.h"
#include "Framework/Graphics.h"

#include <PrimitiveBatch.h> 
#include <VertexTypes.h> 
#include <WICTextureLoader.h> 

//-------------------------------------------------------------------
// コンストラクタ
//-------------------------------------------------------------------
Floor::Floor(int size)
	:
	m_graphics{ Graphics::GetInstance()},
	m_inputLayout{ m_graphics->GetInputLayout() },
	m_batchEffect{},
	m_primitiveBatch{},
	m_texture{},
	m_vertex{}
{
	using namespace DirectX;
	using namespace DirectX::SimpleMath;

	// エフェクト作成
	auto device = m_graphics->GetDeviceResources()->GetD3DDevice();
	m_batchEffect = std::make_unique<AlphaTestEffect>(device);
	m_batchEffect->SetAlphaFunction(D3D11_COMPARISON_EQUAL);
	m_batchEffect->SetReferenceAlpha(255);

	//	入力レイアウト生成 
	void const* shaderByteCode;
	size_t byteCodeLength;
	m_batchEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);
	device->CreateInputLayout(
		VertexPositionTexture::InputElements,
		VertexPositionTexture::InputElementCount,
		shaderByteCode, byteCodeLength, m_inputLayout.ReleaseAndGetAddressOf()
	);

	// プリミティブバッチ作成
	auto context = m_graphics->GetDeviceResources()->GetD3DDeviceContext();
	m_primitiveBatch = std::make_unique<PrimitiveBatch<VertexPositionTexture>>(context);

	// 頂点情報の設定
	float halfSize = static_cast<float>(size) / 2 - 0.5f;
	m_vertex[0] = DirectX::VertexPositionTexture(Vector3(-halfSize, 0.0f, halfSize), Vector2(0.0f, 0.0f));
	m_vertex[1] = DirectX::VertexPositionTexture(Vector3(halfSize, 0.0f, halfSize), Vector2(halfSize, 0.0f));
	m_vertex[2] = DirectX::VertexPositionTexture(Vector3(halfSize, 0.0f, -halfSize), Vector2(halfSize, halfSize));
	m_vertex[3] = DirectX::VertexPositionTexture(Vector3(-halfSize, 0.0f, -halfSize), Vector2(0.0f, halfSize));
}

//-------------------------------------------------------------------
// デストラクタ
//-------------------------------------------------------------------
Floor::~Floor()
{
}

//-------------------------------------------------------------------
// 描画処理
//-------------------------------------------------------------------
void Floor::Render()
{
	using namespace DirectX::SimpleMath;

	auto context = m_graphics->GetDeviceResources()->GetD3DDeviceContext();
	Matrix view = m_graphics->GetViewMatrix();
	Matrix proj = m_graphics->GetProjectionMatrix();
	auto states = m_graphics->GetCommonStates();

	//	テクスチャサンプラーの設定（クランプテクスチャアドレッシングモード） 
	ID3D11SamplerState* samplers[1] = { states->PointWrap() };
	context->PSSetSamplers(0, 1, samplers);

	//	深度バッファに書き込み参照する 
	context->OMSetDepthStencilState(states->DepthDefault(), 0);

	// カリング
	context->RSSetState(states->CullClockwise());
	context->RSSetState(states->CullNone());

	Matrix world = Matrix::CreateTranslation(Vector3::Zero);
	m_batchEffect->SetWorld(world);

	m_batchEffect->SetView(view);
	m_batchEffect->SetProjection(proj);
	m_batchEffect->SetTexture(m_texture);
	m_batchEffect->Apply(context);
	context->IASetInputLayout(m_inputLayout.Get());

	//	半透明部分を描画 
	m_primitiveBatch->Begin();
	m_primitiveBatch->DrawQuad(m_vertex[0], m_vertex[1], m_vertex[2], m_vertex[3]);
	m_primitiveBatch->End();
}
