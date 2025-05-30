/**
 * @file   Button.cpp
 * @brief  ボタンクラス
 */
#include "pch.h"
#include "Button.h"
#include "Libraries/MyLib/Utils.h"
#include "Framework/Graphics.h"
#include "Framework/InputManager.h"
#include "Framework/Resources.h"
#include "Game/Screen.h"

/// <summary>
/// コンストラクタ
/// </summary>
Button::Button()
	:
	m_texture{},
	m_scale{},
	m_buttonPosition{},
	m_buttonRect{},
	m_onClick{}
{
}

/// <summary>
/// デストラクタ
/// </summary>
Button::~Button()
{
}

/// <summary>
/// 初期化処理
/// </summary>
/// <param name="tex">テクスチャ</param>
/// <param name="scale">画像倍率</param>
/// <param name="pos">座標</param>
void Button::Initialize(ID3D11ShaderResourceView* tex, float scale, const DirectX::SimpleMath::Vector2 pos)
{
	// テクスチャ情報と倍率の受け取り
	m_texture = tex;
	m_scale = scale;
	m_buttonPosition = pos;

	// ボタンの矩形の設定
	m_buttonRect = mylib::GetTextureRect(tex, scale, pos);
}

/// <summary>
/// 描画処理
/// </summary>
/// <param name="rotation">回転</param>
void Button::Render(float rotation)
{
	using namespace DirectX;
	auto spriteBatch = Graphics::GetInstance()->GetSpriteBatch();
	auto states = Graphics::GetInstance()->GetCommonStates();
 
	// 画像の描画
	spriteBatch->Begin(SpriteSortMode_Deferred, states->NonPremultiplied());
	if (this->IsMouseOverUI())
	{	
		spriteBatch->Draw(
			m_texture,
			m_buttonPosition,
			nullptr,
			Colors::Red,
			rotation,
			mylib::GetTextureCenter(m_texture),
			m_scale * 1.1f
		);
	}
	else
	{
		spriteBatch->Draw(
			m_texture,
			m_buttonPosition,
			nullptr,
			Colors::White,
			rotation,
			mylib::GetTextureCenter(m_texture),
			m_scale
		);
	}

	// 矩形の表示
	spriteBatch->Draw(
		Resources::GetInstance()->GetBoxTexture(),
		m_buttonRect,
		Colors::Red
	);

	spriteBatch->End();
}

/// <summary>
/// ボタンがクリックされたかのチェック
/// </summary>
void Button::CheckClickButton()
{
	// マウスとボタンが接触していないなら早期リターン
	if (!this->IsMouseOverUI()) { return; }

	// 左クリックされた場合
	const auto& mouseTracker = InputManager::GetInstance()->GetMouseTracker();
	if (mouseTracker->leftButton)
	{
		// クリック時の処理を行う
		if (m_onClick) { m_onClick(); }
	}
}

/// <summary>
/// マウスが接触していた場合の処理
/// </summary>
void Button::CheckOnMouseOver()
{
	// マウスとボタンが接触していないなら早期リターン
	if (!this->IsMouseOverUI()) { return; }

	// マウス接触時の処理を行う
	if (m_onMouseOver) { m_onMouseOver(); }
}

/// <summary>
/// マウスとボタンが接触しているかどうか
/// </summary>
/// <returns>接触しているか</returns>
bool Button::IsMouseOverUI()
{
	// マウスステートの取得
	const auto& mouseState = InputManager::GetInstance()->GetMouseState();
	
	// マウス座標の取得
	/*DirectX::SimpleMath::Vector2 mousePos{
		static_cast<float>(mouseState.x),
		static_cast<float>(mouseState.y)
	};*/

	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
	HRESULT hr = Graphics::GetInstance()->GetDeviceResources()->GetSwapChain()->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)backBuffer.GetAddressOf());

	D3D11_TEXTURE2D_DESC desc = {};
	backBuffer->GetDesc(&desc);

	int width = static_cast<int>(desc.Width);
	int height = static_cast<int>(desc.Height);

	// 実際の描画解像度
	int realWidth = 1920; 
	int realHeight = 1080;

	// 仮想解像度（固定）
	constexpr float virtualWidth = Screen::WIDTH;
	constexpr float virtualHeight = Screen::HEIGHT;

	// スケーリング係数
	float scaleX = virtualWidth / realWidth;
	float scaleY = virtualHeight / realHeight;

	// マウス座標を仮想解像度にスケーリング
	float scaledX = mouseState.x * scaleX;
	float scaledY = mouseState.y * scaleY;

	// マウスが矩形内にあるかどうか
	if (scaledX >= m_buttonRect.left &&
		scaledX < m_buttonRect.right &&
		scaledY >= m_buttonRect.top &&
		scaledY < m_buttonRect.bottom)
	{
		return true;
	}
	return false;
	//// マウスが矩形内にあるかどうか
	//if (mouseState.x >= m_buttonRect.left  &&
	//	mouseState.x <  m_buttonRect.right &&
	//	mouseState.y >= m_buttonRect.top   &&
	//	mouseState.y <  m_buttonRect.bottom)
	//{
	//	return true;
	//}
	//return false;
}

