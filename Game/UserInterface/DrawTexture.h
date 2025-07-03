/**
 * @file   DrawTexture.h
 * @brief  シーンにテクスチャを描画するクラス
 */
#pragma once
#include "Framework/Graphics.h"
#include <PrimitiveBatch.h>
#include <VertexTypes.h>

class DrawTexture
{
public:
	// コンストラクタ
	DrawTexture();
	// デストラクタ
	~DrawTexture();
	// 描画処理
	void Render(const DirectX::SimpleMath::Vector3& position);
	// 画像のセット
	void SetTexture(ID3D11ShaderResourceView* texture) { m_texture  = texture; }

private:
	// グラフィックス
	Graphics* m_graphics;
	// 入力レイアウト
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
	//	エフェクト 
	std::unique_ptr<DirectX::AlphaTestEffect> m_batchEffect;
	//	プリミティブバッチ 
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionTexture>> m_primitiveBatch;
	// テクスチャハンドル 
	ID3D11ShaderResourceView* m_texture;
};