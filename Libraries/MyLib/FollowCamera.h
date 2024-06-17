/*
	@file	FollowCamera.h
	@brief	後追いカメラクラス、ばね付き
*/
#pragma once
namespace mylib
{
	class FollowCamera
	{
	private:
		// バネレート
		static constexpr float SPRING_RATE_EYE = 0.05f;
		static constexpr float SPRING_RATE_TARGET = 0.05f;
		// 距離、高さ
		static constexpr float DISTANCE = 5.0f;
		static constexpr float HEIGHT = 3.0f;

		// カメラ座標
		DirectX::SimpleMath::Vector3 m_eye;

		// 注視点
		DirectX::SimpleMath::Vector3 m_target;

		// 追跡対象の座標のポインタ（参照先を更新不可とする）
		const DirectX::SimpleMath::Vector3* m_followUpTargetPosition;

		// 追跡対象のクォータニオンのポインタ（参照先を更新不可とする）
		const DirectX::SimpleMath::Quaternion* m_followUpTargetQuaternion;


	public:
		FollowCamera();
		~FollowCamera() = default;

		void Initialize(
			const DirectX::SimpleMath::Vector3* followUpTargetPosition,
			const DirectX::SimpleMath::Quaternion* followUpTargetQuaternion
		);
		void Update(float elapsedTime);

		// getter
		const DirectX::SimpleMath::Vector3& GetEyePosition() const { return m_eye; }
		const DirectX::SimpleMath::Vector3& GetTargetPosition() const { return m_target; }
	};
}
