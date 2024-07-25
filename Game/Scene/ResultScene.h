/*
	@file	ResultScene.h
	@brief	リザルトシーンクラス
*/
#pragma once
#include "IScene.h"
#include "Framework/Graphics.h"

// 前方宣言
class CommonResources;

namespace mylib
{
	class DebugCamera;
	class GridFloor;
}


class ResultScene final :
    public IScene
{
private:
	// グラフィックス
	Graphics* m_graphics;

	// 共通リソース
	CommonResources* m_commonResources;

	// スプライトバッチ
	//std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;

	DirectX::SpriteBatch* m_spriteBatch;

	// スプライトフォント
	//std::unique_ptr<DirectX::SpriteFont> m_spriteFont;

	DirectX::SpriteFont* m_spriteFont;

	// テクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;

	// テクスチャの半分の大きさ
	DirectX::SimpleMath::Vector2 m_texCenter;

	// シーンチェンジフラグ
	bool m_isChangeScene;


public:
	ResultScene();
    ~ResultScene() override;

    void Initialize(CommonResources* resources) override;
    void Update(float elapsedTime)override;
    void Render() override;
    void Finalize() override;

    SceneID GetNextSceneID() const;
};
