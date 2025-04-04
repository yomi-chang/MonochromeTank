/*
	@file	SkySphere.cpp
	@brief	天球クラス
*/
#include "pch.h"
#include "Game/Objects/Stage/StageObject/SkySphere.h"
#include "Framework/Resources.h"
#include "Framework/Graphics.h"

//-------------------------------------------------------------------
// コンストラクタ
//-------------------------------------------------------------------
SkySphere::SkySphere()
{
	// モデルの受け取り
	m_skyModel = Resources::GetInstance()->GetSkySphereModel();
}


//-------------------------------------------------------------------
// 描画処理
//-------------------------------------------------------------------
void SkySphere::Render()
{
	using namespace DirectX;
	using namespace DirectX::SimpleMath;

	// 天球の設定を行う
	m_skyModel->UpdateEffects([](DirectX::IEffect* effect)
	{
			// ベーシックエフェクトを設定する
			BasicEffect* basicEffect = dynamic_cast<BasicEffect*>(effect);
			if (basicEffect)
			{
				// 個別のライトをすべて無効化する
				basicEffect->SetLightEnabled(0, false);
				basicEffect->SetLightEnabled(1, false);
				basicEffect->SetLightEnabled(2, false);

				// モデルを自発光させる
				basicEffect->SetEmissiveColor(Colors::White);
			}
		}
	);

	// 天球の描画
	Matrix world = Matrix::Identity;
	Graphics::GetInstance()->DrawModel(m_skyModel, world);
}
