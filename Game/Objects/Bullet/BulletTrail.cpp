/*
	@file	BulletTrail.cpp
	@brief	弾の軌跡を表示するクラス
*/
#include "pch.h"
#include "Game/Objects/Bullet/BulletTrail.h"
#include "Framework/Graphics.h"
#include "Framework/Resources.h"

BulletTrail::BulletTrail()
	:
	m_posArray{},
	m_primitiveBatch{},
	m_maxTrail{},
	m_graphics{ Graphics::GetInstance()}
{
}

void BulletTrail::Initialize(int trailCount)
{
	using namespace DirectX;
	using namespace DirectX::SimpleMath;

	// 表示するトレイルの数の設定
	m_maxTrail = trailCount;

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
}

void BulletTrail::Render()
{
	using namespace DirectX;
	using namespace DirectX::SimpleMath;

	// 座標情報が2つ未満なら早期リターン
	if (m_posArray.size() <= 1) { return; }

	auto context = m_graphics->GetDeviceResources()->GetD3DDeviceContext();
	auto states = m_graphics->GetCommonStates();
	// 座標の指定
	Matrix world = Matrix::Identity;
	// アルファブレンドの適応
	context->OMSetBlendState(states->AlphaBlend(), nullptr, 0xffffffff);
	// 描画に深度値を適応
	context->OMSetDepthStencilState(states->DepthRead(), 0);
	// 裏面カリングの設定
	context->RSSetState(states->CullNone());
	// テクスチャサンプラにリニアクランプを適用
	ID3D11SamplerState* sampler = states->LinearClamp();
	context->PSSetSamplers(0, 1, &sampler);
	// 入力レイアウトの適用
	context->IASetInputLayout(m_inputLayout.Get());
	// ベーシックエフェクトを設定し適応
	m_basicEffect->SetWorld(world);
	m_basicEffect->SetView(m_graphics->GetViewMatrix());
	m_basicEffect->SetProjection(m_graphics->GetProjectionMatrix());
	m_basicEffect->SetTexture(Resources::GetInstance()->GetBoxTexture());
	m_basicEffect->Apply(context);

	// bufferSize分回す
	for (int i = 1; i < m_posArray.size(); i++)
	{
		VertexPositionTexture vertex[4] =
		{
			VertexPositionTexture(m_posArray[i].top,		Vector2(0.0f, 0.0f)),
			VertexPositionTexture(m_posArray[i].bottom,		Vector2(1.0f, 0.0f)),
			VertexPositionTexture(m_posArray[i - 1].top,	Vector2(0.0f, 1.0f)),
			VertexPositionTexture(m_posArray[i - 1].bottom,	Vector2(1.0f, 1.0f))
		};
		m_primitiveBatch->Begin();
		m_primitiveBatch->DrawQuad(vertex[0], vertex[1], vertex[3], vertex[2]);
		m_primitiveBatch->End();
	}
}

// 座標の受け取り
void BulletTrail::SetPosition(DirectX::SimpleMath::Vector3 top, DirectX::SimpleMath::Vector3 bottom)
{
	// 古い座標を削除
	if (m_posArray.size() > m_maxTrail)
	{
		m_posArray.erase(m_posArray.begin(), m_posArray.begin() + (m_posArray.size() - m_maxTrail));
	}

	// 座標情報の受け取り
	m_posArray.push_back(PosBuffer{ top, bottom });
}
