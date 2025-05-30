/**
 * @file   PlayerTank.h
 * @brief  自機クラス
 */
#pragma once
#include "Interface/IObject.h"
#include "Libraries/MyLib/FollowCamera.h"
#include "Game/Objects/Tank/TankBase/TankCannon.h"
#include "Game/Objects/Tank/TankBase/Tank.h"
#include "Game/UserInterface/HpGauge.h"

class Tank;
class HpGauge;
class Wall;

namespace mylib
{
	class FollowCamera;
}

// 自機クラス
class PlayerTank : public IObject
{
public:
	// コンストラクタ
	PlayerTank(
		int tankNumber,
		const DirectX::SimpleMath::Vector3& position
	);
	// デストラクタ
	~PlayerTank() override;
	// 初期化処理
	void Initialize() override;
	// 更新処理
	void Update(float elapsedTime) override;
	// 描画処理
	void Render() override;
	// 終了処理
	void Finalize() override;
	// 破壊されているか
	bool GetDead() override { return m_tank->GetDead(); }
	// 戦車の取得
	Tank* GetTank() override { return m_tank.get(); }
	// 他の戦車情報の設定
	void SetOtherTanks(std::vector<Tank*> tanks) override;

private:
	// 戦車番号
	int m_tankNumber;
	// 座標
	DirectX::SimpleMath::Vector3 m_position;
	// 回転角
	DirectX::SimpleMath::Quaternion m_angle;
	// 戦車
	std::unique_ptr<Tank> m_tank;
	// HPゲージ
	std::unique_ptr<HpGauge> m_hpGauge;
	// カメラ
	mylib::FollowCamera* m_camera;
	// 1フレーム前の体力
	int m_previousHp;
	// ダメージを受けたかどうか
	bool m_isDamage;

public:
	// 戦車番号の取得
	int GetTankNumber() { return m_tankNumber; }
	// カメラ情報の設定
	void SetCamera(mylib::FollowCamera* camera) { m_camera = camera; }
	// 座標の取得
	const DirectX::SimpleMath::Vector3& GetPosition() { return m_position; }
	// 座標の設定
	void SetPosition(const DirectX::SimpleMath::Vector3& position);
	// 回転角の取得
	const DirectX::SimpleMath::Quaternion& GetAngle() { return m_angle; }
	// 砲身の取得
	TankCannon* GetTankCannon();
	// Tankの所有権を移動する（新しいメソッド）
	std::unique_ptr<Tank> ReleaseTank() { return std::move(m_tank);}
	// ダメージを受けたかどうか
	bool GetDamage() { return m_isDamage; }
	// メッセージを取得する
	void OnMessegeAccepted(Message::MessageID messageID);
	// 照準の描画
	void DrawSight() { m_tank->GetCannon()->DrawSight(); }

private:
	// キーボードイベント
	void KeyBoardEvent(float elapsedTime);
	// 移動処理
	void Move(float elapsedTime);
	// 回転処理
	void RotateTurretCannon();
};