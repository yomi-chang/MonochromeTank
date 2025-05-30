/*
	@file	Utils.cpp
	@brief	便利関数
*/
#include "pch.h"
#include "Libraries/MyLib/Utils.h"
#include <random>

namespace mylib
{
	/// <summary>
	/// クランプ関数
	/// </summary>
	/// <param name="value">丸めたい値</param>
	/// <param name="min">下限</param>
	/// <param name="max">上限</param>
	/// <returns>結果</returns>
	float Clamp(float value, float min, float max)
	{
		return std::max(min, std::min(value, max));
	}

	/// <summary>
	/// 乱数生成
	/// </summary>
	/// <param name="min">下限</param>
	/// <param name="max">上限</param>
	/// <returns>乱数</returns>
	float Random(float min, float max)
	{
		std::random_device seed_gen;
		std::mt19937 engine(seed_gen());

		std::uniform_real_distribution<double> dist(min, max);

		return static_cast<float>(dist(engine));
	}

	/// <summary>
	/// テクスチャの中心座標の取得
	/// </summary>
	/// <param name="tex">テクスチャ</param>
	/// <returns>中心座標</returns>
	DirectX::SimpleMath::Vector2 GetTextureCenter(ID3D11ShaderResourceView* tex)
	{
		using namespace DirectX::SimpleMath;

		// 一時的な変数の宣言
		Microsoft::WRL::ComPtr<ID3D11Resource> resource{};
		Microsoft::WRL::ComPtr<ID3D11Texture2D> tex2D{};
		D3D11_TEXTURE2D_DESC desc{};
		Vector2 texSize{};

		// テクスチャをID3D11Resourceとして見る
		tex->GetResource(resource.ReleaseAndGetAddressOf());

		// ID3D11ResourceをID3D11Texture2Dとして見る
		resource.As(&tex2D);

		// テクスチャ情報を取得する
		tex2D->GetDesc(&desc);

		// テクスチャサイズを取得し、float型に変換する
		texSize.x = static_cast<float>(desc.Width);
		texSize.y = static_cast<float>(desc.Height);

		// テクスチャの中心位置を計算する
		Vector2 texCenter = texSize / 2.0f;

		// テクスチャの中心を返す
		return texCenter;
	}

	/// <summary>
	/// テクスチャのサイズ取得
	/// </summary>
	/// <param name="tex">テクスチャ</param>
	/// <param name="scale">画像倍率</param>
	/// <returns>テクスチャサイズ</returns>
	DirectX::SimpleMath::Vector2 GetTextureSize(ID3D11ShaderResourceView* tex, float scale)
	{
		using namespace DirectX::SimpleMath;

		// 一時的な変数の宣言
		Microsoft::WRL::ComPtr<ID3D11Resource> resource{};
		Microsoft::WRL::ComPtr<ID3D11Texture2D> tex2D{};
		D3D11_TEXTURE2D_DESC desc{};
		DirectX::SimpleMath::Vector2 texSize{};

		// テクスチャをID3D11Resourceとして見る
		tex->GetResource(resource.ReleaseAndGetAddressOf());

		// ID3D11ResourceをID3D11Texture2Dとして見る
		resource.As(&tex2D);

		// テクスチャ情報を取得する
		tex2D->GetDesc(&desc);

		// テクスチャサイズを取得し、float型に変換して画像倍率をかける
		texSize.x = static_cast<float>(desc.Width) * scale;
		texSize.y = static_cast<float>(desc.Height) * scale;

		// テクスチャサイズを返す
		return texSize;
	}

	/// <summary>
	/// テクスチャの矩形の取得
	/// </summary>
	/// <param name="tex">テクスチャ</param>
	/// <param name="scale">画像倍率</param>
	/// <param name="pos">座標</param>
	/// <returns>矩形</returns>
	RECT GetTextureRect(ID3D11ShaderResourceView* tex, float scale, const DirectX::SimpleMath::Vector2 pos)
	{
		// テクスチャサイズの取得
		DirectX::SimpleMath::Vector2 texSize = mylib::GetTextureSize(tex, scale);
		// テクスチャの矩形の計算
		RECT rect{
			pos.x - texSize.x / 2,
			pos.y - texSize.y / 2,
			pos.x + texSize.x / 2,
			pos.y + texSize.y / 2
		};
		return rect;
	}

	/// <summary>
	/// デバッグログを「出力」に表示
	/// </summary>
	/// <param name="text">文字列</param>
	/// <param name="value">値</param>
	void DebugLog(std::string text, float value)
	{
		UNREFERENCED_PARAMETER(value);

	#ifdef _DEBUG
		std::string str = text + ":" + std::to_string(value) + "\n";
		LPCSTR lpcstr = str.c_str();
		OutputDebugStringA(lpcstr);
	#endif
	}
	void DebugLog(std::string text, int value)
	{
		UNREFERENCED_PARAMETER(value);

	#ifdef _DEBUG
		std::string str = text + ":" + std::to_string(value) + "\n";
		LPCSTR lpcstr = str.c_str();
		OutputDebugStringA(lpcstr);
	#endif
	}
	void DebugLog(std::string text)
	{
		UNREFERENCED_PARAMETER(text);

	#ifdef _DEBUG
		std::string str = text + "\n";
		LPCSTR lpcstr = str.c_str();
		OutputDebugStringA(lpcstr);
	#endif
	}
	void DebugLog(const DirectX::SimpleMath::Vector3& position)
	{
		UNREFERENCED_PARAMETER(position);

	#ifdef _DEBUG
		DirectX::SimpleMath::Vector3 pos = position;
		std::string str = "Position:("
			+ std::to_string(position.x) + ","
			+ std::to_string(position.y) + ","
			+ std::to_string(position.z) + ")\n";
		LPCSTR lpcstr = str.c_str();
		OutputDebugStringA(lpcstr);
	#endif
	}
}
