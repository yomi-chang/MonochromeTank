/*
	@file	IScene.h
	@brief	シーンのインターフェイスクラス
*/
#pragma once
#include "Game/Other/SharedData.h"
#include "Game/Other/Sounds.h"
#include "Libraries/MyLib/SoundManager.h"

class IScene
{
public:
	// シーンID
	enum class SceneID : unsigned int
	{
		NONE,
		TITLE,
		SELECT,
		PLAY,
		RESULT
	};


public:
	virtual ~IScene() = default;

	virtual void Initialize() = 0;

	virtual void Update(float elapsedTime) = 0;

	virtual void Render() = 0;

	virtual void Finalize() = 0;

	virtual SceneID GetNextSceneID() const = 0;
};
