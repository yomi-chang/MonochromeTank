/*
	@file	SceneManager.cpp
	@brief	シーンマネージャクラス
*/
#include "pch.h"
#include "SceneManager.h"
#include "TitleScene.h"
#include "SelectScene.h"
#include "PlayScene.h"
#include "ResultScene.h"
#include "Game/Screen.h"
#include "DeviceResources.h"
#include "Libraries/MyLib/MemoryLeakDetector.h"
#include <cassert>


//---------------------------------------------------------
// コンストラクタ
//---------------------------------------------------------
SceneManager::SceneManager()
	:
	m_currentScene{}
{
}

//---------------------------------------------------------
// デストラクタ
//---------------------------------------------------------
SceneManager::~SceneManager()
{
	Finalize();
}

//---------------------------------------------------------
// 初期化処理
//---------------------------------------------------------
void SceneManager::Initialize()
{
	// 最初のシーンの設定
	ChangeScene(IScene::SceneID::TITLE);
}

//---------------------------------------------------------
// 更新処理
//---------------------------------------------------------
void SceneManager::Update(float elapsedTime)
{
	m_currentScene->Update(elapsedTime);

	// 説明用変数：次のシーン
	const IScene::SceneID nextSceneID = m_currentScene->GetNextSceneID();

	// シーンを変更しない場合
	if (nextSceneID == IScene::SceneID::NONE) return;

	// シーンを変更する場合
	ChangeScene(nextSceneID);
}

//---------------------------------------------------------
// 描画処理
//---------------------------------------------------------
void SceneManager::Render()
{
	m_currentScene->Render();
}

//---------------------------------------------------------
// 終了処理
//---------------------------------------------------------
void SceneManager::Finalize()
{
	DeleteScene();
}

//---------------------------------------------------------
// シーンの変更
//---------------------------------------------------------
void SceneManager::ChangeScene(IScene::SceneID sceneID)
{
	DeleteScene();
	CreateScene(sceneID);
}

//---------------------------------------------------------
// シーンの作成
//---------------------------------------------------------
void SceneManager::CreateScene(IScene::SceneID sceneID)
{
	assert(m_currentScene == nullptr);

	switch (sceneID)
	{
		case IScene::SceneID::TITLE:
			m_currentScene = std::make_unique<TitleScene>();
			break;
		case IScene::SceneID::SELECT:
			m_currentScene = std::make_unique<SelectScene>();
			break;
		case IScene::SceneID::PLAY:
			m_currentScene = std::make_unique<PlayScene>();
			break;
		case IScene::SceneID::RESULT:
			m_currentScene = std::make_unique<ResultScene>();
			break;
		default:
			assert(!"SceneManager::CreateScene::シーン名が存在しません！");
			// no break
	}

	assert(m_currentScene && "SceneManager::CreateScene::次のシーンが生成されませんでした！");
	m_currentScene->Initialize();
}

//---------------------------------------------------------
// シーンの削除
//---------------------------------------------------------
void SceneManager::DeleteScene()
{
	if (m_currentScene)
	{
		m_currentScene.reset();
	}
}
