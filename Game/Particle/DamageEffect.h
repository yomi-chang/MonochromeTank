/*
	@file	DamageEffect.h
	@brief	ダメージエフェクトクラス
*/
#pragma once
#include "Framework/Graphics.h"

class DamageEffect
{
public:
	static const std::vector<D3D11_INPUT_ELEMENT_DESC> INPUT_LAYOUT;
	// コンストラクタ
	DamageEffect();
	// デストラクタ
	~DamageEffect();
	// 生成処理
	void Create();
	// 描画処理
	void Render();

public:
	// データ受け渡し用コンスタントバッファ
	struct CBuffer
	{
		DirectX::SimpleMath::Matrix		matWorld;
		DirectX::SimpleMath::Matrix		matView;
		DirectX::SimpleMath::Matrix		matProj;
		DirectX::SimpleMath::Vector4	Diffuse;
		DirectX::SimpleMath::Vector4	time;
	};

	// 時間の受け渡し
	void SetElapsedTime(float elapsedTime) { m_elapsedTime = elapsedTime; }

private:
	// グラフィックス
	Graphics* m_graphics;
	// コンスタントバッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer>	m_cBuffer;
	// 入力レイアウト
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
	//	プリミティブバッチ
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColorTexture>> m_primitiveBatch;
	// ピクセルシェーダ
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_ps;
	// 頂点シェーダ
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vs;
	// ジオメトリシェーダ
	Microsoft::WRL::ComPtr<ID3D11GeometryShader> m_gs;
	// テクスチャハンドル
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;
	// 経過時間
	float m_time;
	float m_elapsedTime;

private:
	// シェーダのロード
	void LoadShader();
	// 定数バッファの作成
	void CreateCBuffer();
};

