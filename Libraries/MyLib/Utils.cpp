/*
	@file	Utils.cpp
	@brief	便利関数
*/
#include "pch.h"
#include "Libraries/MyLib/Utils.h"
#include <random>

namespace mylib
{
	//---------------------------------------------------------
	// クランプ関数
	//---------------------------------------------------------
	float Clamp(float value, float min, float max)
	{
		return std::max(min, std::min(value, max));
	}

	//---------------------------------------------------------
	// 乱数生成
	//---------------------------------------------------------

	float Random(float min, float max)
	{
		std::random_device seed_gen;
		std::mt19937 engine(seed_gen());

		std::normal_distribution<double> dist(min, max);

		return dist(engine);
	}

	//---------------------------------------------------------
	// テクスチャの中心座標を取得する
	//---------------------------------------------------------
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
}

//---------------------------------------------------------
// デバッグログを「出力」に表示
//---------------------------------------------------------
void mylib::DebugLog(std::string text, float value)
{
#ifdef _DEBUG
	std::string str = text + ":" + std::to_string(value) + "\n";
	LPCSTR lpcstr = str.c_str();
	OutputDebugStringA(lpcstr);
#endif
}
void mylib::DebugLog(std::string text, int value)
{
#ifdef _DEBUG
	std::string str = text + ":" + std::to_string(value) + "\n";
	LPCSTR lpcstr = str.c_str();
	OutputDebugStringA(lpcstr);
#endif
}
void mylib::DebugLog(std::string text)
{
#ifdef _DEBUG
	std::string str = text + "\n";
	LPCSTR lpcstr = str.c_str();
	OutputDebugStringA(lpcstr);
#endif
}
void mylib::DebugLog(const DirectX::SimpleMath::Vector3& position)
{
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
