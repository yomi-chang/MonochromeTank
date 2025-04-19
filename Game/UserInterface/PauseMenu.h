/*
	@file	PauseMenu.h
	@brief	ポーズ画面クラス
*/
#pragma once
class Graphics;
class Resources;

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
	// 選択されているUI
	UI m_currentSelectUi;
	// カーソルの角度
	float m_cursorAngle;

public:
	// ポーズ中かどうか
	bool IsPause() { return m_isDisplay; }

private:
	// ポーズ画面の終了
	void Cancel();
	// タイトルに戻る
	void ReturnToTitle();
};

