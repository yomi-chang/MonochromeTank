/*
	@file	PlayerTank.h
	@brief	自機クラス
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
class PlayerTank : IObject
{
private:
	// 砲塔の回転制限
	const float TURRET_ANGLE_MIN = DirectX::XMConvertToRadians(-45.0f);
	const float TURRET_ANGLE_MAX = DirectX::XMConvertToRadians(45.0f);
	// 砲身の回転制限
	const float CANNON_ANGLE_MIN = DirectX::XMConvertToRadians(-7.5f);
	const float CANNON_ANGLE_MAX = DirectX::XMConvertToRadians(22.5f);

public:
	PlayerTank(
		int tankNumber,
		DirectX::SimpleMath::Vector3 position
	);

	~PlayerTank()override;

	void Initialize()override;

	void Update(float elapsedTime)override;

	void Render()override;
	void Finalize()override;

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


private:
	// キーボードイベント
	void KeyBoardEvent(float elapsedTime);
	// 移動処理
	void Move(float elapsedTime);
	// 回転処理
	void RotateTurretCannon();

public:
	// 他戦車情報受け渡し
	void SetOtherTanks(std::vector<Tank*> tanks);
	// カメラ情報の受け取り
	void SetCamera(mylib::FollowCamera* camera) { m_camera = camera; }


	// 座標の取得
	DirectX::SimpleMath::Vector3 GetPosition() { return m_position; }
	// 座標の受け取り
	void SetPosition(DirectX::SimpleMath::Vector3 position);
	// 回転角の取得
	DirectX::SimpleMath::Quaternion GetAngle() { return m_angle; }
	// 砲身の取得
	TankCannon* GetTankCannon();
	// 戦車情報の取得
	Tank* GetTank() { return m_tank.get(); }

	// Tankの所有権を移動する（新しいメソッド）
	std::unique_ptr<Tank> ReleaseTank() { return std::move(m_tank);}
	
	// 死亡しているかどうか
	bool GetDead() { return m_tank->GetDead(); }

	// ダメージを受けたかどうか
	bool GetDamage() { return m_isDamage; }

	// メッセージを取得する
	void OnMessegeAccepted(Message::MessageID messageID) {};
};