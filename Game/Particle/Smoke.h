#pragma once

class Graphics;

class Smoke
{
public:
	Smoke();
	~Smoke() = default;

	void Initialize();

	void Update(float elapsedTime);

	void Render(DirectX::SimpleMath::Vector3 position);

private:
	// 煙画像
	ID3D11ShaderResourceView* m_smokeTexture;

	// エフェクトの再生中かどうか
	bool m_isPlay;
	// 不透明度
	float m_alpha;
	// グラフィックス
	Graphics* m_graphics;
	// 入力レイアウト
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
	// エフェクト
	std::unique_ptr<DirectX::BasicEffect> m_basicEffect;
	// プリミティブバッチ
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionTexture>> m_primitiveBatch;

public:
	// 再生が終了したかの取得
	float GetFinishEffect() { return m_isPlay; }
};