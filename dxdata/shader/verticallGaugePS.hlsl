#include "common.hlsl"
Texture2D g_Texture : register(t0);
SamplerState g_SamplerState : register(s0);


void main(in PS_IN In, out float4 OutDiffuse : SV_Target)
{
    float per = 1.0 - ParameterTwo.x;
    if (In.TexCoord.y > per)
    {
        OutDiffuse = g_Texture.Sample(g_SamplerState, In.TexCoord);
    }
    else
    {
        OutDiffuse = float4(0.0f, 0.0f, 0.0f, 0.0f); // “§–¾
    }
}