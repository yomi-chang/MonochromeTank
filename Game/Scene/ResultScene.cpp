/*
	@file	ResultScene.cpp
	@brief	リザルトシーンクラス
*/
#include "pch.h"
#include "ResultScene.h"
#include "Game/Screen.h"
#include "DeviceResources.h"
#include "Libraries/MyLib/MemoryLeakDetector.h"
#include "Framework/InputManager.h"
#include "Game/Other/SharedData.h"
#include "Game/Objects/Tank/TankBase/Tank.h"
#include "Libraries/MyLib/LockOnCamera.h"
#include "Game/Objects/Stage/Floor.h"
#include "Game/Scene/Fade.h"
#include "Game/Objects/Stage/StageObject/SkySphere.h"
#include <cassert>

using namespace DirectX;
using namespace DirectX::SimpleMath;

//---------------------------------------------------------
// コンストラクタ
//---------------------------------------------------------
ResultScene::ResultScene()
	:
	m_graphics{Graphics::GetInstance()},
	m_spriteBatch{},
	m_texture{},
	m_texCenter{},
	m_isChangeScene{},
	m_tank{},
	m_camera{},
	m_floor{},
	m_texturePos{}
{
}

//---------------------------------------------------------
// デストラクタ
//---------------------------------------------------------
ResultScene::~ResultScene()
{
	// do nothing.
}

//---------------------------------------------------------
// 初期化する
//---------------------------------------------------------
void ResultScene::Initialize()
{
	// スプライトバッチを作成する
	m_spriteBatch = m_graphics->GetSpriteBatch();

	//// 画像をロードする
	//DX::ThrowIfFailed(
	//	CreateWICTextureFromFile(
	//		device,
	//		L"Resources/Textures/Result.png",
	//		nullptr,
	//		m_texture.ReleaseAndGetAddressOf()
	//	)
	//);

	m_texture = Resources::GetInstance()->GetResultTexture();

	
	Microsoft::WRL::ComPtr<ID3D11Resource> resource{};
	Microsoft::WRL::ComPtr<ID3D11Texture2D> tex2D{};
	D3D11_TEXTURE2D_DESC desc{};
	Vector2 texSize{};

	// テクスチャの情報を取得する================================
	// テクスチャをID3D11Resourceとして見る
	m_texture->GetResource(resource.GetAddressOf());

	// ID3D11ResourceをID3D11Texture2Dとして見る
	resource.As(&tex2D);

	// テクスチャ情報を取得する
	tex2D->GetDesc(&desc);

	// テクスチャサイズを取得し、float型に変換する
	texSize.x = static_cast<float>(desc.Width);
	texSize.y = static_cast<float>(desc.Height);

	// テクスチャの中心位置を計算する
	m_texCenter = texSize / 2.0f;

	// シーン変更フラグを初期化する
	m_isChangeScene = false;

	// 勝利した戦車情報を受け取る
	m_tank = SharedData::GetInstance()->GetWinnerTank();
	m_tank->GetCannon()->SetDisplaySight(false);
	if (m_tank->GetTankNumber() == 0)
	{
		m_texturePos = { 0,0,700,100 };
	}
	else
	{
		m_texturePos = { 0,100,700,200 };
	}

	// TPSカメラの生成
	m_camera = std::make_unique<mylib::LockOnCamera>();
	m_camera->Initialize();
	m_camera->SetTargetPosition(m_tank->GetPosition());
	m_camera->SetEyePosition(DirectX::SimpleMath::Vector3(0.0f, 1.0f, 1.0f));

	// 床の生成
	m_floor = std::make_unique<Floor>(50);
	m_floor->SetTexture(Resources::GetInstance()->GetFloorTexture());

	// シーン遷移用
	m_fade = std::make_unique<Fade>(1.0f);
	m_fade->FadeOut();

	// 天球
	m_skySphere = std::make_unique<SkySphere>();
}

//---------------------------------------------------------
// 更新する
//---------------------------------------------------------
void ResultScene::Update(float elapsedTime)
{
	// 宣言をしたが、実際は使用していない変数
	UNREFERENCED_PARAMETER(elapsedTime);

	// 戦車の更新
	float angle = DirectX::XMConvertToRadians(0.7f);
	m_tank->GetBody()->Rotate(Quaternion::CreateFromYawPitchRoll(angle, 0.0f, 0.0f));
	m_tank->Update(elapsedTime);

	// フォローカメラを更新する
	m_camera->Update(elapsedTime);

	// フェード
	m_fade->Update(elapsedTime);

	// フェードイン中でフェードが終了しているならシーン遷移
	if (m_fade->GetFadeType() == Fade::FADEIN &&
		m_fade->FinishFade())
	{
		m_isChangeScene = true;
	}

	// フェードが終了していないなら早期リターン
	if (!m_fade->FinishFade()) { return; }


	// キーボードステートトラッカーを取得する
	const auto& kbTracker = InputManager::GetInstance()->GetKeyboardTracker();

	// スペースキーが押されたら
	if (kbTracker->IsKeyPressed(DirectX::Keyboard::Space))
		m_fade->FadeIn();
}

//---------------------------------------------------------
// 描画する
//---------------------------------------------------------
void ResultScene::Render()
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

	// 戦車の描画
	m_tank->Render();

	// 天球の描画
	m_skySphere->Render();

	// スプライトバッチの開始：オプションでソートモード、ブレンドステートを指定する
	m_spriteBatch->Begin(SpriteSortMode_Deferred, states->NonPremultiplied());

	// ロゴの描画位置を決める
	RECT rect{ m_graphics->GetDeviceResources()->GetOutputSize() };
	// 画像の中心を計算する
	Vector2 pos{ rect.right / 2.0f, rect.bottom / 2.0f };

	// TRIDENTロゴを描画する
	m_spriteBatch->Draw(
		m_texture.Get(),	// テクスチャ(SRV)
		pos,				// スクリーンの表示位置(originの描画位置)
		&m_texturePos,		// 矩形(RECT)
		Colors::White,		// 背景色
		0.0f,				// 回転角(ラジアン)
		m_texCenter,		// テクスチャの基準になる表示位置(描画中心)(origin)
		1.0f				// スケール(scale)
	);

	// スプライトバッチの終わり
	m_spriteBatch->End();
}

//---------------------------------------------------------
// 後始末する
//---------------------------------------------------------
void ResultScene::Finalize()
{
	// do nothing.
}

//---------------------------------------------------------
// 次のシーンIDを取得する
//---------------------------------------------------------
IScene::SceneID ResultScene::GetNextSceneID() const
{
	// シーン変更がある場合
	if (m_isChangeScene)
	{
		return IScene::SceneID::TITLE;
	}

	// シーン変更がない場合
	return IScene::SceneID::NONE;
}
