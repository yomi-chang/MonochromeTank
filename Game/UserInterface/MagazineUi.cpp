#include "pch.h"
#include "Game/UserInterface/MagazineUi.h"

#include "Framework/Graphics.h"
#include "Framework/InputManager.h"
#include "Framework/Resources.h"

#include "Game/Objects/Tank/PlayerTank.h"

#include "Libraries/Microsoft/ReadData.h"

MagazineUi::MagazineUi()
    : 
    m_windowHeight{},
    m_windowWidth{},
    m_player{},
    m_pixelShader{},
    m_bulletTexture{},
    m_cannonTexture{},
    m_frameTexture{},
    m_reloadTexture{},
    m_gaugeTexture{},
    m_graphics{ Graphics::GetInstance() },
    m_spriteBatch{},
    m_framePos{},
    m_cannonReloadPos{},
    m_bulletReloadPos{}
{
    // 画面サイズの受け取り
    m_graphics->GetScreenSize(m_windowWidth, m_windowHeight);
}

void MagazineUi::Initialize()
{
    using namespace DirectX;
    using namespace DirectX::SimpleMath;

    // シェーダの読み込み
    //LoadShader();

    // テクスチャの読み込み
    LoadTexture();

    // スプライトバッチの受け取り
    m_spriteBatch = m_graphics->GetSpriteBatch();
    m_spriteFont = m_graphics->GetFont();

    // 初期で選択されている弾は砲弾
    m_framePos = SELECT_CANNON;

    m_cannonReloadPos = CANNON_RELOAD_GAUGE_FRONT;
    m_bulletReloadPos = BULLET_RELOAD_GAUGE_FRONT;

    m_cannonReloadPos.right = CANNON_RELOAD_GAUGE_FRONT.left; 
    m_bulletReloadPos.right = BULLET_RELOAD_GAUGE_FRONT.left;

    // ウィンドウサイズの指定
    /*RECT rect = m_graphics->GetDeviceResources()->GetOutputSize();

    m_backBuffer = std::make_unique<DX::RenderTexture>(DXGI_FORMAT_R8G8B8A8_UNORM);
    m_backBuffer->SetDevice(m_graphics->GetDeviceResources()->GetD3DDevice());
    m_backBuffer->SetWindow(rect);*/
}

