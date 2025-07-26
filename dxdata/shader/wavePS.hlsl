#include "common.hlsl"


Texture2D g_Texture : register(t0);
Texture2D g_HeightTexture : register(t1);
//RWTexture2D<float4> texture : register(u0);
SamplerState g_SamplerState : register(s0);

void main(in PS_IN In, out float4 OutDiffuse : SV_Target)
{
    float2 texPos = float2(In.TexCoord + float2(ParameterOne.x * ParameterOne.y, 0.0f));
   float4   normal = normalize(In.Normal);
    
    
//物体から光源へのベクトルを計算
    float4 lv = In.WorldPosition - Light.Position;
    lv = normalize(lv);
    //float4 lightIntensity = saturate(dot(normal, lv));

    //float4 texcolor = g_Texture.Sample(g_SamplerState, texPos);
    
    //float4 color = saturate(In.Diffuse * lightIntensity);
    
    //OutDiffuse = texcolor * color;
    //OutDiffuse.a = 1.0f;

//光源計算　ハーフランバート
    float light = 0.5f * dot(normal.xyz, lv.xyz) + 0.5f;
    OutDiffuse = g_Texture.Sample(g_SamplerState, texPos);
    OutDiffuse.rgb *= (In.Diffuse.rgb * light); //明るさを乗算
    OutDiffuse.a = In.Diffuse.a; //a値に明るさは影響を与えない為

    OutDiffuse.rgb += Light.Ambient.rgb; //環境光加算

//視線vectorの作成
    float3 eyev = CameraPosition.xyz - In.WorldPosition.xyz;
    eyev = normalize(eyev);

    
// フレネル反射の計算
    float fresnel = pow(1.0 - max(0.0, dot(eyev, In.Normal.xyz)), 5.0);
    fresnel = lerp(0.1, 1.0, fresnel); // フレネル効果の強度を調整
    
//ハーフvectorを演算
    float3 harfv = eyev.xyz + lv.xyz;
    harfv = normalize(harfv);

//ハーフvectorと法線の内積を計算
    float specular = dot(harfv, normal.xyz);
    specular = saturate(specular); //値をさちゅレート

    specular = pow(specular, 10);
    
    // フレネル反射を適用
    specular *= fresnel;

    OutDiffuse.rgb += specular; // スペキュラー反射をDiffuseに追加
    //OutDiffuse.a *= fresnel;
}