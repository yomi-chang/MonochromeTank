cbuffer ConstBuffer : register(b0)
{
    float4 windowSize;
};

struct VS_INPUT
{
    float3 pos : POSITION;       // 頂点位置
    float4 color : COLOR;        // 頂点カラー
    float2 tex : TEXCOORD;       // テクスチャ座標
};

struct PS_INPUT
{
    float4 pos : SV_POSITION;     // 頂点位置
    float4 color : COLOR;         // 頂点カラー
    float2 tex : TEXCOORD;        // テクスチャ座標
};