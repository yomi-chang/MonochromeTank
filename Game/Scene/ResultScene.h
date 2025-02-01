/*
	@file	ResultScene.h
	@brief	リザルトシーンクラス
*/
#pragma once
#include "IScene.h"
#include "Framework/Graphics.h"

namespace mylib
{
	class LockOnCamera;
}

class Tank;
class Floor;
class Fade;
class SkySphere;

class ResultScene final :
    public IScene
{
private:
	// グラフィックス
	Graphics* m_graphics;

	DirectX::SpriteBatch* m_spriteBatch;;

	// テクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;

	// テクスチャの半分の大きさ
	DirectX::SimpleMath::Vector2 m_texCenter;

	// シーンチェンジフラグ
	bool m_isChangeScene;

	// 勝利した戦車
	Tank* m_tank;

	// カメラ
	std::unique_ptr<mylib::LockOnCamera> m_camera;

	// 床
	std::unique_ptr<Floor> m_floor;

	// シーン遷移
	std::unique_ptr<Fade> m_fade;

	// テクスチャの切り取り座標
	RECT m_texturePos;

	// 天球
	std::unique_ptr<SkySphere> m_skySphere;


public:
	ResultScene();
    ~ResultScene() override;

    void Initialize() override;
    void Update(float elapsedTime)override;
    void Render() override;
    void Finalize() override;

    SceneID GetNextSceneID() const;
};
