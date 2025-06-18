/*
	@file	SelectScene.cpp
	@brief	セレクトシーンクラス
*/
#include "pch.h"
#include "SelectScene.h"
#include "Game/Screen.h"
#include "Libraries/MyLib/MemoryLeakDetector.h"
#include "Game/Objects/Stage/Floor.h"
#include "Game/Objects/Tank/TankBase/Tank.h"
#include "Libraries/MyLib/LockOnCamera.h"
#include "Game/Scene/Fade.h"
#include "Game/UserInterface/Button.h"
#include <cassert>

/// <summary>
/// 更新処理
/// </summary>
SelectScene::SelectScene()
	:
	m_graphics{ Graphics::GetInstance() },
	m_isChangeScene{},
	m_floor{},
	m_fade{},
	m_resources{ Resources::GetInstance() },
	m_selectAngle{},
	m_stageTexturePos{},
	m_tankCountTexturePos{},
	m_manualTex{},
	m_settingFrameTex{},
	m_stageTextTex{},
	m_countTextTex{},
	m_cursorTex{},
	m_buttons{}
{
}

/// <summary>
/// デストラクタ
/// </summary>
SelectScene::~SelectScene()
{
	// do nothing.
}

/// <summary>
/// 初期化処理
/// </summary>
void SelectScene::Initialize()
{	
	using namespace DirectX;
	using namespace DirectX::SimpleMath;

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

	// 画像の設定
	this->SetTextures();
	
	// BGMの再生
	SharedData::GetInstance()->GetSoundManager()->PlayBGM(XACT_WAVEBANK_SOUNDS_SELECTSCENE_BGM);

	// 床の生成
	m_floor = std::make_unique<Floor>(FLOOR_SIZE);
	m_floor->SetTexture(m_resources->GetFloorTexture());

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

	// ボタン作成
	this->CreateButton();

	// シーン変更フラグの初期化
	m_isChangeScene = false;

	// ゲーム設定の初期設定
	m_selectPos = SELECT_POS1;
	m_stageTexturePos = STAGE1;
	m_tankCountTexturePos = TANK_COUNT2;
}

/// <summary>
/// 更新処理
/// </summary>
/// <param name="elapsedTime">フレーム間の経過時間</param>
void SelectScene::Update(float elapsedTime)
{
	UNREFERENCED_PARAMETER(elapsedTime);

	// フェード
	m_fade->Update(elapsedTime);

	// フォローカメラを更新する
	m_camera->Update(elapsedTime);

	// フェードイン中でフェードが終了しているならシーン遷移
	if (m_fade->GetFadeType() == Fade::FADEIN &&
		m_fade->FinishFade())
	{
		// ステージ設定の設定
		this->SetStageSetting();
		// シーン遷移フラグ
		m_isChangeScene = true;
	}

	// フェードが終了していないなら早期リターン
	if (!m_fade->FinishFade()) { return; }

	// ボタンの接触及びクリック処理
	for (auto& button : m_buttons)
	{
		button->CheckOnMouseOver();
		button->CheckClickButton();
	}

	// カーソルの回転
	m_selectAngle += elapsedTime * CURSOR_SPEED;
}

/// <summary>
/// 描画処理
/// </summary>
void SelectScene::Render()
{
	using namespace DirectX::SimpleMath;

	// ビュー行列の取得
	auto view = Matrix::CreateLookAt(
		m_camera->GetEyePosition(),
		m_camera->GetTargetPosition(),
		Vector3::UnitY
	);
	Graphics::GetInstance()->SetViewMatrix(view);

	// 床の描画
	m_floor->Render();

	// UIの描画
	this->DrawUi();

	// シーン遷移用
	m_fade->Render();
}

/// <summary>
/// 終了処理
/// </summary>
void SelectScene::Finalize()
{
	// do nothing.
}

/// <summary>
/// 次のシーンIDの取得
/// </summary>
/// <returns>シーンID</returns>
IScene::SceneID SelectScene::GetNextSceneID() const
{
	// シーン変更がある場合
	if (m_isChangeScene)
	{
		return IScene::SceneID::PLAY;
	}

	// シーン変更がない場合
	return IScene::SceneID::NONE;
}

