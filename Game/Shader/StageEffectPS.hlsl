#include "Effect.hlsli"

Texture2D tex : register(t0);
SamplerState samLinear : register(s0);

// プロトタイプ宣言
float random(float2 uv);
float2 random2(float2 st);
float perlinNoise(float2 uv);
float fBm(float2 uv);

float4 main(PS_INPUT input) : SV_TARGET
{
	//	画像表示
    float4 img = tex.Sample(samLinear, input.Tex);
    
    float n = fBm((input.Tex + float2(0.0f,-time.x * 0.5f)) * 500) - 0.15f;
    
    return lerp(float4(0, 0, 0, 0.03f), img, step(n, 0));
}

// 乱数の生成
float random(float2 uv)
{
    return frac(sin(dot(uv, float2(12.9898f, 78.233f))) * 43758.5453f);
}

// 2次元乱数の生成
float2 random2(float2 st)
{
    st = float2(dot(st, float2(127.1f, 311.7f)),
                dot(st, float2(269.5f, 183.3f)));
    return -1.0f + 2.0f * frac(sin(st) * 43758.5453f);
}

// パーリンノイズの生成
float perlinNoise(float2 uv)
{
    float2 p = floor(uv);
    float2 f = frac(uv);
    float2 u = f * f * (3.0 - 2.0 * f);
    
    float2 v00 = random2(p + float2(0, 0));
    float2 v10 = random2(p + float2(1, 0));
    float2 v01 = random2(p + float2(0, 1));
    float2 v11 = random2(p + float2(1, 1));
    
    return lerp(
        lerp(dot(v00, f - float2(0, 0)), dot(v10, f - float2(1, 0)), u.x),
        lerp(dot(v01, f - float2(0, 1)), dot(v11, f - float2(1, 1)), u.x),
        u.y) + 0.5f;
}

// FBM(非整数ブラウン運動)の生成
float fBm(float2 uv)
{
    float f = 0;
    float2 q = uv;
    f += 0.5000f * perlinNoise(q);
    q = q * 2.01f;
    f += 0.2500f * perlinNoise(q);
    q = q * 2.02f;
    f += 0.2500f * perlinNoise(q);
    return f;
}