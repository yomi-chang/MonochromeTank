/*
	@file	SceneManager.h
	@brief	シーンマネージャクラス
*/
#pragma once
#include "IScene.h"

class SceneManager
{
private:
	std::unique_ptr<IScene> m_currentScene;

public:
	SceneManager();
	~SceneManager();

	void Initialize();
	void Update(float elapsedTime);
	void Render();
	void Finalize();


private:
	void ChangeScene(IScene::SceneID sceneID);
	void CreateScene(IScene::SceneID sceneID);
	void DeleteScene();
};
