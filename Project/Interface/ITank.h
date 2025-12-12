/**
 * @file   ITank.h
 * @brief  Tankインターフェース
 */
#pragma once
#include "Framework/Graphics.h"
#include "Framework/Resources.h"
#include "Interface/IObserver.h"
#include "Libraries/MyLib/Utils.h"
#include "Game/Objects/Tank/TankBase/Tank.h"


class ITank : public IObserver
{
public:
	// デストラクタ
	virtual ~ITank() = default;
	// 初期化処理
	virtual void Initialize() = 0;
	// 更新処理
	virtual void Update(float elapsedTime) = 0;
	// 描画処理
	virtual void Render() = 0;
	// 終了処理
	virtual void Finalize() = 0;

	// 破壊されているかどうか
	virtual bool GetDead() = 0;
	// 戦車の取得
	virtual Tank* GetTank() = 0;
	// 他の戦車情報の設定
	virtual void SetOtherTanks(std::vector<Tank*> tanks) = 0;
	// 体力ゲージの描画
	virtual void DrawHpGauge() = 0;
	// 弾の描画
	virtual void DrawBullet() = 0;
};