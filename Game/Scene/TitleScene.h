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
	static constexpr int FLOOR_SIZE = 20;
	static constexpr float CAMERA_DISTANCE = 1.0f;
	static constexpr float CAMERA_HEIGHT = 3.0f;
	static constexpr DirectX::SimpleMath::Vector3 CAMERA_EYE_POSITION = DirectX::SimpleMath::Vector3(0.0f, 5.0f, 5.0f);
	static constexpr RECT UI_POS = { 400,500,920,620 };

public:
	// コンストラクタ
	TitleScene();
	// デストラクタ
	~TitleScene() override;
	// 初期化処理
	void Initialize() override;
	// 更新処理
	void Update(float elapsedTime) override;
	// 描画処理
	void Render() override;
	// 終了処理
	void Finalize() override;
	// 次のシーンIDの取得
	SceneID GetNextSceneID() const;

private:
	// グラフィックス
	Graphics* m_graphics;
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
};
