/*
	@file	LockOnCamera.h
	@brief	移動可能なカメラクラス
*/
#pragma once

namespace mylib
{
	class LockOnCamera
	{
	private:
		// バネレート
		static constexpr float SPRING_RATE_EYE = 0.2f;
		static constexpr float SPRING_RATE_TARGET = 0.2f;
		// 距離、高さ
		static constexpr float DISTANCE = 2.5f;
		static constexpr float HEIGHT = 1.5f;

		// カメラ座標
		DirectX::SimpleMath::Vector3 m_eye;

		// 注視点
		DirectX::SimpleMath::Vector3 m_target;

		// 追跡対象の座標
		DirectX::SimpleMath::Vector3 m_targetPosition;

		// 追跡対象のクォータニオン
		DirectX::SimpleMath::Quaternion m_followUpTargetQuaternion;

		// 対象からの距離
		float m_distance;
		float m_height;

	public:
		// コンストラクタ
		LockOnCamera();
		// デストラクタ
		~LockOnCamera();
		// 初期化処理
		void Initialize();
		// 更新処理
		void Update(float elapsedTime);

		// getter
		const DirectX::SimpleMath::Vector3& GetEyePosition() const { return m_eye; }
		const DirectX::SimpleMath::Vector3& GetTargetPosition() const { return m_target; }

		// setter
		void SetEyePosition(const DirectX::SimpleMath::Vector3& position) { m_eye = position; }
		void SetTargetPosition(const DirectX::SimpleMath::Vector3& position) { m_targetPosition = position; }
		void SetDistance(const float distance) { m_distance = distance; }
		void SetHeight(const float height) { m_height = height; }
	};
}