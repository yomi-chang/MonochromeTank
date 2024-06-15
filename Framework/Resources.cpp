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
	// リソースディレクトリを設定する
	m_graphics->GetFX()->SetDirectory(L"Resources\\Models");

	// 「車体」モデルをロードする
	m_tankBodyModel = DirectX::Model::CreateFromCMO(m_device, L"Resources\\Models\\TankBody.cmo", *m_graphics->GetFX());

	// 「砲塔」モデルをロードする
	m_tankTurretModel = DirectX::Model::CreateFromCMO(m_device, L"Resources\\Models\\TankTurret.cmo", *m_graphics->GetFX());

	// 「砲塔」モデルをロードする
	m_tankCanonModel = DirectX::Model::CreateFromCMO(m_device, L"Resources\\Models\\TankCanon.cmo", *m_graphics->GetFX());

	// 「天球」モデルをロードする
	m_skySphereModel = DirectX::Model::CreateFromCMO(m_device, L"Resources\\Models\\SnowSky.cmo", *m_graphics->GetFX());
}
