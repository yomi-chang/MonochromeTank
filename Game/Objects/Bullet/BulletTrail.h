/*
	@file	BulletTrail.h
	@brief	弾の軌跡を表示するクラス
*/
#pragma once

class Graphics;

class BulletTrail
{
private:
	// 弾の座標
	struct PosBuffer
	{
		DirectX::SimpleMath::Vector3 top;		//弾の先端の位置
		DirectX::SimpleMath::Vector3 bottom;	//弾の末端の位置
	};
	std::vector<PosBuffer> m_posArray;			// 位置を保存するバッファ

	// グラフィックス
	Graphics* m_graphics;
	// 入力レイアウト
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
	//	エフェクト 
	std::unique_ptr<DirectX::BasicEffect> m_basicEffect;
	//	プリミティブバッチ 
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionTexture>> m_primitiveBatch;
	//  表示するトレイルの数
	int m_maxTrail;

public:
	BulletTrail();
	~BulletTrail() = default;
	// 初期化処理　長さを決める
	void Initialize(int trailCount);
	void Render();											//描画
	void SetPosition(DirectX::SimpleMath::Vector3 top, DirectX::SimpleMath::Vector3 bottom);
	void DeletePosBuffer() { m_posArray.clear(); }
};