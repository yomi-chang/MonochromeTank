#pragma once
#include "Model.h"
#include "Graphics.h"

// Resourcesクラスを定義する
class Resources
{
public:
	//// 車体モデルの取得
	//DirectX::Model* GetTankBodyModel() { return m_turretModel.get(); }
	//// 砲塔モデルの取得
	//DirectX::Model* GetTankTurretModel() { return m_turretBottomModel.get(); }
	//// 砲身モデルの取得
	//DirectX::Model* GetTankCanonModel() { return m_turretFootModel.get(); }

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
		//m_turretModel{},
		//m_turretBottomModel{},					// 砲塔下部モデル
		//m_turretFootModel{},							// 砲塔脚モデル
		//m_turretPlateModel{},						// 砲塔プレートモデル
		//m_turretMiddleModel{},					// 砲塔中部モデル
		//m_turretTopModel{},							// 砲塔上部モデル
		//m_turretFortModel{},							// 砲台モデル
		//m_turretGunModel{},							// 砲身モデル
		//m_bulletModel{}									// 砲弾モデル
	{
	}

private:
	// リソース
	static std::unique_ptr<Resources> m_resources;
	// グラフィックス
	Graphics* m_graphics = Graphics::GetInstance();
	// デバイス
	ID3D11Device* m_device = m_graphics->GetDeviceResources()->GetD3DDevice();

	//// 「砲塔」モデル
	//std::unique_ptr<DirectX::Model> m_turretModel;
	//// 「砲塔下部」モデル
	//std::unique_ptr<DirectX::Model> m_turretBottomModel;
	//// 「砲塔脚」モデル
	//std::unique_ptr<DirectX::Model> m_turretFootModel;
	//// 「砲塔プレート」
	//std::unique_ptr<DirectX::Model> m_turretPlateModel;
	//// 「砲塔中部」
	//std::unique_ptr<DirectX::Model> m_turretMiddleModel;
	//// 「砲塔上部」モデル
	//std::unique_ptr<DirectX::Model> m_turretTopModel;
	//// 「砲台」モデル
	//std::unique_ptr<DirectX::Model> m_turretFortModel;
	//// 「砲身」モデル
	//std::unique_ptr<DirectX::Model> m_turretGunModel;
	//// 「砲弾」モデル
	//std::unique_ptr<DirectX::Model> m_bulletModel;
};
