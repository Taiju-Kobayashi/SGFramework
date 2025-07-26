#include "common.hlsl"
Texture2D g_HeightTexture : register(t3);
SamplerState g_SamplerState : register(s0);


void main(in VS_IN In, out PS_IN Out)
{
      //法線計算 parameterのZ(高さ),W(幅)にテクスチャ画像サイズをセット
    float2 shiftX = float2(1.0f / ParameterTwo.w, 0.0f);
    float2 shiftZ = float2(0.0f, 1.0f / ParameterTwo.z);
    
    float3 colorX1 = g_HeightTexture.Sample(g_SamplerState, In.TexCoord + shiftX) * 2.0f - 1.0f;
    float3 colorX2 = g_HeightTexture.Sample(g_SamplerState, In.TexCoord - shiftX) * 2.0f - 1.0f;
    float3 colorZ1 = g_HeightTexture.Sample(g_SamplerState, In.TexCoord + shiftZ) * 2.0f - 1.0f;
    float3 colorZ2 = g_HeightTexture.Sample(g_SamplerState, In.TexCoord - shiftZ) * 2.0f - 1.0f;
    float heightX = (colorX1.r * 0.299 + colorX1.g * 0.587 + colorX1.b * 0.14) / 3;
    float heightx = (colorX2.r * 0.299 + colorX2.g * 0.587 + colorX2.b * 0.14) / 3;
    float heightZ = (colorZ1.r * 0.299 + colorZ1.g * 0.587 + colorZ1.b * 0.14) / 3;
    float heightz = (colorZ2.r * 0.299 + colorZ2.g * 0.587 + colorZ2.b * 0.14) / 3;

    float3 vx = (1.0, 0.5 * (heightX - heightx), 0);
    float3 vz = (0, 0.5 * (heightZ - heightz), 1.0);
    float4 vn;
    vn.xyz = 0.5 * normalize(cross(vz, vx)) + 0.5;
    vn.w = 0.0;
    //===========================================================

    
    //===========================================================
    matrix wvp;
    wvp = mul(World, View);
    wvp = mul(wvp, Projection);
    
    float4 worldNormal, normal;
    worldNormal = mul(In.normal, World);
    worldNormal = normalize(worldNormal);

    float light = max(-dot(Light.Direction.xyz, worldNormal.xyz) * 0.5 + 0.5, 0.0);
    light = saturate(light);

    Out.Diffuse = In.Diffuse * Material.Diffuse * light * Light.Diffuse;
    Out.Diffuse += In.Diffuse * Material.Ambient * Light.Ambient;
    Out.Diffuse += Material.Emission;
    Out.Diffuse.a = In.Diffuse.a * Material.Diffuse.a;
    
    Out.Position = mul(In.Position, wvp);
    Out.TexCoord = In.TexCoord;

}

