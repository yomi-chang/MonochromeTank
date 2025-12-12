/**
 * @file   PauseMenu.h
 * @brief  ポーズ画面クラス
 */
#pragma once
class Graphics;
class Resources;
class Button;

class PauseMenu
{
private:
	// カーソルの回転速度
	static constexpr int CURSOR_SPEED = 2;

private:
	enum UI
	{
		TITLE,
		CANCEL,
	};

public:
	// コンストラクタ
	PauseMenu();
	// デストラクタ
	~PauseMenu();
	// 初期処理
	void Initialize();
	// 更新処理
	void Update(float elapsedTime);
	// 描画処理
	void Render();

private:
	// グラフィックス
	Graphics* m_graphics;
	// リソース
	Resources* m_resources;
	// ポーズ画面を表示するか
	bool m_isDisplay;
	// タイトルに戻るか
	bool m_returnTitle;
	// 選択されているUI
	UI m_currentSelectUi;
	// カーソルの角度
	float m_cursorAngle;
	// ボタン
	std::vector<std::unique_ptr<Button>> m_buttons;

public:
	// ポーズ中かどうか
	bool IsPause() { return m_isDisplay; }
	// タイトルに戻るかどうか
	bool IsReturnTitle() { return m_returnTitle; }

private:
	// ボタンの作成
	void CreateButtons();
	// ポーズ画面の終了
	void Cancel();
	// タイトルに戻る
	void ReturnToTitle();
};

