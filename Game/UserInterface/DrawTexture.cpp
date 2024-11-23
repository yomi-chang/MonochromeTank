#include "pch.h"
#include "Game/UserInterface/DrawTexture.h"
#include "Framework/Graphics.h"
#include <PrimitiveBatch.h> 
#include <VertexTypes.h> 
#include <WICTextureLoader.h> 

using namespace DirectX;

//---------------------------------------------------------
// コンストラクタ
//---------------------------------------------------------
DrawTexture::DrawTexture()
	:
	m_graphics{Graphics::GetInstance()},
	m_inputLayout{ m_graphics->GetInputLayout() },
	m_batchEffect{},
	m_primitiveBatch{},
	m_texture{},
	m_states { m_graphics->GetCommonStates()}
{
	using namespace DirectX;

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
		shaderByteCode, byteCodeLength, m_inputLayout.GetAddressOf()
	);

	// プリミティブバッチ作成
	auto context = m_graphics->GetDeviceResources()->GetD3DDeviceContext();
	m_primitiveBatch = std::make_unique<PrimitiveBatch<VertexPositionTexture>>(context);
}

//---------------------------------------------------------
// デストラクタ
//---------------------------------------------------------
DrawTexture::~DrawTexture()
{
}

//---------------------------------------------------------
// 描画処理
//---------------------------------------------------------
void DrawTexture::Render(DirectX::SimpleMath::Vector3 position)
{
	using namespace DirectX::SimpleMath;

	auto context = m_graphics->GetDeviceResources()->GetD3DDeviceContext();
	auto view = m_graphics->GetViewMatrix();
	auto proj = m_graphics->GetProjectionMatrix();

	

	//  頂点情報
	VertexPositionTexture vertex[4] =
	{
		VertexPositionTexture(SimpleMath::Vector3(-0.5f,0.5f, 0.0f),	SimpleMath::Vector2(0.0f, 0.0f)),
		VertexPositionTexture(SimpleMath::Vector3(0.5f, 0.5f, 0.0f),	SimpleMath::Vector2(1.0f, 0.0f)),
		VertexPositionTexture(SimpleMath::Vector3(0.5f, -0.5f, 0.0f),	SimpleMath::Vector2(1.0f,1.0f)),
		VertexPositionTexture(SimpleMath::Vector3(-0.5f,-0.5f, 0.0f),	SimpleMath::Vector2(0.0f, 1.0f)),
	};

	// 座標指定
	Matrix mat = Matrix::CreateTranslation(position);
	//mat = Matrix::CreateScale(5.0f);

	// ビルボード
	Matrix billboard = view.Invert();
	billboard._41 = 0;
	billboard._42 = 0;
	billboard._43 = 0;
	billboard *= mat;

	//	テクスチャサンプラーの設定（クランプテクスチャアドレッシングモード） 
	ID3D11SamplerState* samplers[1] = { m_states->PointWrap() };
	context->PSSetSamplers(0, 1, samplers);

	//	深度バッファに書き込み参照する 
	context->OMSetDepthStencilState(m_states->DepthDefault(), 0);

	//	カリングなし　
	context->RSSetState(m_states->CullNone());

	//	不透明のみ描画する設定 
	m_batchEffect->SetAlphaFunction(D3D11_COMPARISON_NOT_EQUAL);
	m_batchEffect->SetReferenceAlpha(0);

	m_batchEffect->SetWorld(billboard);

	m_batchEffect->SetView(view);
	m_batchEffect->SetProjection(proj);
	m_batchEffect->SetTexture(m_texture);
	m_batchEffect->Apply(context);
	context->IASetInputLayout(m_inputLayout.Get());

	//	半透明部分を描画 
	m_primitiveBatch->Begin();
	m_primitiveBatch->DrawQuad(vertex[0], vertex[1], vertex[2], vertex[3]);
	m_primitiveBatch->End();
}


