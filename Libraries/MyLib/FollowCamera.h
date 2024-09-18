/*
	@file	FollowCamera.h
	@brief	後追いカメラクラス、ばね付き
*/
#pragma once
#include "Game/Collider/SphereCollider.h"

class PlayerTank;

namespace mylib
{
	class FollowCamera
	{
	private:
		// バネレート
		static constexpr float SPRING_RATE_EYE = 0.2f;
		static constexpr float SPRING_RATE_TARGET = 0.2f;
		// 距離、高さ
		static constexpr float DISTANCE = 4.5f;
		static constexpr float HEIGHT = 2.0f;

		// カメラ座標
		DirectX::SimpleMath::Vector3 m_eye;

		// 注視点
		DirectX::SimpleMath::Vector3 m_target;

		// 追跡対象の座標
		DirectX::SimpleMath::Vector3 m_followUpTargetPosition;

		// 追跡対象のクォータニオン
		DirectX::SimpleMath::Quaternion m_followUpTargetQuaternion;

		// 自機
		PlayerTank* m_tank;

		// コライダー
		std::unique_ptr<SphereCollider> m_collider;

		// カメラが揺れているか
		bool m_isShakeCamera;

		// カメラの揺れる速度(1秒間に何回振動するか)
		static constexpr float DEFAULT_SHAKE_SPEED = 25.0f;
		float m_shakeSpeed;

		// カメラの揺れ幅
		static constexpr float DEFAULT_SHAKE_WIDTH = 0.05f;
		float m_shakeWidth;

		// カメラの振動時間
		static constexpr float DEFAULT_SHAKE_TIME = 0.5f;
		float m_shakeTime;

		// カメラが揺れている時間
		float m_shakeCount;

	public:
		FollowCamera();
		~FollowCamera() = default;

		void Initialize(PlayerTank* tank);

		void Update(float elapsedTime);

		// getter
		const DirectX::SimpleMath::Vector3& GetEyePosition() const { return m_eye; }
		const DirectX::SimpleMath::Vector3& GetTargetPosition() const { return m_target; }

		// setter
		void SetEyePosition(const DirectX::SimpleMath::Vector3& position) { m_eye = position; }

		// コライダーの取得
		DirectX::BoundingSphere* GetBoundingSphere() { return m_collider->GetBoundingSphere(); }

		// カメラの振動開始
		void StartShakeCamera(
			float speed = DEFAULT_SHAKE_SPEED,
			float width = DEFAULT_SHAKE_WIDTH,
			float time = DEFAULT_SHAKE_TIME
		);

	private:
		// カメラの振動
		void ShakeCamera(float elapsedTime);
	};
}
