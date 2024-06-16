#pragma once
#include "Model.h"
#include "Framework/Graphics.h"

// Resourcesクラスを定義する
class Resources
{
public:
	// 車体モデルの取得
	DirectX::Model* GetTankBodyModel() { return m_tankBodyModel.get(); }
	// 砲塔モデルの取得
	DirectX::Model* GetTankTurretModel() { return m_tankTurretModel.get(); }
	// 砲身モデルの取得
	DirectX::Model* GetTankCannonModel() { return m_tankCanonModel.get(); }

	// 天球モデルの取得
	DirectX::Model* GetSkySphereModel() { return m_skySphereModel.get(); }

	// 弾モデルの取得
	DirectX::Model* GetBulletModel() { return m_bulletModel.get(); }

public:
	Resources(Resources&&) = default;
	Resources& operator= (Resources&&) = default;
	Resources& operator= (Resources const&) = delete;
	// デストラクタ
	~Resources() = default;
	// Resoucesクラスのインスタンスを取得する
	static Resources* const GetInstance();
	// リソースをロードする
	void LoadResource();

private:
	// コンストラクタ
	Resources() noexcept
		:
		m_tank{},
		m_tankBodyModel{},
		m_tankTurretModel{},
		m_tankCanonModel{},
		m_skySphereModel{},
		m_bulletModel {}
	{
	}

private:
	// リソース
	static std::unique_ptr<Resources> m_resources;
	// グラフィックス
	Graphics* m_graphics = Graphics::GetInstance();
	// デバイス
	ID3D11Device* m_device = m_graphics->GetDeviceResources()->GetD3DDevice();

	// 戦車モデル
	std::unique_ptr<DirectX::Model> m_tank;

	// 車体モデル
	std::unique_ptr<DirectX::Model> m_tankBodyModel;

	// 砲塔モデル
	std::unique_ptr<DirectX::Model> m_tankTurretModel;

	// 砲身モデル
	std::unique_ptr<DirectX::Model> m_tankCanonModel;

	// 天球モデル
	std::unique_ptr<DirectX::Model> m_skySphereModel;

	// 弾モデル
	std::unique_ptr<DirectX::Model> m_bulletModel;
};
