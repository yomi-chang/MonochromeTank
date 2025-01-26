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
	m_spriteFont{},
	m_texture{},
	m_texCenter{},
	m_isChangeScene{},
	m_tank{},
	m_camera{},
	m_floor{}
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
	auto device = m_graphics->GetDeviceResources()->GetD3DDevice();

	// スプライトバッチを作成する
	m_spriteBatch = m_graphics->GetSpriteBatch();
	m_spriteFont = m_graphics->GetFont();

	// 画像をロードする
	DX::ThrowIfFailed(
		CreateWICTextureFromFile(
			device,
			L"Resources/Textures/Result.png",
			nullptr,
			m_texture.ReleaseAndGetAddressOf()
		)
	);


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

	// TPSカメラの生成
	m_camera = std::make_unique<mylib::LockOnCamera>();
	m_camera->Initialize();
	m_camera->SetTargetPosition(m_tank->GetPosition());
	m_camera->SetEyePosition(DirectX::SimpleMath::Vector3(0.0f, 1.0f, 1.0f));

	// 床の生成
	m_floor = std::make_unique<Floor>(50);
	m_floor->SetTexture(Resources::GetInstance()->GetFloorTexture());
}

//---------------------------------------------------------
// 更新する
//---------------------------------------------------------
void ResultScene::Update(float elapsedTime)
{
	// 宣言をしたが、実際は使用していない変数
	UNREFERENCED_PARAMETER(elapsedTime);

	// キーボードステートトラッカーを取得する
	const auto& kbTracker = InputManager::GetInstance()->GetKeyboardTracker();

	// スペースキーが押されたら
	if (kbTracker->IsKeyPressed(DirectX::Keyboard::Space))
	{
		m_isChangeScene = true;
	}

	float angle = DirectX::XMConvertToRadians(0.7f);
	m_tank->GetBody()->Rotate(Quaternion::CreateFromYawPitchRoll(angle, 0.0f, 0.0f));
	m_tank->Update(elapsedTime);

	// フォローカメラを更新する
	m_camera->Update(elapsedTime);
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

	m_floor->Render();

	m_tank->Render();

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
		nullptr,			// 矩形(RECT)
		Colors::White,		// 背景色
		0.0f,				// 回転角(ラジアン)
		m_texCenter,		// テクスチャの基準になる表示位置(描画中心)(origin)
		1.0f,				// スケール(scale)
		SpriteEffects_None,	// エフェクト(effects)
		0.0f				// レイヤ深度(画像のソートで必要)(layerDepth)
	);


	// 純粋にスプライトフォントで文字列を描画する方法
	m_spriteFont->DrawString(m_spriteBatch/*.get()*/, L"Result Scene", Vector2(10, 40));

	wchar_t buf[32];
	swprintf_s(buf, 32, L"right : %d, bottom : %d", rect.right, rect.bottom);
	m_spriteFont->DrawString(m_spriteBatch/*.get()*/, buf, Vector2(10,70));

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
