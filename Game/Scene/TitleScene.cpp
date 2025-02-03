/*
	@file	TitleScene.cpp
	@brief	タイトルシーンクラス
*/
#include "pch.h"
#include "TitleScene.h"
#include "Game/Screen.h"
#include "DeviceResources.h"
#include "Libraries/MyLib/MemoryLeakDetector.h"
#include "Framework/InputManager.h"
#include "Framework/Graphics.h"
#include "Framework/Resources.h"

#include "Game/Objects/Stage/Floor.h"
#include "Game/Objects/Tank/TankBase/Tank.h"
#include "Libraries/MyLib/LockOnCamera.h"
#include "Game/Scene/Fade.h"
#include <cassert>

using namespace DirectX;
using namespace DirectX::SimpleMath;

//---------------------------------------------------------
// コンストラクタ
//---------------------------------------------------------
TitleScene::TitleScene()
	:
	m_graphics{ Graphics::GetInstance()},
	m_spriteBatch{},
	m_spriteFont{},
	m_titleLogo{},
	m_pressSpace{},
	m_texCenter{},
	m_isChangeScene{},
	m_floor{},
	m_tanks{},
	m_fade{}
{
}

//---------------------------------------------------------
// デストラクタ
//---------------------------------------------------------
TitleScene::~TitleScene()
{
	// do nothing.
}

//---------------------------------------------------------
// 初期化する
//---------------------------------------------------------
void TitleScene::Initialize()
{	
	// BGMの再生
	SharedData::GetInstance()->GetSoundManager()->PlayBGM(XACT_WAVEBANK_SOUNDS_TITLESCENE_BGM);

	// スプライトバッチを作成する
	m_spriteBatch = m_graphics->GetSpriteBatch();
	m_spriteFont = m_graphics->GetFont();

	// 画像の受け取り
	m_titleLogo = Resources::GetInstance()->GetTitleLogoTexture();
	m_pressSpace = Resources::GetInstance()->GetPressSpaceTexture();

	/*
		以下、テクスチャの大きさを求める→テクスチャの中心座標を計算する
	*/
	// 一時的な変数の宣言
	Microsoft::WRL::ComPtr<ID3D11Resource> resource{};
	Microsoft::WRL::ComPtr<ID3D11Texture2D> tex2D{};
	D3D11_TEXTURE2D_DESC desc{};
	Vector2 texSize{};

	// テクスチャの情報を取得する================================
	// テクスチャをID3D11Resourceとして見る
	m_titleLogo->GetResource(resource.GetAddressOf());

	// ID3D11ResourceをID3D11Texture2Dとして見る
	resource.As(&tex2D);

	// テクスチャ情報を取得する
	tex2D->GetDesc(&desc);

	// テクスチャサイズを取得し、float型に変換する
	texSize.x = static_cast<float>(desc.Width);
	texSize.y = static_cast<float>(desc.Height);

	// テクスチャの中心位置を計算する
	m_texCenter = texSize / 2.0f;


	// 床の生成
	m_floor = std::make_unique<Floor>(50);
	m_floor->SetTexture(Resources::GetInstance()->GetFloorTexture());

	m_tanks.push_back(std::make_unique<Tank>(1, Vector3{ -1.5f, 0.1f, -1.5f }, DirectX::XMConvertToRadians(-135.0f)));
	m_tanks.push_back(std::make_unique<Tank>(2, Vector3{ 1.5f, 0.1f, -1.5f }, DirectX::XMConvertToRadians(135.0f)));
	m_tanks.push_back(std::make_unique<Tank>(3, Vector3{ -1.5f, 0.1f, 1.5f }, DirectX::XMConvertToRadians(-45.0f)));
	m_tanks.push_back(std::make_unique<Tank>(4, Vector3{ 1.5f, 0.1f, 1.5f }, DirectX::XMConvertToRadians(45.0f)));
	for (auto& tank : m_tanks)
	{
		tank->Initialize();
	}

	// 射影行列を作成する
	RECT rect{ m_graphics->GetDeviceResources()->GetOutputSize() };
	Matrix projection;
	projection = Matrix::CreatePerspectiveFieldOfView(
		XMConvertToRadians(65.0f),
		static_cast<float>(rect.right) / static_cast<float>(rect.bottom),
		0.1f, 1000.0f
	);

	// 射影行列を設定する
	m_graphics->SetProjectionMatrix(projection);

	// TPSカメラの生成
	m_camera = std::make_unique<mylib::LockOnCamera>();
	m_camera->Initialize();
	m_camera->SetTargetPosition(Vector3(0, 0, 0));
	m_camera->SetDistance(1.0f);
	m_camera->SetHeight(3.0f);
	m_camera->SetEyePosition(DirectX::SimpleMath::Vector3(0.0f, 5.0f, 5.0f));

	// シーン遷移用
	m_fade = std::make_unique<Fade>(1.0f);
	m_fade->FadeOut();

	// シーン変更フラグを初期化する
	m_isChangeScene = false;
}

