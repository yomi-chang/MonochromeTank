/*
	@file	SceneManager.h
	@brief	シーンマネージャクラス
*/
#pragma once
#include "Interface/IScene.h"

class SceneManager
{
private:
	// 現在のシーン
	std::unique_ptr<IScene> m_currentScene;

public:
	// コンストラクタ
	SceneManager();
	// デストラクタ
	~SceneManager();
	// 初期化処理
	void Initialize();
	// 更新処理
	void Update(float elapsedTime);
	// 描画処理
	void Render();
	// 終了処理
	void Finalize();


private:
	// シーンの変更
	void ChangeScene(IScene::SceneID sceneID);
	// シーンの作成
	void CreateScene(IScene::SceneID sceneID);
	// シーンの削除
	void DeleteScene();

private:
	// 勝利した戦車の情報
	
};
