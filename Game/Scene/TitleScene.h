/*
	@file	TitleScene.h
	@brief	タイトルシーンクラス
*/
#pragma once
#include "Interface/IScene.h"
#include "Framework/Graphics.h"

namespace mylib
{
	class LockOnCamera;
}

class Graphics;
class Floor;
class Tank;
class Fade;

class TitleScene final :
    public IScene
{
private:
	// グラフィックス
	Graphics* m_graphics;

	DirectX::SpriteBatch* m_spriteBatch;

	DirectX::SpriteFont* m_spriteFont;

	// テクスチャ
	ID3D11ShaderResourceView* m_titleLogo;
	ID3D11ShaderResourceView* m_pressSpace;

	// テクスチャの半分の大きさ
	DirectX::SimpleMath::Vector2 m_texCenter;

	// シーンチェンジフラグ
	bool m_isChangeScene;


	// カメラ
	std::unique_ptr<mylib::LockOnCamera> m_camera;

	// 床
	std::unique_ptr<Floor> m_floor;

	// 戦車
	std::vector<std::unique_ptr<Tank>> m_tanks;

	// シーン遷移
	std::unique_ptr<Fade> m_fade;


public:
    TitleScene();
    ~TitleScene() override;

    void Initialize() override;
    void Update(float elapsedTime)override;
    void Render() override;
    void Finalize() override;

    SceneID GetNextSceneID() const;
};
