#include "Ui.hlsli"

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;
    
    output.pos = float4(input.pos,1);   // 頂点位置をそのまま渡す
    output.color = input.color;         // 頂点カラーをそのまま渡す
    output.tex = input.tex;             // テクスチャ座標をそのまま渡す
    
    return output;
}