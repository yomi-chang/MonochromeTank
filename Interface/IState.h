/*
	@file	IState.h
	@brief	IStateインターフェース
*/
#pragma once
#include "Game/Objects/Tank/TankBase/Tank.h"
#include "Libraries/MyLib/Utils.h"
#include "Game/Other/Parameter.h"
#include "Message/Messenger.h"

class IState
{
public:
	// 状態番号
	enum class StateID : unsigned int
	{
		NONE,
		PATROL,
		TRACKING,
		ATTACK,
		RETREAT_ATTACK,
		AVOIDWALL
	};

public:
	// デストラクタ
	virtual ~IState() = default;
	// 初期化処理
	virtual void Initialize(Tank* tank) = 0;
	// 更新処理
	virtual void Update(float elapsedTime) = 0;
	// 行動状態遷移をした際に呼び出される関数
	virtual void Enter() = 0;
	// 追跡対象の戦車の取得
	virtual Tank* GetTargetTank() = 0;
	// 追跡対象の戦車の設定
	virtual void SetTargetTank(Tank* targetTank) = 0;
	// 状態番号の取得
	virtual StateID GetStateID() = 0;
};