#pragma once
#include "Model.h"
#include "Framework/Graphics.h"

// Resourcesクラスを定義する
class Resources
{
public:
	DirectX::Model* GetTankBodyModel() { return m_tankBodyModel.get(); }			// 車体モデル
	DirectX::Model* GetTankTurretModel() { return m_tankTurretModel.get(); }		// 砲塔モデル
	DirectX::Model* GetTankCannonModel() { return m_tankCanonModel.get(); }			// 砲身モデル
	DirectX::Model* GetSkySphereModel() { return m_skySphereModel.get(); }			// 天球モデル
	DirectX::Model* GetBulletModel() { return m_bulletModel.get(); }				// 弾モデル
	DirectX::Model* GetFixedTurretModel() { return m_fixedTurretModel.get(); }		// 固定砲台モデル

	ID3D11ShaderResourceView* GetTargetTexture() { return m_targetTexture.Get(); }			// 照準テクスチャ
	ID3D11ShaderResourceView* GetTargetLockTexture() { return m_targetLockTexture.Get(); }	// 照準ロックテクスチャ
	ID3D11ShaderResourceView* GetShadowTexture() { return m_shadowTexture.Get(); }			// 丸影テクスチャ
	ID3D11ShaderResourceView* GetFloorTexture() { return m_floorTexture.Get(); }			// 床テクスチャ

	ID3D11ShaderResourceView* GetBoxTexture() { return m_boxTexture.Get(); }				// 塗りつぶし用テクスチャ
	ID3D11ShaderResourceView* GetCannonBallTexture() { return m_cannonBallTexture.Get(); }	// 砲弾テクスチャ
	ID3D11ShaderResourceView* GetBulletTexture() { return m_bulletTexture.Get(); }			// 連射弾テクスチャ
	ID3D11ShaderResourceView* GetFrameTexture() { return m_frameTexture.Get(); }			// フレームテクスチャ
	ID3D11ShaderResourceView* GetRelooadTexture() { return m_reloadTexture.Get(); }			// リロードテキストテクスチャ
	ID3D11ShaderResourceView* GetTitleLogoTexture() { return m_titleLogoTexture.Get(); }	// タイトルロゴテクスチャ
	ID3D11ShaderResourceView* GetSpaceKeyTexture() { return m_spaceKeyTexture.Get(); }		// スペースキーテクスチャ

	ID3D11ShaderResourceView* GetBlackSmokeTexture() { return m_blackSmokeTexture.Get(); }	// 破壊演出テクスチャ

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
		m_fixedTurretModel{},

		m_targetTexture{},
		m_targetLockTexture{},
		m_shadowTexture{},
		m_floorTexture{},

		m_boxTexture{},
		m_bulletTexture{},
		m_cannonBallTexture{},
		m_frameTexture{},
		m_reloadTexture{},
		m_titleLogoTexture{},
		m_spaceKeyTexture{},

		m_blackSmokeTexture{}
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
	// 丸影テクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shadowTexture;
	// 床テクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_floorTexture;

	// UIテクスチャ====================================================
	// 塗りつぶし用テクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_boxTexture;
	// 砲弾テクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_bulletTexture;
	// 連射弾テクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_cannonBallTexture;
	// フレームテクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_frameTexture;
	// リロードテキストテクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_reloadTexture;
	// タイトルロゴテクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_titleLogoTexture;
	// スペースキーテクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_spaceKeyTexture;

	// エフェクトテクスチャ============================================
	// 破壊演出テクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_blackSmokeTexture;

};
