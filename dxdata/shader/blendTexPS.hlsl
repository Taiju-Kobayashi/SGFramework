
#include "common.hlsl"


Texture2D g_Texture : register(t0);
Texture2D g_Texture1 : register(t1); //テクスチャブレンドテスト
Texture2D g_BlendTex : register(t2); //テクスチャブレンドテスト
Texture2D g_HeightTexture : register(t3);

SamplerState g_SamplerState : register(s0);


void main(in PS_IN In, out float4 OutDiffuse : SV_Target)
{
    float4 Color1 = g_Texture.Sample(g_SamplerState, In.TexCoord);
    float4 Color2 = g_Texture1.Sample(g_SamplerState, In.TexCoord);
    
    //ブレンド
    float4 BlendRetio = g_BlendTex.Sample(g_SamplerState, In.TexCoord);
    OutDiffuse.rgb = lerp(Color1.rgb, Color2.rgb, BlendRetio.r);
    OutDiffuse.a = 1.0f;
    
        //パターン2
     // ハイトマップから高さを取得
    float4 color = g_HeightTexture.Sample(g_SamplerState, In.TexCoord);
    float height = (color.r * 0.299 + color.g * 0.587 + color.b * 0.14) / 3;
    //  // ddx, ddyを用いた法線ベクトルの計算
    float dx = ddx(height);
    float dy = ddy(height);
    //float3 Localnormal = normalize(float3(-dx, -dy, 1.0));
    
    float3 crossNormal = cross(
    float3(0, ddy(height), 1),
    float3(1, ddx(height), 0));

    //// 法線マップを生成 (0.5を足して[0, 1]の範囲に)
    float3 normalMap = crossNormal * 0.5f + 0.5f;
    
    {
   float light = 0.5f + 0.5f * -dot(normalMap.xyz, Light.Direction.xyz);
 
        OutDiffuse.rgb *= Light.Diffuse.rgb * light; //明るさを乗算
        OutDiffuse.a *= In.Diffuse.a;


        
    //視線vectorの作成
        float3 eyev = CameraPosition.xyz - In.WorldPosition.xyz;
        eyev = normalize(eyev);
   
    
    //光の反射vector
        float3 refv = reflect(Light.Direction.xyz, normalMap.xyz);
        refv = normalize(refv);
    
        float specular = -dot(eyev, refv); //鏡面反射計算
        specular = saturate(specular); //値をさちゅレート
    
        specular = pow(specular, 30);
    
        OutDiffuse.rgb += specular; //スペキュラーイ値をDiffuseとして足す
    }
}