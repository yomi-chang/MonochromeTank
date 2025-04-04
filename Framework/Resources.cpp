/*
	@file	Resources.cpp
	@brief	モデル、テクスチャ等のリソース管理クラス
*/
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
	m_tankBodyModel = DirectX::Model::CreateFromSDKMESH(m_device, L"Resources\\Models\\TankBody.sdkmesh", *m_graphics->GetFX());

	// 「砲塔」モデルをロードする
	m_tankTurretModel = DirectX::Model::CreateFromSDKMESH(m_device, L"Resources\\Models\\TankTurret.sdkmesh", *m_graphics->GetFX());

	// 「砲塔」モデルをロードする
	m_tankCanonModel = DirectX::Model::CreateFromSDKMESH(m_device, L"Resources\\Models\\TankCannon.sdkmesh", *m_graphics->GetFX());

	// 「天球」モデルをロードする
	m_skySphereModel = DirectX::Model::CreateFromCMO(m_device, L"Resources\\Models\\GraySky.cmo", *m_graphics->GetFX());

	// 「弾」モデルをロードする
	m_bulletModel = DirectX::Model::CreateFromCMO(m_device, L"Resources\\Models\\bullet.cmo", *m_graphics->GetFX());

	// 「固定砲台」モデルをロードする
	m_fixedTurretModel = DirectX::Model::CreateFromSDKMESH(m_device, L"Resources\\Models\\FixedTurret.sdkmesh", *m_graphics->GetFX());


	//	照準テクスチャのロード 
	DirectX::CreateWICTextureFromFile(
		m_graphics->GetDeviceResources()->GetD3DDevice(),
		L"Resources\\Textures\\Target.png",
		nullptr,
		m_targetTexture.GetAddressOf()
	);

	//	照準ロックテクスチャのロード 
	DirectX::CreateWICTextureFromFile(
		m_graphics->GetDeviceResources()->GetD3DDevice(),
		L"Resources\\Textures\\TargetLock.png",
		nullptr,
		m_targetLockTexture.GetAddressOf()
	);

	// 丸影テクスチャのロード
	DirectX::CreateWICTextureFromFile(
		m_graphics->GetDeviceResources()->GetD3DDevice(),
		L"Resources\\Textures\\Shadow.png",
		nullptr,
		m_shadowTexture.GetAddressOf()
	);

	// 床のロード
	DirectX::CreateWICTextureFromFile(
		m_graphics->GetDeviceResources()->GetD3DDevice(),
		L"Resources\\Textures\\floorTexture.png",
		nullptr,
		m_floorTexture.GetAddressOf()
	);



	//	塗りつぶし用テクスチャのロード 
	DirectX::CreateWICTextureFromFile(
		m_graphics->GetDeviceResources()->GetD3DDevice(),
		L"Resources\\Textures\\box_1x1.png",
		nullptr,
		m_boxTexture.GetAddressOf()
	);

	//	砲弾テクスチャのロード 
	DirectX::CreateWICTextureFromFile(
		m_graphics->GetDeviceResources()->GetD3DDevice(),
		L"Resources\\Textures\\CannonBall.png",
		nullptr,
		m_cannonBallTexture.GetAddressOf()
	);

	//	連射弾テクスチャのロード 
	DirectX::CreateWICTextureFromFile(
		m_graphics->GetDeviceResources()->GetD3DDevice(),
		L"Resources\\Textures\\Bullet.png",
		nullptr,
		m_bulletTexture.GetAddressOf()
	);

	//	フレームテクスチャのロード 
	DirectX::CreateWICTextureFromFile(
		m_graphics->GetDeviceResources()->GetD3DDevice(),
		L"Resources\\Textures\\Frame.png",
		nullptr,
		m_frameTexture.GetAddressOf()
	);

	//	リロードテキストテクスチャのロード 
	DirectX::CreateWICTextureFromFile(
		m_graphics->GetDeviceResources()->GetD3DDevice(),
		L"Resources\\Textures\\ReloadText.png",
		nullptr,
		m_reloadTexture.GetAddressOf()
	);

	//	タイトルロゴテクスチャのロード 
	DirectX::CreateWICTextureFromFile(
		m_graphics->GetDeviceResources()->GetD3DDevice(),
		L"Resources\\Textures\\TitleLogo.png",
		nullptr,
		m_titleLogoTexture.GetAddressOf()
	);

	//	スペースキーテクスチャのロード 
	DirectX::CreateWICTextureFromFile(
		m_graphics->GetDeviceResources()->GetD3DDevice(),
		L"Resources\\Textures\\space.png",
		nullptr,
		m_spaceKeyTexture.GetAddressOf()
	);

	//	フォントテクスチャのロード 
	DirectX::CreateWICTextureFromFile(
		m_graphics->GetDeviceResources()->GetD3DDevice(),
		L"Resources\\Textures\\font.png",
		nullptr,
		m_fontTexture.GetAddressOf()
	);

	//	マニュアルテクスチャのロード 
	DirectX::CreateWICTextureFromFile(
		m_graphics->GetDeviceResources()->GetD3DDevice(),
		L"Resources\\Textures\\Manual.png",
		nullptr,
		m_manualTexture.GetAddressOf()
	);

	//	設定フレームテクスチャのロード 
	DirectX::CreateWICTextureFromFile(
		m_graphics->GetDeviceResources()->GetD3DDevice(),
		L"Resources\\Textures\\Setting.png",
		nullptr,
		m_settingTexture.GetAddressOf()
	);

	//	戦車カウント用テキストテクスチャのロード 
	DirectX::CreateWICTextureFromFile(
		m_graphics->GetDeviceResources()->GetD3DDevice(),
		L"Resources\\Textures\\countText.png",
		nullptr,
		m_countTextTexture.GetAddressOf()
	);

	//	ステージ用テキストテクスチャのロード 
	DirectX::CreateWICTextureFromFile(
		m_graphics->GetDeviceResources()->GetD3DDevice(),
		L"Resources\\Textures\\StageText.png",
		nullptr,
		m_stageTextTexture.GetAddressOf()
	);

	//	設定選択テクスチャのロード 
	DirectX::CreateWICTextureFromFile(
		m_graphics->GetDeviceResources()->GetD3DDevice(),
		L"Resources\\Textures\\Gear.png",
		nullptr,
		m_selectTexture.GetAddressOf()
	);

	//	リザルトテクスチャのロード 
	DirectX::CreateWICTextureFromFile(
		m_graphics->GetDeviceResources()->GetD3DDevice(),
		L"Resources\\Textures\\ResultText.png",
		nullptr,
		m_resultTexture.GetAddressOf()
	);

	//	スキップテクスチャのロード 
	DirectX::CreateWICTextureFromFile(
		m_graphics->GetDeviceResources()->GetD3DDevice(),
		L"Resources\\Textures\\SkipUI.png",
		nullptr,
		m_skipTexture.GetAddressOf()
	);

	//	プレススペーステクスチャのロード 
	DirectX::CreateWICTextureFromFile(
		m_graphics->GetDeviceResources()->GetD3DDevice(),
		L"Resources\\Textures\\PressSpaceKey.png",
		nullptr,
		m_pressSpaceTexture.GetAddressOf()
	);

	//	プレイシーンの戦車カウントテクスチャのロード 
	DirectX::CreateWICTextureFromFile(
		m_graphics->GetDeviceResources()->GetD3DDevice(),
		L"Resources\\Textures\\TanksTexture.png",
		nullptr,
		m_tankCountTexture.GetAddressOf()
	);


	//	破壊演出テクスチャのロード 
	DirectX::CreateWICTextureFromFile(
		m_graphics->GetDeviceResources()->GetD3DDevice(),
		L"Resources\\Textures\\blackSmoke.png",
		nullptr,
		m_blackSmokeTexture.GetAddressOf()
	);

	//	ダメージ演出テクスチャのロード 
	DirectX::CreateWICTextureFromFile(
		m_graphics->GetDeviceResources()->GetD3DDevice(),
		L"Resources\\Textures\\DamageEffect.jpg",
		nullptr,
		m_damageEffectTexture.GetAddressOf()
	);
}
