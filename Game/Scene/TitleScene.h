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
class Button;

class TitleScene final :
    public IScene
{
private:
	static constexpr int FLOOR_SIZE = 20;
	static constexpr float CAMERA_DISTANCE = 1.0f;
	static constexpr float CAMERA_HEIGHT = 3.0f;
	static constexpr DirectX::SimpleMath::Vector3 CAMERA_EYE_POSITION = DirectX::SimpleMath::Vector3(0.0f, 5.0f, 5.0f);
	static constexpr float LOGO_SCALE = 1.8f;
	static constexpr float TITLE_TEXT_SCALE = 0.8f;
	static constexpr float CURSOR_SCALE = 1.0f;
	static constexpr float CURSOR_SPEED = 2.0f;

	static constexpr RECT START_UI = {};
	static constexpr RECT EXIT_UI = {};

	enum UI
	{
		START,
		EXIT
	};

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
	ID3D11ShaderResourceView* m_titleLogoTex;
	ID3D11ShaderResourceView* m_startTextTex;
	ID3D11ShaderResourceView* m_exitTextTex;
	ID3D11ShaderResourceView* m_cursorUiTex;
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
	// 選択されているもの
	UI m_currentSelectUi;
	// カーソルの角度
	float m_cursorAngle;
	// ボタン
	std::vector<std::unique_ptr<Button>> m_buttons;

public:
	// 選択されているUIの決定
	void PressSelectUi();

private:
	// 画像の設定
	void SetTextures();
	// オブジェクト生成
	void CreateObject();
	// UIの描画
	void DrawUi();
	// ボタンの作成
	void CreateButton();
};
