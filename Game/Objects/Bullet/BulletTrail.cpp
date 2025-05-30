/**
 * @file   BulletTrail.cpp
 * @brief  弾の軌跡を表示するクラス
 */
#include "pch.h"
#include "Game/Objects/Bullet/BulletTrail.h"
#include "Framework/Graphics.h"
#include "Framework/Resources.h"

/// <summary>
/// コンストラクタ
/// </summary>
BulletTrail::BulletTrail()
	:
	m_posArray{},
	m_inputLayout{},
	m_basicEffect{},
	m_primitiveBatch{},
	m_maxTrail{},
	m_graphics{ Graphics::GetInstance()}
{
}

/// <summary>
/// デストラクタ
/// </summary>
BulletTrail::~BulletTrail()
{
}

/// <summary>
/// 初期化処理
/// </summary>
/// <param name="trailCount">トレイルの長さ</param>
void BulletTrail::Initialize(int trailCount)
{
	using namespace DirectX;
	using namespace DirectX::SimpleMath;

	// 表示する長さの設定
	m_maxTrail = trailCount;

	// エフェクト作成
	auto device = m_graphics->GetDeviceResources()->GetD3DDevice();
	m_basicEffect = std::make_unique<BasicEffect>(device);
	m_basicEffect->SetTextureEnabled(true);

	// 入力レイアウト生成
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

/// <summary>
/// 描画処理
/// </summary>
void BulletTrail::Render()
{
	using namespace DirectX;
	using namespace DirectX::SimpleMath;

	// サイズが一定以上出ないなら早期リターン
	if (m_posArray.size() <= 1) { return; }

	// 必要な情報の受け取り
	auto context = m_graphics->GetDeviceResources()->GetD3DDeviceContext();
	auto states = m_graphics->GetCommonStates();
	
	Matrix world = Matrix::Identity;

	// アルファブレンドの適応
	context->OMSetBlendState(states->AlphaBlend(), nullptr, 0xffffffff);
	// 深度バッファ設定
	context->OMSetDepthStencilState(states->DepthRead(), 0);
	// カリングの設定(カリングなし)
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
	m_basicEffect->SetTexture(Resources::GetInstance()->GetTrailSmokeTexture());
	m_basicEffect->Apply(context);

	// トレイルの描画
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

/// <summary>
/// 座標の設定
/// </summary>
/// <param name="right">板ポリの右端</param>
/// <param name="left">板ポリの左端</param>
void BulletTrail::SetPosition(
	const DirectX::SimpleMath::Vector3& right,
	const DirectX::SimpleMath::Vector3& left
)
{
	// maxTrailを超過した分を削除
	if (m_posArray.size() > m_maxTrail)
	{
		m_posArray.erase(m_posArray.begin(), m_posArray.begin() + (m_posArray.size() - m_maxTrail));
	}

	// 座標の設定
	m_posArray.push_back(PosBuffer{ right, left });
}
