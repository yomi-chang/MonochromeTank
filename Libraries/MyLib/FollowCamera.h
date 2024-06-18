/*
	@file	FollowCamera.h
	@brief	後追いカメラクラス、ばね付き
*/
#pragma once
#include "Game/Objects/Tank/Tank.h"

namespace mylib
{
	class FollowCamera
	{
	private:
		// バネレート
		static constexpr float SPRING_RATE_EYE = 0.05f;
		static constexpr float SPRING_RATE_TARGET = 0.05f;
		// 距離、高さ
		static constexpr float DISTANCE = 10.0f;
		static constexpr float HEIGHT = 2.0f;

		// カメラ座標
		DirectX::SimpleMath::Vector3 m_eye;

		// 注視点
		DirectX::SimpleMath::Vector3 m_target;

		// 追跡対象の座標
		DirectX::SimpleMath::Vector3 m_followUpTargetPosition;

		// 追跡対象のクォータニオン
		DirectX::SimpleMath::Quaternion m_followUpTargetQuaternion;

		Tank* m_tank;

	public:
		FollowCamera();
		~FollowCamera() = default;

		void Initialize(Tank* tank);

		void Update(float elapsedTime);

		// getter
		const DirectX::SimpleMath::Vector3& GetEyePosition() const { return m_eye; }
		const DirectX::SimpleMath::Vector3& GetTargetPosition() const { return m_target; }
	};
}
