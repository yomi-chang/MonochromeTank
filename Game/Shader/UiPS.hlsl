#include "Ui.hlsli"

// UI要素のためのテクスチャとサンプラーステート
Texture2D tex : register(t0);
SamplerState samLinear : register(s0);

float4 main(PS_INPUT input) : SV_TARGET
{
    // テクスチャ座標を使ってテクスチャをサンプル
    float4 output = tex.Sample(samLinear, input.tex);
    
    // ピクセルシェーダの出力として色を返す
    return output;
}