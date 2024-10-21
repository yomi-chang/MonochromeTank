#include "pch.h"
#include "Game/UserInterface/Magazine.h"

#include "Framework/Graphics.h"
#include "Framework/InputManager.h"

#include "Libraries/Microsoft/ReadData.h"

using namespace DirectX;

Magazine::Magazine()
    : 
    m_windowHeight{},
    m_windowWidth{},
    m_graphics{ Graphics::GetInstance() }
{
    // 画面サイズの受け取り
    m_graphics->GetScreenSize(m_windowWidth, m_windowHeight);

    // スプライトバッチの受け取り
    m_spriteBatch = m_graphics->GetSpriteBatch();
}

void Magazine::Initialize()
{
    // シェーダの読み込み
    LoadShader();
}

// 更新
void Magazine::Update()
{
}

// 描画
void Magazine::Render()
{
    auto context = m_graphics->GetDeviceResources()->GetD3DDeviceContext();

    // シェーダのセット
    context->PSSetShader(m_pixelShader.Get(), nullptr, 0);

    // ピクセルシェーダにテクスチャを登録
    context->PSSetShaderResources(0, 1, m_texture.GetAddressOf());

    // 描画
    auto batch = m_graphics->GetPrimitiveBatch();
    batch->Begin();
    batch->End();

    // シェーダの登録解除
    context->PSSetShader(nullptr, nullptr, 0);
}

// シェーダの読み込み
void Magazine::LoadShader()
{
    auto device = m_graphics->GetDeviceResources()->GetD3DDevice();
    
    // シェーダ読み込み用変数
    std::vector<uint8_t> blob;

    // ピクセルシェーダの読み込み
    blob = DX::ReadData(L"Resources/Shaders/ピクセルシェーダの名前");
    DX::ThrowIfFailed(
        device->CreatePixelShader(
            blob.data(), blob.size(), nullptr,
            m_pixelShader.ReleaseAndGetAddressOf()
        )
    );
}





