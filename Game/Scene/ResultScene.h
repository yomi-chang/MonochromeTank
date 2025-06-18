/*
	@file	ResultScene.h
	@brief	リザルトシーンクラス
*/
#pragma once
#include "Interface/IScene.h"
#include "Framework/Graphics.h"

namespace mylib
{
	class LockOnCamera;
}

class Tank;
class Floor;
class Fade;
class SkySphere;
class Button;

class ResultScene final :
    public IScene
{
private:
	static constexpr int FLOOR_SIZE = 50;
	static constexpr float TEXT_SIZE = 0.6f;

public:
	// コンストラクタ
	ResultScene();
	// デストラクタ
	~ResultScene() override;
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
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;
	ID3D11ShaderResourceView* m_pressSpace;
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
	// ボタン
	std::vector<std::unique_ptr<Button>> m_buttons;

private:
	// UIの描画
	void DrawUi();
	// ボタンの作成
	void CreateButton();
};
