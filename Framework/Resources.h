#pragma once
#include "Model.h"
#include "Framework/Graphics.h"

// Resourcesクラスを定義する
class Resources
{
public:
	DirectX::Model* GetTankBodyModel() { return m_tankBodyModel.get(); }		// 車体モデル
	DirectX::Model* GetTankTurretModel() { return m_tankTurretModel.get(); }	// 砲塔モデル
	DirectX::Model* GetTankCannonModel() { return m_tankCanonModel.get(); }		// 砲身モデル
	DirectX::Model* GetSkySphereModel() { return m_skySphereModel.get(); }		// 天球モデル
	DirectX::Model* GetBulletModel() { return m_bulletModel.get(); }			// 弾モデル

	ID3D11ShaderResourceView* GetTargetTexture() { return m_targetTexture.Get(); }			// 照準テクスチャ
	ID3D11ShaderResourceView* GetTargetLockTexture() { return m_targetLockTexture.Get(); }	// 照準ロックテクスチャ
	ID3D11ShaderResourceView* GetCannonBallTexture() { return m_cannonBallTexture.Get(); }	// 砲弾テクスチャ
	ID3D11ShaderResourceView* GetBulletTexture() { return m_bulletTexture.Get(); }			// 通常弾テクスチャ
	ID3D11ShaderResourceView* GetShadowTexture() { return m_shadowTexture.Get(); }			// 丸影テクスチャ
	ID3D11ShaderResourceView* GetFloorTexture() { return m_floorTexture.Get(); }			// 床テクスチャ

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
		m_bulletModel {},

		m_targetTexture{},
		m_targetLockTexture{},
		m_bulletTexture{},
		m_cannonBallTexture{},
		m_shadowTexture{},
		m_floorTexture{}
	{
	}

private:
	// リソース
	static std::unique_ptr<Resources> m_resources;
	// グラフィックス
	Graphics* m_graphics = Graphics::GetInstance();
	// デバイス
	ID3D11Device* m_device = m_graphics->GetDeviceResources()->GetD3DDevice();

	// モデル==========================================================
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
	// 固定砲台モデル
	std::unique_ptr<DirectX::Model> m_fixedTurretModel;

	// テクスチャ======================================================
	// 照準テクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_targetTexture;
	// 照準ロックテクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_targetLockTexture;
	// 砲弾テクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_bulletTexture;
	// 通常弾テクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_cannonBallTexture;
	// 丸影テクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shadowTexture;
	// 床テクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_floorTexture;
};
