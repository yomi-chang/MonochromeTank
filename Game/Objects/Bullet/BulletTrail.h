#pragma once

class Graphics;

// 弾の座標
struct PosBuffer
{
	DirectX::SimpleMath::Vector3 head;	//弾の先端の位置
	DirectX::SimpleMath::Vector3 tail;	//弾の末端の位置
};

class BulletTrail
{
private:
	std::vector<PosBuffer> m_posArray;						// 剣の位置を保存するバッファ
	PosBuffer tempPos;										// 現在フレームでの剣の位置
	int m_bufferSize;										// バッファサイズ(どれだけトレイルを伸ばすか)
	DirectX::SimpleMath::Vector3 m_bulletHead;				// 弾の先端座標
	DirectX::SimpleMath::Vector3 m_bulletTail;				// 弾の末端座標

	// グラフィックス
	Graphics* m_graphics;
	// 入力レイアウト
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
	//	エフェクト 
	std::unique_ptr<DirectX::BasicEffect> m_basicEffect;
	//	プリミティブバッチ 
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionTexture>> m_primitiveBatch;

public:
	BulletTrail();
	~BulletTrail() = default;
	// 初期化処理　長さを決める
	void Initialize(int bufferSize);
	void Update();											//履歴を更新して、頂点バッファを更新する
	void Render();											//描画
	void SetPosition(DirectX::SimpleMath::Vector3& head, DirectX::SimpleMath::Vector3& tail);  //現在フレームの剣の位置を保存する
};