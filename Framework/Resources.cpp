#include "pch.h"
#include "Framework/Resources.h"

std::unique_ptr<Resources> Resources::m_resources = nullptr;

// Resourcesクラスのインスタンスを取得する
Resources* const Resources::GetInstance()
{
	if (m_resources == nullptr)
	{
		// Resourcesクラスのインスタンスを生成する
		m_resources.reset(new Resources());
	}
	// Resourcesクラスのインスタンスを返す
	return m_resources.get();
}

// リソースをロードする
void Resources::LoadResource()
{
	//// リソースディレクトリを設定する
	//m_graphics->GetFX()->SetDirectory(L"resources\\cmo");
	//// 「砲塔下部」モデルをロードする
	//m_turretBottomModel = DirectX::Model::CreateFromCMO(m_device, L"resources\\cmo\\turret_bottom3.cmo", *m_graphics->GetFX());	
	//// 「砲塔脚」モデルをロードする
	//m_turretFootModel = DirectX::Model::CreateFromCMO(m_device, L"resources\\cmo\\turret_bottom_foot.cmo", *m_graphics->GetFX());
	//// 「砲塔プレート」モデルをロードする
	//m_turretPlateModel = DirectX::Model::CreateFromCMO(m_device, L"resources\\cmo\\turret_plate.cmo", *m_graphics->GetFX());
	//// 「砲塔中部」モデルをロードする
	//m_turretMiddleModel = DirectX::Model::CreateFromCMO(m_device, L"resources\\cmo\\turret_middle.cmo", *m_graphics->GetFX());
	//// 「砲塔上部」モデルをロードする
	//m_turretTopModel = DirectX::Model::CreateFromCMO(m_device, L"resources\\cmo\\turret_top.cmo", *m_graphics->GetFX());
	//// 「砲台」モデルをロードする
	//m_turretFortModel = DirectX::Model::CreateFromCMO(m_device, L"resources\\cmo\\turret_fort.cmo", *m_graphics->GetFX());
	//// 「主砲」モデルをロードする
	//m_turretGunModel = DirectX::Model::CreateFromCMO(m_device, L"resources\\cmo\\turret_gun.cmo", *m_graphics->GetFX());
	//// 「砲弾」モデルをロードする
	//m_bulletModel = DirectX::Model::CreateFromCMO(m_device, L"resources\\cmo\\bullet.cmo", *m_graphics->GetFX());
}
