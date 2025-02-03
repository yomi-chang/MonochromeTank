#include "Effect.hlsli"

Texture2D tex : register(t0);
SamplerState samLinear : register(s0);

float4 main(PS_INPUT input) : SV_TARGET
{
	
    float4 fade = tex.Sample(samLinear, input.Tex);
    float4 output = { 0, 0, 0, 0 };
	
    float alpha = fade.r * time.x * 0.3f;
    output.a = alpha;
    return output;
};
	