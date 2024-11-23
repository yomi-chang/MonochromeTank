#include "pch.h"
#include "Game/UserInterface/Magazine.h"

#include "Framework/Graphics.h"
#include "Framework/InputManager.h"
#include "Framework/Resources.h"

#include "Libraries/Microsoft/ReadData.h"

Magazine::Magazine()
    : 
    m_windowHeight{},
    m_windowWidth{},
    m_player{},
    m_pixelShader{},
    m_bulletTexture{},
    m_cannonTexture{},
    m_graphics{ Graphics::GetInstance() }
{
    // 画面サイズの受け取り
    m_graphics->GetScreenSize(m_windowWidth, m_windowHeight);
}

void Magazine::Initialize()
{
    using namespace DirectX;
    using namespace DirectX::SimpleMath;

    // シェーダの読み込み
    LoadShader();

    // テクスチャの読み込み
    LoadTexture();

    // ウィンドウサイズの指定
    RECT rect = m_graphics->GetDeviceResources()->GetOutputSize();

    m_backBuffer = std::make_unique<DX::RenderTexture>(DXGI_FORMAT_R8G8B8A8_UNORM);
    m_backBuffer->SetDevice(m_graphics->GetDeviceResources()->GetD3DDevice());
    m_backBuffer->SetWindow(rect);
}

// 描画
void Magazine::Render()
{
    using namespace DirectX;
    auto context = m_graphics->GetDeviceResources()->GetD3DDeviceContext();
    auto deviceResources = m_graphics->GetDeviceResources();
    
    // レンダーターゲットの変更
    auto rtv = m_backBuffer->GetRenderTargetView();
    context->ClearRenderTargetView(rtv, Colors::Black);
    context->OMSetRenderTargets(1, &rtv, nullptr);

    // 描画座標の設定
    SimpleMath::Vector2 position = SimpleMath::Vector2(100, 100);
    auto spriteBatch = m_graphics->GetSpriteBatch();
    spriteBatch->Begin();
    spriteBatch->Draw(m_bulletTexture.Get(), position);
    spriteBatch->End();
    
    ID3D11ShaderResourceView* nullSRV = nullptr;
    context->PSSetShaderResources(0, 1, &nullSRV);

    rtv = deviceResources->GetRenderTargetView();
    auto depthStencil = deviceResources->GetDepthStencilView();
    context->OMSetRenderTargets(1, &rtv, depthStencil);

    ID3D11ShaderResourceView* srv = m_backBuffer->GetShaderResourceView();
    context->PSSetShaderResources(0, 1, &srv);
    context->PSSetShader(m_pixelShader.Get(), nullptr, 0);
}

// シェーダの読み込み
void Magazine::LoadShader()
{
    auto device = m_graphics->GetDeviceResources()->GetD3DDevice();
    
    // シェーダ読み込み用変数
    std::vector<uint8_t> blob;

    // ピクセルシェーダの読み込み
    blob = DX::ReadData(L"Resources/Shaders/TexturePS.cso");
    DX::ThrowIfFailed(
        device->CreatePixelShader(
            blob.data(), blob.size(), nullptr,
            m_pixelShader.ReleaseAndGetAddressOf()
        )
    );
}

// テクスチャの読み込み
void Magazine::LoadTexture()
{
    // 砲弾テクスチャの受け取り
    m_cannonTexture = Resources::GetInstance()->GetCannonBallTexture();

    // 通常弾テクスチャの受け取り
    m_bulletTexture = Resources::GetInstance()->GetBulletTexture();

    // 正しくテクスチャを受け取れているかの確認
    if (!m_bulletTexture || !m_cannonTexture)
    {
        OutputDebugString(L"Texture is null!\n");
        return;
    }
}