// 描画
void MagazineUi::Render()
{
    using namespace DirectX;

    // 残弾数の表示
    // 描画開始
    m_spriteBatch->Begin();

    // 現在セットされている弾の確認
    float bulletReloadRatio = 0.0f;
    int gaugeWidth = 0;
    if (m_player->GetTankCannon()->GetCurrentBullet() == TankCannon::BulletType::CANNONBALL)
    {
        // 選択されている弾の変更
        m_framePos = SELECT_CANNON;

        // リロードの進行の受け取り
        bulletReloadRatio = m_player->GetTankCannon()->GetCannonReloadRatio();

        // ゲージ全体の幅から増加量を計算
        gaugeWidth = CANNON_RELOAD_GAUGE_FRONT.right - CANNON_RELOAD_GAUGE_FRONT.left;
        m_cannonReloadPos.right = m_cannonReloadPos.left + static_cast<int>(gaugeWidth * bulletReloadRatio);

        // リロードが完了しているならゲージを0にする
        if (bulletReloadRatio == 1.0f) { m_cannonReloadPos.right = CANNON_RELOAD_GAUGE_FRONT.left; }
    }
    else
    {
        m_framePos = SELECT_BULLET;

        bulletReloadRatio = m_player->GetTankCannon()->GetBulletReloadRatio();

        gaugeWidth = BULLET_RELOAD_GAUGE_FRONT.right - BULLET_RELOAD_GAUGE_FRONT.left;
        m_bulletReloadPos.right = m_bulletReloadPos.left + static_cast<int>(gaugeWidth * bulletReloadRatio);

        if (bulletReloadRatio == 1.0f) { m_bulletReloadPos.right = BULLET_RELOAD_GAUGE_FRONT.left; }
    }

    // 砲弾
    // 使用済みの弾は色を変更する
    if (m_player->GetTankCannon()->GetCannonBall()->GetBulletState() == IBullet::UNUSED)
        m_spriteBatch->Draw(m_cannonTexture.Get(), CANNON_UI_POS);
    else
        m_spriteBatch->Draw(m_cannonTexture.Get(), CANNON_UI_POS, DirectX::Colors::DimGray);
    
    // リロードゲージ
    m_spriteBatch->Draw(m_gaugeTexture.Get(), CANNON_RELOAD_GAUGE_BACK, DirectX::Colors::Black);
    m_spriteBatch->Draw(m_gaugeTexture.Get(), m_cannonReloadPos, DirectX::Colors::Aqua);

    // 通常弾
    for (auto& bullet : m_player->GetTankCannon()->GetBullets())
    {
        if (bullet->GetBulletState() == IBullet::UNUSED)
            m_spriteBatch->Draw(m_bulletTexture.Get(), BULLET_UI_POS);
        else
            m_spriteBatch->Draw(m_bulletTexture.Get(), BULLET_UI_POS, DirectX::Colors::DimGray);
    }
    m_spriteBatch->Draw(m_gaugeTexture.Get(), BULLET_RELOAD_GAUGE_BACK, DirectX::Colors::Black);
    m_spriteBatch->Draw(m_gaugeTexture.Get(), m_bulletReloadPos, DirectX::Colors::Aqua);

    // 選択している弾を示すフレーム
    m_spriteBatch->Draw(m_frameTexture.Get(), m_framePos);

    // 残弾数の表示
    RECT rect = { 1180,620,1245,680 };
    m_spriteBatch->Draw(m_gaugeTexture.Get(), rect, DirectX::Colors::White);
    int num = 0;
    if (m_player->GetTankCannon()->GetCannonBall()->GetBulletState() == IBullet::UNUSED)
        num++;
    m_spriteFont->DrawString(
        m_spriteBatch,
        std::to_wstring(num).c_str(),
        DirectX::XMFLOAT2(1190, 615),   
        DirectX::Colors::Black,
        0.0f,                       
        DirectX::XMFLOAT2(0, 0),     
        2.0f
    );

    num = 0;
    rect = { 1180,520,1245,580 };
    m_spriteBatch->Draw(m_gaugeTexture.Get(), rect, DirectX::Colors::White);
    for (auto& bullet : m_player->GetTankCannon()->GetBullets())
    {
        if (bullet->GetBulletState() == IBullet::UNUSED)
            num++;
    }
    m_spriteFont->DrawString(
        m_spriteBatch,
        std::to_wstring(num).c_str(),
        DirectX::XMFLOAT2(1180, 515),
        DirectX::Colors::Black,
        0.0f,
        DirectX::XMFLOAT2(0, 0),
        2.0f
    );



    // 描画終了
    m_spriteBatch->End();



    //using namespace DirectX;
    //auto context = m_graphics->GetDeviceResources()->GetD3DDeviceContext();
    //auto deviceResources = m_graphics->GetDeviceResources();
    //
    //// レンダーターゲットの変更
    //auto rtv = m_backBuffer->GetRenderTargetView();
    //context->ClearRenderTargetView(rtv, Colors::Black);
    //context->OMSetRenderTargets(1, &rtv, nullptr);

    //// 描画座標の設定
    //SimpleMath::Vector2 position = SimpleMath::Vector2(100, 100);
    //auto spriteBatch = m_graphics->GetSpriteBatch();
    //spriteBatch->Begin();
    //spriteBatch->Draw(m_bulletTexture.Get(), position);
    //spriteBatch->End();
    //
    //ID3D11ShaderResourceView* nullSRV = nullptr;
    //context->PSSetShaderResources(0, 1, &nullSRV);

    //rtv = deviceResources->GetRenderTargetView();
    //auto depthStencil = deviceResources->GetDepthStencilView();
    //context->OMSetRenderTargets(1, &rtv, depthStencil);

    //ID3D11ShaderResourceView* srv = m_backBuffer->GetShaderResourceView();
    //context->PSSetShaderResources(0, 1, &srv);
    //context->PSSetShader(m_pixelShader.Get(), nullptr, 0);
}

// シェーダの読み込み
void MagazineUi::LoadShader()
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
void MagazineUi::LoadTexture()
{
    m_cannonTexture = Resources::GetInstance()->GetCannonBallTexture();
    m_bulletTexture = Resources::GetInstance()->GetBulletTexture();
    m_frameTexture = Resources::GetInstance()->GetFrameTexture();
    m_reloadTexture = Resources::GetInstance()->GetRelooadTexture();
    m_gaugeTexture = Resources::GetInstance()->GetBoxTexture();

    // 正しくテクスチャを受け取れているかの確認
    if (!m_bulletTexture || !m_cannonTexture)
    {
        OutputDebugString(L"Texture is null!\n");
        return;
    }
}
