/**
 * @file   BulletTrail.h
 * @brief  弾の軌跡を表示するクラス
 */
#pragma once

class Graphics;

class BulletTrail
{
public:
	// コンストラクタ
	BulletTrail();
	// デストラクタ
	~BulletTrail();
	// 初期化処理
	void Initialize(int trailCount);
	// 描画処理 
	void Render();

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
	// 座標の受け取り
	void SetPosition(const DirectX::SimpleMath::Vector3& right, const DirectX::SimpleMath::Vector3& left);
	// 配列の削除
	void DeletePosArray() { m_posArray.clear(); }
};