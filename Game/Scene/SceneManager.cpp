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


/// <summary>
/// コンストラクタ
/// </summary>
SceneManager::SceneManager()
	:
	m_currentScene{}
{
}

/// <summary>
/// デストラクタ
/// </summary>
SceneManager::~SceneManager()
{
	this->Finalize();
}

/// <summary>
/// 初期化処理
/// </summary>
void SceneManager::Initialize()
{
	// 最初のシーンの設定
	this->ChangeScene(IScene::SceneID::TITLE);
}

/// <summary>
/// 更新処理
/// </summary>
/// <param name="elapsedTime">フレーム間の経過時間</param>
void SceneManager::Update(float elapsedTime)
{
	m_currentScene->Update(elapsedTime);

	// 説明用変数：次のシーン
	const IScene::SceneID nextSceneID = m_currentScene->GetNextSceneID();

	// シーンを変更しない場合
	if (nextSceneID == IScene::SceneID::NONE) return;

	// シーンを変更する場合
	this->ChangeScene(nextSceneID);
}

/// <summary>
/// 描画処理
/// </summary>
void SceneManager::Render()
{
	m_currentScene->Render();
}

/// <summary>
/// 終了処理
/// </summary>
void SceneManager::Finalize()
{
	this->DeleteScene();
}

/// <summary>
/// シーン変更
/// </summary>
/// <param name="sceneID">シーンID</param>
void SceneManager::ChangeScene(IScene::SceneID sceneID)
{
	this->DeleteScene();
	this->CreateScene(sceneID);
}

/// <summary>
/// シーンの作成
/// </summary>
/// <param name="sceneID">シーンID</param>
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

/// <summary>
/// シーンの削除
/// </summary>
void SceneManager::DeleteScene()
{
	if (m_currentScene)
	{
		m_currentScene.reset();
	}
}
