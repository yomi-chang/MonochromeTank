/**
 * @file   HitEffect.cpp
 * @brief  弾のヒットエフェクトクラス
 */
#include "pch.h"
#include "Game/Particle/HitEffect.h"
#include "Framework/Graphics.h"
#include "Framework/Resources.h"

#include <PrimitiveBatch.h> 
#include <VertexTypes.h> 
#include <WICTextureLoader.h> 

/// <summary>
/// コンストラクタ
/// </summary>
HitEffect::HitEffect()
	:
	m_hitTexture{},
	m_alpha{},
	m_graphics{ Graphics::GetInstance() },
	m_isPlay{}
{
}

/// <summary>
/// デストラクタ
/// </summary>
HitEffect::~HitEffect()
{
}

/// <summary>
/// 初期化処理
/// </summary>
void HitEffect::Initialize()
{
	using namespace DirectX;
	using namespace DirectX::SimpleMath;

	// エフェクト作成
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

	// プリミティブバッチ作成
	auto context = m_graphics->GetDeviceResources()->GetD3DDeviceContext();
	m_primitiveBatch = std::make_unique<PrimitiveBatch<VertexPositionTexture>>(context);

	// 画像の受け取り
	m_hitTexture = Resources::GetInstance()->GetBlackSmokeTexture();

	// 不透明度の初期値の設定
	m_alpha = 1.0f;
}

/// <summary>
/// 更新処理
/// </summary>
/// <param name="elapsedTime">フレーム間の経過時間</param>
void HitEffect::Update(float elapsedTime)
{
	// 画像の不透明度を下げていく
	m_alpha -= elapsedTime * 0.5f;

	// 完全に透明になった時に再生終了
	if (m_alpha <= 0.0f)
		m_isPlay = true;
}

/// <summary>
/// 描画処理
/// </summary>
/// <param name="position">座標</param>
void HitEffect::Render(const DirectX::SimpleMath::Vector3& position)
{
	using namespace DirectX;
	using namespace DirectX::SimpleMath;

	auto context = m_graphics->GetDeviceResources()->GetD3DDeviceContext();
	auto view = m_graphics->GetViewMatrix();
	auto states = m_graphics->GetCommonStates();

	// 頂点情報の設定
	VertexPositionTexture vertex[4] =
	{
		VertexPositionTexture(Vector3(-2,2,0),	Vector2(0.0f, 0.0f)),
		VertexPositionTexture(Vector3(2,2,0),	Vector2(1.0f, 0.0f)),
		VertexPositionTexture(Vector3(-2,-2,0),	Vector2(0.0f, 1.0f)),
		VertexPositionTexture(Vector3(2,-2,0),	Vector2(1.0f, 1.0f))
	};

	// 座標の指定
	Matrix world = Matrix::CreateTranslation(position);

	// ビルボード
	Matrix billboard = view.Invert();
	billboard._41 = 0;
	billboard._42 = 0;
	billboard._43 = 0;
	billboard *= world;

	// アルファブレンドの適応
	context->OMSetBlendState(states->AlphaBlend(), nullptr, 0xffffffff);
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
	m_basicEffect->SetAlpha(m_alpha);
	m_basicEffect->SetWorld(billboard);
	m_basicEffect->SetView(m_graphics->GetViewMatrix());
	m_basicEffect->SetProjection(m_graphics->GetProjectionMatrix());
	m_basicEffect->SetTexture(Resources::GetInstance()->GetBlackSmokeTexture());
	m_basicEffect->Apply(context);

	//	半透明部分を描画 
	m_primitiveBatch->Begin();
	m_primitiveBatch->DrawQuad(vertex[0], vertex[1], vertex[3], vertex[2]);
	m_primitiveBatch->End();
}