/// <summary>
/// 画像の設定
/// </summary>
void SelectScene::SetTextures()
{
	m_manualTex = m_resources->GetManualTexture();
	m_settingFrameTex = m_resources->GetSettingTexture();
	m_stageTextTex = m_resources->GetStageTextTexture();
	m_countTextTex = m_resources->GetCountTextTexture();
	m_cursorTex = m_resources->GetCursorTexture();
}

/// <summary>
/// ボタンの作成
/// </summary>
void SelectScene::CreateButton()
{
	using namespace DirectX::SimpleMath;

	// ステージ右選択ボタン
	auto stageSelectRightButton = std::make_unique<Button>();
	stageSelectRightButton->Initialize(
		m_resources->GetSelectRightTexture(),
		0.5f,
		Vector2(Screen::CENTER_X - 180, Screen::CENTER_Y - 60)
	);
	// マウス接触処理
	stageSelectRightButton->SetOnMouseOver([this] {
		m_selectPos = SELECT_POS1;
	});
	// マウスクリック処理
	stageSelectRightButton->SetOnClick([this] {
		if (m_stageTexturePos == STAGE1) { m_stageTexturePos = STAGE2; }
		else if (m_stageTexturePos == STAGE2) { m_stageTexturePos = STAGE3; }
		else if (m_stageTexturePos == STAGE3) { m_stageTexturePos = STAGE1; }
		SharedData::GetInstance()->GetSoundManager()->PlaySE(XACT_WAVEBANK_SOUNDS_CURSOR_SE);
	});

	// ステージ左選択ボタン
	auto stageSelectLeftButton = std::make_unique<Button>();
	stageSelectLeftButton->Initialize(
		m_resources->GetSelectLeftTexture(),
		0.5f,
		Vector2(Screen::CENTER_X - 460, Screen::CENTER_Y - 60)
	);
	// マウス接触処理
	stageSelectLeftButton->SetOnMouseOver([this] {
		m_selectPos = SELECT_POS1;
	});
	// マウスクリック処理
	stageSelectLeftButton->SetOnClick([this] {
		if (m_stageTexturePos == STAGE1) { m_stageTexturePos = STAGE3; }
		else if (m_stageTexturePos == STAGE2) { m_stageTexturePos = STAGE1; }
		else if (m_stageTexturePos == STAGE3) { m_stageTexturePos = STAGE2; }
		SharedData::GetInstance()->GetSoundManager()->PlaySE(XACT_WAVEBANK_SOUNDS_CURSOR_SE);
	});

	// 戦車カウント右選択ボタン
	auto tankCountRightButton = std::make_unique<Button>();
	tankCountRightButton->Initialize(
		m_resources->GetSelectRightTexture(),
		0.5f,
		Vector2(Screen::CENTER_X - 180, Screen::CENTER_Y + 85)
	);
	// マウス接触処理
	tankCountRightButton->SetOnMouseOver([this] {
		m_selectPos = SELECT_POS2;
	});
	// マウスクリック処理
	tankCountRightButton->SetOnClick([this] {
		if (m_tankCountTexturePos == TANK_COUNT1) { m_tankCountTexturePos = TANK_COUNT2; }
		else if (m_tankCountTexturePos == TANK_COUNT2) { m_tankCountTexturePos = TANK_COUNT3; }
		else if (m_tankCountTexturePos == TANK_COUNT3) { m_tankCountTexturePos = TANK_COUNT1; }
		SharedData::GetInstance()->GetSoundManager()->PlaySE(XACT_WAVEBANK_SOUNDS_CURSOR_SE);
	});

	// 戦車カウント左選択ボタン
	auto tankCountLeftButton = std::make_unique<Button>();
	tankCountLeftButton->Initialize(
		m_resources->GetSelectLeftTexture(),
		0.5f,
		Vector2(Screen::CENTER_X - 460, Screen::CENTER_Y + 85)
	);
	// マウス接触処理
	tankCountLeftButton->SetOnMouseOver([this] {
		m_selectPos = SELECT_POS2;
	});
	// マウスクリック処理
	tankCountLeftButton->SetOnClick([this] {
		if (m_tankCountTexturePos == TANK_COUNT1) { m_tankCountTexturePos = TANK_COUNT3; }
		else if (m_tankCountTexturePos == TANK_COUNT2) { m_tankCountTexturePos = TANK_COUNT1; }
		else if (m_tankCountTexturePos == TANK_COUNT3) { m_tankCountTexturePos = TANK_COUNT2; }
		SharedData::GetInstance()->GetSoundManager()->PlaySE(XACT_WAVEBANK_SOUNDS_CURSOR_SE);
	});

	// ゲーム開始ボタン
	auto startButton = std::make_unique<Button>();
	startButton->Initialize(
		m_resources->GetStartTextTexture(),
		0.4f,
		Vector2(Screen::CENTER_X - 350, Screen::CENTER_Y + 220)
	);
	// マウス接触処理
	startButton->SetOnMouseOver([this] {
		m_selectPos = SELECT_POS3;
	});
	// マウスクリック処理
	startButton->SetOnClick([this] {
		SharedData::GetInstance()->GetSoundManager()->PlaySE(XACT_WAVEBANK_SOUNDS_BUTTON_SE);
		m_fade->FadeIn();
	});

	// ボタン情報を配列の譲渡
	m_buttons.emplace_back(std::move(stageSelectRightButton));
	m_buttons.emplace_back(std::move(stageSelectLeftButton));
	m_buttons.emplace_back(std::move(tankCountRightButton));
	m_buttons.emplace_back(std::move(tankCountLeftButton));
	m_buttons.emplace_back(std::move(startButton));
}

