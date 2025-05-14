/*
	@file	DamageEffect.cpp
	@brief	ダメージエフェクトクラス
*/
#include "pch.h"
#include "DamageEffect.h"
#include "Framework/Resources.h"
#include "Libraries/Microsoft/ReadData.h"

const std::vector<D3D11_INPUT_ELEMENT_DESC> DamageEffect::INPUT_LAYOUT =
{
	{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

//-------------------------------------------------------------------
// コンストラクタ
//-------------------------------------------------------------------
DamageEffect::DamageEffect()
	:
	m_graphics{ Graphics::GetInstance() },
	m_cBuffer{},
	m_inputLayout{},
	m_primitiveBatch{},
	m_texture{},
	m_ps{},
	m_vs{},
	m_gs{},
	m_time{},
	m_elapsedTime{}
{
}

//-------------------------------------------------------------------
// デストラクタ
//-------------------------------------------------------------------
DamageEffect::~DamageEffect()
{
}

//-------------------------------------------------------------------
// 生成処理
//-------------------------------------------------------------------
void DamageEffect::Create()
{
	// シェーダのロード
	this->LoadShader();

	// 定数バッファの作成
	this->CreateCBuffer();

	// テクスチャの読み込み
	m_texture = Resources::GetInstance()->GetDamageEffectTexture();

	auto context = m_graphics->GetDeviceResources()->GetD3DDeviceContext();
	m_primitiveBatch = std::make_unique<DirectX::PrimitiveBatch<DirectX::VertexPositionColorTexture>>(context);
}

//-------------------------------------------------------------------
// 描画処理
//-------------------------------------------------------------------
void DamageEffect::Render()
{
	auto context = m_graphics->GetDeviceResources()->GetD3DDeviceContext();
	auto states = m_graphics->GetCommonStates();

	// 時間
	m_time += m_elapsedTime;

	//	頂点情報(板ポリゴンの４頂点の座標情報）
	DirectX::VertexPositionColorTexture vertex[4] =
	{
		DirectX::VertexPositionColorTexture(DirectX::SimpleMath::Vector3(0.0f,  0.0f, 0.0f), DirectX::SimpleMath::Vector4::One, DirectX::SimpleMath::Vector2(0.0f, 0.0f)),
	};

	//	シェーダーに渡す追加のバッファを作成する。(ConstBuffer）
	CBuffer cbuff;
	cbuff.matView = DirectX::SimpleMath::Matrix::Identity;
	cbuff.matProj = DirectX::SimpleMath::Matrix::Identity;
	cbuff.matWorld = DirectX::SimpleMath::Matrix::Identity;
	cbuff.Diffuse = DirectX::SimpleMath::Vector4(1, 1, 1, 1);
	cbuff.time = DirectX::SimpleMath::Vector4(m_elapsedTime, 0, 0, 0);

	//	受け渡し用バッファの内容更新(ConstBufferからID3D11Bufferへの変換）
	context->UpdateSubresource(m_cBuffer.Get(), 0, NULL, &cbuff, 0, 0);

	//	シェーダーにバッファを渡す
	ID3D11Buffer* cb[1] = { m_cBuffer.Get() };
	context->VSSetConstantBuffers(0, 1, cb);
	context->GSSetConstantBuffers(0, 1, cb);
	context->PSSetConstantBuffers(0, 1, cb);

	//	画像用サンプラーの登録
	ID3D11SamplerState* sampler[1] = { states->LinearWrap() };
	context->PSSetSamplers(0, 1, sampler);

	//	半透明描画指定
	ID3D11BlendState* blendstate = states->NonPremultiplied();

	//	透明判定処理
	context->OMSetBlendState(blendstate, nullptr, 0xFFFFFFFF);

	//	深度バッファに書き込み参照する
	context->OMSetDepthStencilState(states->DepthDefault(), 0);

	//	カリングは左周り
	context->RSSetState(states->CullNone());

	//	シェーダをセットする
	context->VSSetShader(m_vs.Get(), nullptr, 0);
	context->GSSetShader(m_gs.Get(), nullptr, 0);
	context->PSSetShader(m_ps.Get(), nullptr, 0);

	//	ピクセルシェーダにテクスチャを登録する
	context->PSSetShaderResources(0, 1, m_texture.GetAddressOf());

	//	インプットレイアウトの登録
	context->IASetInputLayout(m_inputLayout.Get());

	//	板ポリゴンを描画
	m_primitiveBatch->Begin();
	m_primitiveBatch->Draw(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST, &vertex[0], 4);
	m_primitiveBatch->End();

	//	シェーダの登録を解除しておく
	context->VSSetShader(nullptr, nullptr, 0);
	context->GSSetShader(nullptr, nullptr, 0);
	context->PSSetShader(nullptr, nullptr, 0);
}

//-------------------------------------------------------------------
// シェーダの読み込み
//-------------------------------------------------------------------
void DamageEffect::LoadShader()
{
	auto device = m_graphics->GetDeviceResources()->GetD3DDevice();

	// シェーダ読み込み変数
	std::vector<uint8_t> blob;

	// ピクセルシェーダ
	blob = DX::ReadData(L"Resources/Shaders/DamageEffectPS.cso");
	DX::ThrowIfFailed(
		device->CreatePixelShader(blob.data(), blob.size(), nullptr, m_ps.ReleaseAndGetAddressOf())
	);

	// 頂点シェーダ
	blob = DX::ReadData(L"Resources/Shaders/DamageEffectVS.cso");
	DX::ThrowIfFailed(
		device->CreateVertexShader(blob.data(), blob.size(), nullptr, m_vs.ReleaseAndGetAddressOf())
	);

	// インプットレイアウトの作成
	device->CreateInputLayout(&INPUT_LAYOUT[0],
		static_cast<UINT>(INPUT_LAYOUT.size()),
		blob.data(), blob.size(),
		m_inputLayout.GetAddressOf());

	// ジオメトリシェーダ
	blob = DX::ReadData(L"Resources/Shaders/DamageEffectGS.cso");
	DX::ThrowIfFailed(
		device->CreateGeometryShader(blob.data(), blob.size(), nullptr, m_gs.ReleaseAndGetAddressOf())
	);
}

//-------------------------------------------------------------------
// バッファの作成
//-------------------------------------------------------------------
void DamageEffect::CreateCBuffer()
{
	auto device = m_graphics->GetDeviceResources()->GetD3DDevice();
	
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(CBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	device->CreateBuffer(&bd, nullptr, &m_cBuffer);
}
