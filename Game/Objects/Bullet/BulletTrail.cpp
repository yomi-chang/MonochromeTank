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
	m_bufferSize{},
	m_posArray{},
	m_primitiveBatch{},
	m_graphics{ Graphics::GetInstance()}
{
}

void BulletTrail::Initialize(int bufferSize)
{
	m_bufferSize = bufferSize;

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
}

void BulletTrail::Update()
{
	////データを更新
	//for (size_t i = posArray.size() - 1; i > 0; --i)
	//{
	//	posArray[i] = posArray[i - 1];
	//}
	//posArray.front() = tempPos;
	//tempPos = PosBuffer();
}

void BulletTrail::Render()
{
	using namespace DirectX;
	using namespace DirectX::SimpleMath;

	// 座標情報が2つ未満なら早期リターン
	if (m_posArray.size() < 2) { return; }

	//頂点データを更新する
	/*float amount = 1.0f / (usedPosArray.size() - 1);
	float v = 0;
	vertex.clear();
	vertex.resize(usedPosArray.size() * 2);*/
	auto context = m_graphics->GetDeviceResources()->GetD3DDeviceContext();
	auto states = m_graphics->GetCommonStates();
	// 座標の指定
	Matrix world = Matrix::Identity;
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
	m_basicEffect->SetWorld(world);
	m_basicEffect->SetView(m_graphics->GetViewMatrix());
	m_basicEffect->SetProjection(m_graphics->GetProjectionMatrix());
	m_basicEffect->SetTexture(Resources::GetInstance()->GetBoxTexture());
	m_basicEffect->Apply(context);

	// bufferSize分回す
	for (int i = 1; i < m_bufferSize; i++)
	{
		VertexPositionTexture vertex[4] =
		{
			VertexPositionTexture(m_posArray[i].head,		Vector2(0.0f, 0.0f)),
			VertexPositionTexture(m_posArray[i].tail,		Vector2(1.0f, 0.0f)),
			VertexPositionTexture(m_posArray[i - 1].head,	Vector2(0.0f, 1.0f)),
			VertexPositionTexture(m_posArray[i - 1].tail,	Vector2(1.0f, 1.0f))
		};
		//v += amount;
		m_primitiveBatch->Begin();
		m_primitiveBatch->DrawQuad(vertex[0], vertex[1], vertex[3], vertex[2]);
		m_primitiveBatch->End();
	}

	//　配列情報をクリアする
	//m_posArray.clear();
}

// 座標の受け取り
void BulletTrail::SetPosition(DirectX::SimpleMath::Vector3& head, DirectX::SimpleMath::Vector3& tail)
{
	// 座標情報の受け取り
	m_posArray.push_back(PosBuffer{ head, tail });
}
