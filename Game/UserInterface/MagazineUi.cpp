/*
    @file	MagazineUi.cpp
    @brief	弾関係UIクラス
*/
#include "pch.h"
#include "Game/UserInterface/MagazineUi.h"

#include "Framework/Graphics.h"
#include "Framework/InputManager.h"
#include "Framework/Resources.h"
#include "Libraries/MyLib/Utils.h"
#include "Game/Screen.h"

#include "Game/Objects/Tank/PlayerTank.h"

//-------------------------------------------------------------------
// コンストラクタ
//-------------------------------------------------------------------
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
    m_reloadPos{}
{
    // 画面サイズの受け取り
    m_graphics->GetScreenSize(m_windowWidth, m_windowHeight);
}

//-------------------------------------------------------------------
// 初期化処理
//-------------------------------------------------------------------
void MagazineUi::Initialize()
{
    using namespace DirectX;
    using namespace DirectX::SimpleMath;

    // テクスチャの読み込み
    LoadTexture();

    // スプライトバッチの受け取り
    m_spriteBatch = m_graphics->GetSpriteBatch();

    m_reloadPos = RELOAD_GAUGE_BACK;
    m_reloadPos.right = RELOAD_GAUGE_BACK.left;
}

//-------------------------------------------------------------------
// 描画処理
//-------------------------------------------------------------------
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
        // リロードの進行の受け取り
        bulletReloadRatio = m_player->GetTankCannon()->GetCannonReloadRatio();
        // ゲージ全体の幅から増加量を計算
        gaugeWidth = RELOAD_GAUGE_BACK.right - RELOAD_GAUGE_BACK.left;
        m_reloadPos.right = m_reloadPos.left + static_cast<int>(gaugeWidth * bulletReloadRatio);
        // リロードが完了しているならゲージを0にする
        if (bulletReloadRatio == 1.0f) { m_reloadPos.right = RELOAD_GAUGE_BACK.left; }
    }
    else
    {
        bulletReloadRatio = m_player->GetTankCannon()->GetBulletReloadRatio();
        gaugeWidth = RELOAD_GAUGE_BACK.right - RELOAD_GAUGE_BACK.left;
        m_reloadPos.right = m_reloadPos.left + static_cast<int>(gaugeWidth * bulletReloadRatio);
        if (bulletReloadRatio == 1.0f) { m_reloadPos.right = RELOAD_GAUGE_BACK.left; }
    }

    // 選択されていない弾
    SubBulletUi();
    // 選択されている弾
    MainBulletUi();

    // 描画終了
    m_spriteBatch->End();
}

//-------------------------------------------------------------------
// テクスチャの読み込み
//-------------------------------------------------------------------
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

// サブの弾のUI表示
void MagazineUi::SubBulletUi()
{
    // 枠
    DirectX::SimpleMath::Vector4 color = static_cast<DirectX::SimpleMath::Vector4>(DirectX::Colors::Gray);
    color.w = 0.5f;
    m_spriteBatch->Draw(m_gaugeTexture, SUB_BULLET_FRAME, color);

    // 砲弾を選択している場合
    if (m_player->GetTankCannon()->GetCurrentBullet() == TankCannon::BulletType::CANNONBALL)
        m_spriteBatch->Draw(m_bulletTexture, SUB_BULLET_UI , DirectX::Colors::DimGray);
    // 連射弾を選択している場合
    else
        m_spriteBatch->Draw(m_cannonTexture, SUB_BULLET_UI, DirectX::Colors::DimGray);
}

// メインの弾のUI表示
void MagazineUi::MainBulletUi()
{
    using namespace DirectX;
    using namespace DirectX::SimpleMath;

    // 枠
    DirectX::SimpleMath::Vector4 color = static_cast<DirectX::SimpleMath::Vector4>(DirectX::Colors::WhiteSmoke);
    color.w = 0.5f;
    m_spriteBatch->Draw(m_gaugeTexture, MAIN_BULLET_FRAME, color);

    // 砲弾を選択している場合
    if (m_player->GetTankCannon()->GetCurrentBullet() == TankCannon::BulletType::CANNONBALL)
        m_spriteBatch->Draw(m_cannonTexture, MAIN_BULLET_UI);
    // 連射弾を選択している場合
    else
        m_spriteBatch->Draw(m_bulletTexture, MAIN_BULLET_UI);
    
    // リロードゲージ
    m_spriteBatch->Draw(m_gaugeTexture, RELOAD_GAUGE_BACK, DirectX::Colors::Black);
    m_spriteBatch->Draw(m_gaugeTexture, m_reloadPos, DirectX::Colors::Aqua);

    // 弾変更UIの描画
    m_spriteBatch->Draw(
        Resources::GetInstance()->GetSpaceKeyTexture(),
        Vector2(Screen::CENTER_X + 560 ,Screen::CENTER_Y + 300),                  
        nullptr,                                     
        DirectX::Colors::White,                          
        0.0f,                                          
        mylib::GetTextureCenter(Resources::GetInstance()->GetSpaceKeyTexture()),
        0.25f
    );

    int value = CheckBulletValue();
    RECT rect = { 0,0,0,0 };
    rect.left = (BASE_POS_X + value / 10) * FONT_SIZE;
    rect.right = rect.left + FONT_SIZE;
    rect.bottom = FONT_SIZE;
    // 二桁目
    m_spriteBatch->Draw(
        Resources::GetInstance()->GetFontTexture(),
        DirectX::XMFLOAT2(1110  , 640),
        &rect,
        DirectX::Colors::White,
        0.0,
        Vector2::Zero,
        FONT_SCALE
    );

    rect.left = (BASE_POS_X + value % 10) * FONT_SIZE;
    rect.right = rect.left + FONT_SIZE;
    // 一桁目
    m_spriteBatch->Draw(
        Resources::GetInstance()->GetFontTexture(),
        DirectX::XMFLOAT2(1135, 640),
        &rect,
        DirectX::Colors::White,
        0.0,
        Vector2::Zero,
        FONT_SCALE
    );

}

// 残弾数の確認
int MagazineUi::CheckBulletValue()
{
    int num = 0;
    // 砲弾の残り弾数の確認
    if (m_player->GetTankCannon()->GetCurrentBullet() == TankCannon::BulletType::CANNONBALL)
    {
        if (m_player->GetTankCannon()->GetCannonBall()->GetBulletState() == IBullet::UNUSED)
            num++;
    }
    else
    {
        for (auto& bullet : m_player->GetTankCannon()->GetBullets())
        {
            if (bullet->GetBulletState() == IBullet::UNUSED)
                num++;
        }
    }
    return num;
}


