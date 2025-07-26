

#include "common.hlsl"


Texture2D g_Texture : register(t0);
Texture2D g_Texture1 : register(t1);
SamplerState g_SamplerState : register(s0);

static float PI = 3.141592653589;

void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
    //�����x�N�g��
    float3 eyev = In.WorldPosition.xyz - CameraPosition.xyz;
    eyev = normalize(eyev);
    //�@���x�N�g����
    float3 normal = normalize(In.Normal.xyz);
    
    
    //11 / 18
    //���}�b�s���O
    //���˃x�N�g��
    float3 refvcolor;
   {
        float3 refv = reflect(eyev.xyz, normal);
        refv = normalize(refv);
    
    //���}�b�s���O�e�N�X�`�����W(�X�t�B�A�}�b�s���O�p�v�Z��)

        float2 envtexCoord;
        //envtexCoord.x = refv.x * 0.3 + 0.5;
        //envtexCoord.y = -refv.y * 0.3 + 0.5;
    
    //�p�m���}�摜�ł̊��}�b�s���O��
        envtexCoord.x = atan2(refv.x, refv.z) / (PI * 2);
        envtexCoord.y = acos(refv.y) / PI;
        refvcolor = g_Texture1.Sample(g_SamplerState, envtexCoord).rgb;
    }
     //11/25 ���� �x�N�g��
    float3 refrvColor;
    {
        float3 refrv = refract(eyev.xyz, normal, 1.0 / 1.5);
        refrv = normalize(refrv);
    
     //���}�b�s���O�e�N�X�`�����W(�X�t�B�A�}�b�s���O�p�v�Z��)
        float2 envtexCoord;
        envtexCoord.x = refrv.x * 0.3 + 0.5;
        envtexCoord.y = -refrv.y * 0.3 + 0.5;
   
    
    //���܃}�b�s���O
        refrvColor = g_Texture1.Sample(g_SamplerState, envtexCoord).rgb;
    }
    
    //�t���l���̔��˗�
    float f0 = 0.04f;
    float d = saturate(dot(-eyev, normal));
    float frensnel = f0 + (1.0 - f0) * pow((1.0 - d), 0.1);
    outDiffuse.rgb = lerp(refrvColor,refvcolor,frensnel);
    outDiffuse.a = 1.0f;
}