//---------------------------------------------------------
// 更新する
//---------------------------------------------------------
void TitleScene::Update(float elapsedTime)
{
	// 宣言をしたが、実際は使用していない変数
	UNREFERENCED_PARAMETER(elapsedTime);

	// フェード
	m_fade->Update(elapsedTime);

	// 敵戦車の更新
	for (auto& tank : m_tanks)
	{
		tank->Update(elapsedTime);
	}

	// フォローカメラを更新する
	m_camera->Update(elapsedTime);

	// フェードイン中でフェードが終了しているならシーン遷移
	if (m_fade->GetFadeType() == Fade::FADEIN &&
		m_fade->FinishFade())
	{
		m_isChangeScene = true;
	}

	// フェードが終了していないなら早期リターン
	if (!m_fade->FinishFade()){ return; }
	
	// キーボードステートの取得
	const auto& kbTracker = InputManager::GetInstance()->GetKeyboardTracker();

	// スペースキーが押されたらフェード開始
	if (kbTracker->IsKeyPressed(DirectX::Keyboard::Space))
		m_fade->FadeIn();
}

//---------------------------------------------------------
// 描画する
//---------------------------------------------------------
void TitleScene::Render()
{
	auto states = m_graphics->GetCommonStates();

	// ビュー行列の取得
	auto view = Matrix::CreateLookAt(
		m_camera->GetEyePosition(),
		m_camera->GetTargetPosition(),
		Vector3::UnitY
	);
	Graphics::GetInstance()->SetViewMatrix(view);

	// 床の描画
	m_floor->Render();

	// 敵戦車の更新
	for (auto& tank : m_tanks)
	{
		tank->Render();
	}

	// スプライトバッチの開始：オプションでソートモード、ブレンドステートを指定する
	m_spriteBatch->Begin(SpriteSortMode_Deferred, states->NonPremultiplied());

	// ロゴの描画位置を決める
	RECT rect{ m_graphics->GetDeviceResources()->GetOutputSize() };
	// 画像の中心を計算する
	Vector2 pos{ rect.right / 2.0f, rect.bottom / 2.0f };

	// ロゴを描画する
	m_spriteBatch->Draw(
		m_titleLogo,	// テクスチャ(SRV)
		pos,				// スクリーンの表示位置(originの描画位置)
		nullptr,			// 矩形(RECT)
		Colors::White,		// 背景色
		0.0f,				// 回転角(ラジアン)
		m_texCenter,		// テクスチャの基準になる表示位置(描画中心)(origin)
		1.5f				// スケール(scale)
	);

	rect = { 400,500,920,620 };
	// UIの描画
	m_spriteBatch->Draw(
		m_pressSpace,
		rect
	);

	// スプライトバッチの終わり
	m_spriteBatch->End();

	// シーン遷移用
	m_fade->Render();
}

//---------------------------------------------------------
// 後始末する
//---------------------------------------------------------
void TitleScene::Finalize()
{
	// do nothing.
}

//---------------------------------------------------------
// 次のシーンIDを取得する
//---------------------------------------------------------
IScene::SceneID TitleScene::GetNextSceneID() const
{
	// シーン変更がある場合
	if (m_isChangeScene)
	{
		return IScene::SceneID::SELECT;
	}

	// シーン変更がない場合
	return IScene::SceneID::NONE;
}
