

#include "common.hlsl"


Texture2D g_Texture : register(t0);
Texture2D g_Texture1 : register(t1);
SamplerState g_SamplerState : register(s0);

static float PI = 3.141592653589;

void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
    //視線ベクトル
    float3 eyev = In.WorldPosition.xyz - CameraPosition.xyz;
    eyev = normalize(eyev);
    //法線ベクトル委
    float3 normal = normalize(In.Normal.xyz);
    
    
    //11 / 18
    //環境マッピング
    //反射ベクトル
    float3 refvcolor;
   {
        float3 refv = reflect(eyev.xyz, normal);
        refv = normalize(refv);
    
    //環境マッピングテクスチャ座標(スフィアマッピング用計算式)

        float2 envtexCoord;
        //envtexCoord.x = refv.x * 0.3 + 0.5;
        //envtexCoord.y = -refv.y * 0.3 + 0.5;
    
    //パノラマ画像での環境マッピング式
        envtexCoord.x = atan2(refv.x, refv.z) / (PI * 2);
        envtexCoord.y = acos(refv.y) / PI;
        refvcolor = g_Texture1.Sample(g_SamplerState, envtexCoord).rgb;
    }
     //11/25 屈折 ベクトル
    float3 refrvColor;
    {
        float3 refrv = refract(eyev.xyz, normal, 1.0 / 1.5);
        refrv = normalize(refrv);
    
     //環境マッピングテクスチャ座標(スフィアマッピング用計算式)
        float2 envtexCoord;
        envtexCoord.x = refrv.x * 0.3 + 0.5;
        envtexCoord.y = -refrv.y * 0.3 + 0.5;
   
    
    //屈折マッピング
        refrvColor = g_Texture1.Sample(g_SamplerState, envtexCoord).rgb;
    }
    
    //フレネルの反射率
    float f0 = 0.04f;
    float d = saturate(dot(-eyev, normal));
    float frensnel = f0 + (1.0 - f0) * pow((1.0 - d), 0.1);
    outDiffuse.rgb = lerp(refrvColor,refvcolor,frensnel);
    outDiffuse.a = 1.0f;
}