/// <summary>
/// ステージ設定の設定
/// </summary>
void SelectScene::SetStageSetting()
{
	int tankCount = 0;
	std::string stageName;

	// 戦車の数
	if (m_tankCountTexturePos == TANK_COUNT1)	   { tankCount = 1; }
	else if (m_tankCountTexturePos == TANK_COUNT2) { tankCount = 2; }
	else if (m_tankCountTexturePos == TANK_COUNT3) { tankCount = 3; }

	// ステージ名
	if (m_stageTexturePos == STAGE1)	  { stageName = "Stage1"; }
	else if (m_stageTexturePos == STAGE2) { stageName = "Stage2"; }
	else if (m_stageTexturePos == STAGE3) { stageName = "Stage3"; }

	// データの保存
	SharedData::GetInstance()->SetSelectData(tankCount, stageName);
}

/// <summary>
/// UIの描画
/// </summary>
void SelectScene::DrawUi()
{
	using namespace DirectX;
	using namespace DirectX::SimpleMath;

	auto states = m_graphics->GetCommonStates();
	auto spriteBatch = m_graphics->GetSpriteBatch();

	// スプライトバッチの開始：オプションでソートモード、ブレンドステートを指定する
	spriteBatch->Begin(SpriteSortMode_Deferred, states->NonPremultiplied());
	// チュートリアル表示
	spriteBatch->Draw(
		m_manualTex,
		Vector2{ Screen::CENTER_X + 250,Screen::CENTER_Y },
		nullptr,
		DirectX::Colors::White,
		0.0f,
		mylib::GetTextureCenter(m_manualTex),
		0.5f
	);
	// 設定フレーム
	spriteBatch->Draw(
		m_settingFrameTex,
		Vector2{ Screen::CENTER_X - 350,Screen::CENTER_Y },
		nullptr,
		DirectX::Colors::White,
		0.0f,
		mylib::GetTextureCenter(m_settingFrameTex),
		0.5f
	);
	// ステージテキスト
	spriteBatch->Draw(
		m_stageTextTex,
		Vector2{ Screen::CENTER_X - 315,Screen::CENTER_Y - 5 },
		&m_stageTexturePos,
		DirectX::Colors::White,
		0.0f,
		mylib::GetTextureCenter(m_stageTextTex),
		0.5f
	);
	// 戦車数テキスト
	spriteBatch->Draw(
		m_countTextTex,
		Vector2{ Screen::CENTER_X - 270,Screen::CENTER_Y + 85 },
		&m_tankCountTexturePos,
		DirectX::Colors::White,
		0.0f,
		mylib::GetTextureCenter(m_countTextTex),
		0.5f
	);
	// カーソル
	spriteBatch->Draw(
		m_cursorTex,
		m_selectPos,
		nullptr,
		DirectX::Colors::White,
		m_selectAngle,
		mylib::GetTextureCenter(m_cursorTex)
	);
	// スプライトバッチの終わり
	spriteBatch->End();

	// ボタンの描画
	for (auto& button : m_buttons)
	{
		button->Render();
	}
}
