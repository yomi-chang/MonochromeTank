/*
	@file	ResultScene.h
	@brief	リザルトシーンクラス
*/
#pragma once
#include "IScene.h"
#include "Framework/Graphics.h"

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

    void Initialize() override;
    void Update(float elapsedTime)override;
    void Render() override;
    void Finalize() override;

    SceneID GetNextSceneID() const;
};
