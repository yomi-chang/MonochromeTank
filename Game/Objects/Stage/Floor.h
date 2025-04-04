/*
	@file	Floor.h
	@brief	床クラス
*/
#pragma once
#include <PrimitiveBatch.h>
#include <VertexTypes.h>

class Graphics;

class Floor
{
public:
	// コンストラクタ
	Floor(float size);

	// デストラクタ
	~Floor() = default;

	// 描画処理
	void Render();

	// 画像のセット
	void SetTexture(ID3D11ShaderResourceView* texture) { m_texture = texture; }

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

	// 頂点情報
	DirectX::VertexPositionTexture m_vertex[4];
};