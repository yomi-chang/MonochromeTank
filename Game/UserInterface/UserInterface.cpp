//--------------------------------------------------------------------------------------
// File: UserInterface.h
//
// ユーザーインターフェースクラス
//
//-------------------------------------------------------------------------------------

#include "pch.h"
#include "Game/UserInterface/UserInterface.h"
#include "Framework/Graphics.h"

#include "Libraries/MyLib/BinaryFile.h"
#include "DeviceResources.h"
#include <SimpleMath.h>
#include <Effects.h>
#include <PrimitiveBatch.h>
#include <VertexTypes.h>
#include <WICTextureLoader.h>
#include <CommonStates.h>
#include <vector>

#include "Libraries/Microsoft/ReadData.h"

using namespace DirectX;

// インプットレイアウト
const std::vector<D3D11_INPUT_ELEMENT_DESC> UserInterface::INPUT_LAYOUT =
{
	{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR",	0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, sizeof(SimpleMath::Vector3), D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(SimpleMath::Vector3)+ sizeof(SimpleMath::Vector4), D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

// コンストラクタ
UserInterface::UserInterface()
	:
	m_windowHeight{},
	m_windowWidth{},
	m_textureHeight{},
	m_textureWidth{},
	m_texture{},
	m_res{},
	m_scale{},
	m_position{},
	m_anchor(ANCHOR::TOP_LEFT)
{
	// グラフィックスの受け取り
	m_graphics = Graphics::GetInstance();

	// 画面サイズの指定
	m_graphics->SetScreenSize(m_windowWidth, m_windowHeight);
}

// デストラクタ
UserInterface::~UserInterface()
{
}

/// <summary>
/// テクスチャリソース読み込み関数
/// </summary>
/// <param name="path">相対パス(Resources/Textures/・・・.pngなど）</param>
void UserInterface::LoadTexture(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture)
{

	////	指定された画像を読み込む
	//auto device = m_graphics->GetDeviceResources()->GetD3DDevice();

	//m_texture = texture;
	///*HRESULT result = DirectX::CreateWICTextureFromFile(device, path, m_res.ReleaseAndGetAddressOf(), m_texture.ReleaseAndGetAddressOf());
	//Microsoft::WRL::ComPtr<ID3D11Texture2D> tex;
	//DX::ThrowIfFailed(m_res.As(&tex));*/

	////	読み込んだ画像の情報を取得する
	//D3D11_TEXTURE2D_DESC desc;
	////texture->GetDesc(&desc);

	////	読み込んだ画像のサイズを取得する
	//m_textureWidth = desc.Width;
	//m_textureHeight = desc.Height;
}

/// <summary>
/// 生成関数
/// </summary>
/// <param name="pDR">ユーザーリソース等から持ってくる</param>
void UserInterface::Create(
	const wchar_t* path,
	DirectX::SimpleMath::Vector2 position,
	DirectX::SimpleMath::Vector2 scale,
	ANCHOR anchor)
{
	//auto context = m_graphics->GetDeviceResources()->GetD3DDeviceContext();
	//
	//m_position = position;
	//m_baseScale = m_scale = scale;
	//m_anchor = anchor;

	//// シェーダーの作成
	//CreateShader();

	//// 画像の読み込み
	//LoadTexture(path);

	////	プリミティブバッチの作成
	//m_batch = std::make_unique<PrimitiveBatch<VertexPositionColorTexture>>(context);

	////m_states = m_graphics->GetCommonStates();
}

// Shader作成部分だけ分離した関数
void UserInterface::CreateShader()
{
	//auto device = m_graphics->GetDeviceResources()->GetD3DDevice();

	////	コンパイルされたシェーダファイルを読み込み
	///*mylib::BinaryFile VSData = mylib::BinaryFile::LoadFile(L"Resources/Shaders/UIVS.cso");
	//mylib::BinaryFile GSData = mylib::BinaryFile::LoadFile(L"Resources/Shaders/UIGS.cso");
	//mylib::BinaryFile PSData = mylib::BinaryFile::LoadFile(L"Resources/Shaders/UIPS.cso");*/


	//// シェーダ読み込み用変数
	//std::vector<uint8_t> blob;

	//// ピクセルシェーダの読み込み
	//blob = DX::ReadData(L"Resources/Shaders/UiPS.cso");
	//DX::ThrowIfFailed(
	//	device->CreatePixelShader(
	//		blob.data(), blob.size(), nullptr,
	//		m_pixelShader.ReleaseAndGetAddressOf()
	//	)
	//);

	//// 頂点シェーダの読み込み
	//blob = DX::ReadData(L"Resources/Shaders/UiVS.cso");
	//DX::ThrowIfFailed(
	//	device->CreateVertexShader(
	//		blob.data(), blob.size(), nullptr,
	//		m_vertexShader.ReleaseAndGetAddressOf()
	//	)
	//);

	////	インプットレイアウトの作成
	//device->CreateInputLayout(&INPUT_LAYOUT[0],
	//	static_cast<UINT>(INPUT_LAYOUT.size()),
	//	blob.data(), blob.size(),
	//	m_inputLayout.GetAddressOf());

	//// ジオメトリシェーダの読み込み
	//blob = DX::ReadData(L"Resources/Shaders/UiVS.cso");
	//DX::ThrowIfFailed(
	//	device->CreateVertexShader(
	//		blob.data(), blob.size(), nullptr,
	//		m_vertexShader.ReleaseAndGetAddressOf()
	//	)
	//);

	//////	頂点シェーダ作成
	////if (FAILED(device->CreateVertexShader(VSData.GetData(), VSData.GetSize(), NULL, m_vertexShader.ReleaseAndGetAddressOf())))
	////{//	エラー
	////	MessageBox(0, L"CreateVertexShader Failed.", NULL, MB_OK);
	////	return;
	////}
	//////	ジオメトリシェーダ作成
	////if (FAILED(device->CreateGeometryShader(GSData.GetData(), GSData.GetSize(), NULL, m_geometryShader.ReleaseAndGetAddressOf())))
	////{// エラー
	////	MessageBox(0, L"CreateGeometryShader Failed.", NULL, MB_OK);
	////	return;
	////}
	//////	ピクセルシェーダ作成
	////if (FAILED(device->CreatePixelShader(PSData.GetData(), PSData.GetSize(), NULL, m_pixelShader.ReleaseAndGetAddressOf())))
	////{// エラー
	////	MessageBox(0, L"CreatePixelShader Failed.", NULL, MB_OK);
	////	return;
	////}

	////	シェーダーにデータを渡すためのコンスタントバッファ生成
	//D3D11_BUFFER_DESC bd;
	//ZeroMemory(&bd, sizeof(bd));
	//bd.Usage = D3D11_USAGE_DEFAULT;
	//bd.ByteWidth = sizeof(ConstBuffer);
	//bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	//bd.CPUAccessFlags = 0;
	//device->CreateBuffer(&bd, nullptr, &m_CBuffer);
}

// 描画関数
void UserInterface::Render()
{
	//auto context = m_graphics->GetDeviceResources()->GetD3DDeviceContext();
	//auto state = m_graphics->GetCommonStates();

	//	// 頂点情報
	//	// Position.xy	:拡縮用スケール
	//	// Position.z	:アンカータイプ(0～8)の整数で指定
	//	// Color.xy　	:アンカー座標(ピクセル指定:1280 ×720)
	//	// Color.zw		:画像サイズ
	//	// Tex.xy		:ウィンドウサイズ（バッファも同じ。こちらは未使用）
	//VertexPositionColorTexture vertex[1] = {
	//	VertexPositionColorTexture(SimpleMath::Vector3(m_scale.x, m_scale.y, static_cast<float>(m_anchor))
	//	, SimpleMath::Vector4(m_position.x, m_position.y, static_cast<float>(m_textureWidth), static_cast<float>(m_textureHeight))
	//	, SimpleMath::Vector2(static_cast<float>(m_windowWidth), static_cast<float>(m_windowHeight)))
	//};
	////	ただし上記の設定値には、WorldやViewなどの3D空間から変換するための計算を一切しないため、
	////	スクリーン座標として描画される

	////	シェーダーに渡す追加のバッファを作成する。(ConstBuffer）
	//ConstBuffer cbuff;
	//cbuff.windowSize = SimpleMath::Vector4(static_cast<float>(m_windowWidth), static_cast<float>(m_windowHeight), 1, 1);

	////	受け渡し用バッファの内容更新(ConstBufferからID3D11Bufferへの変換）
	//context->UpdateSubresource(m_CBuffer.Get(), 0, NULL, &cbuff, 0, 0);

	////	シェーダーにバッファを渡す
	//ID3D11Buffer* cb[1] = { m_CBuffer.Get() };
	//context->VSSetConstantBuffers(0, 1, cb);
	//context->GSSetConstantBuffers(0, 1, cb);
	//context->PSSetConstantBuffers(0, 1, cb);

	////	画像用サンプラーの登録
	//ID3D11SamplerState* sampler[1] = { state->LinearWrap() };
	//context->PSSetSamplers(0, 1, sampler);


	////	半透明描画指定
	//ID3D11BlendState* blendstate = state->NonPremultiplied();

	////	透明判定処理
	//context->OMSetBlendState(blendstate, nullptr, 0xFFFFFFFF);

	////	深度バッファに書き込み参照する
	//context->OMSetDepthStencilState(state->DepthDefault(), 0);

	////	カリングは左周り
	//context->RSSetState(state->CullNone());

	////	シェーダをセットする
	//context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
	//context->GSSetShader(m_geometryShader.Get(), nullptr, 0);
	//context->PSSetShader(m_pixelShader.Get(), nullptr, 0);

	////	ピクセルシェーダにテクスチャを登録する。
	//context->PSSetShaderResources(0, 1, m_texture.GetAddressOf());

	////	インプットレイアウトの登録
	//context->IASetInputLayout(m_inputLayout.Get());

	////	板ポリゴンを描画
	//m_batch->Begin();
	//m_batch->Draw(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST, &vertex[0], 1);
	//m_batch->End();

	////	シェーダの登録を解除しておく
	//context->VSSetShader(nullptr, nullptr, 0);
	//context->GSSetShader(nullptr, nullptr, 0);
	//context->PSSetShader(nullptr, nullptr, 0);
}