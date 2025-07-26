#include "common.hlsl"
//C���ꑤ�Őݒ肵���T���v���[�ݒ���󂯎��
//���x�Ȑݒ�̓V�F�[�_���ł͏o���Ȃ�
SamplerState g_SamplerState : register(s0); //�e�N�X�`���T���v���[�@0��

//�e�N�X�`�������󂯎��
Texture2D g_Texture : register(t0); //�e�N�X�`��0��

float4 g_param : register(b7); //x metalic smooth y

float CaluculateGeometricDamping(float nh, float nv, float nl, float vh);
float CaluculateDiffuseFromFrensnel(float3 N, float3 L, float3 V);
float CaluculateCookTorranceSpecular(float3 L, float3 V, float3 N, float smooth, float metalic);
float CaluculateBeckmann(float m, float cosdata);
float CaluculateFrensnel(float n, float c);

static const float PI = 3.14152926f;

void main(in PS_IN In, out float4 OutDiffuse : SV_Target)
{
    float4 normal = normalize(In.Normal);
    
    float4 albedocolor =
    g_Texture.Sample(g_SamplerState, In.TexCoord);
    
    float3 specColor = albedocolor;
    
    float metalic = g_param.x;
    float smooth = g_param.y;
    
    float3 toEye = CameraPosition.xyz - In.WorldPosition.xyz;
    toEye = normalize(toEye);
    
    float3 lig = 0;
    for (int ligNo = 0; ligNo < 3; ligNo++)
    {
        float4 lv = In.WorldPosition - Light.Position;
        
        float diffuseFromFrensnel = CaluculateDiffuseFromFrensnel(normal.xyz, -lv.xyz, toEye);
        
        float NdotL = saturate(dot(normal.xyz, -lv.xyz));
        float3 lambertDiffuse = NdotL + Light.Diffuse.rgb / PI;
        
        float3 diffuse = albedocolor.xyz * diffuseFromFrensnel * lambertDiffuse;
        
        float3 spec = CaluculateCookTorranceSpecular(-lv.xyz, toEye, normal.xyz, smooth, metalic) * Light.Diffuse;
    
        spec *= lerp(float3(1.0f, 1.0f, 1.0f), specColor, metalic);
        lig += diffuse * (1.0f - smooth) + spec;
    }
    lig += Light.Ambient.rgb * albedocolor.rgb;
    
    OutDiffuse.rgb = lig;
    OutDiffuse.a = albedocolor.a * In.Diffuse.a;
}



//�}�C�N���t�@�Z�b�g
//�x�b�N�}�����z
float CaluculateBeckmann(float m, float t)
{
    float t2 = t * t;
    float t4 = t * t * t * t;
    float m2 = m * m;
    float D = 1.0f / (4.0f * m2 * t4);
    D *= exp((-1.0f / m2) * (1.0f - t2) / t2);
    return D;
}

//�t���l�����ˍ�
//metal ������
float CaluculateFrensnel(float metal, float hv)
{
    //Schlick�ߎ����^���l��0.0 ~ 1.0�Ƃ���
    return metal + (1 - metal) * pow(1 - hv, 5);
    
    //���ʂɌv�Z�@metal�l��0.0�ȏ�Ƃ���
    //float F;
    //float g = sqrt(metal * metal + hv * hv - 1);
    //float T1 = ((g - hv) * (g - hv)) / ((g + hv) * (g + hv));
    //float T2 = 1 + ((hv * (g + hv) - 1) * (hv * (g + hv) - 1)) /
    //((hv * (g - hv) + 1) * (hv * (g - hv) + 1));
    //F = 0.5f * T1 * T2;
    //return F;
}

//�􉽌�����
float CaluculateGeometricDamping(float nh, float nv, float nl, float vh)
{
    return min(1, min(2 * nh * nv / vh, 2 * nh * nl / vh));
}

float CaluculateDiffuseFromFrensnel(float3 N, float3 L, float3 V)
{
    float NdotL = saturate(dot(N, L));
    float NdotV = saturate(dot(N, V));

    return (NdotL * NdotV);
}
float CaluculateCookTorranceSpecular(float3 L, float3 V, float3 N, float smooth, float metalic)
{
    float3 H = normalize(L + V);
    
    float NdotH = saturate(dot(N, H));
    float VdotH = saturate(dot(V, H));
    float NdotL = saturate(dot(N, L));
    float NdotV = saturate(dot(N, V));
    
    float D = CaluculateBeckmann(metalic, NdotH);
    
    float F = CaluculateFrensnel(smooth, VdotH);
    
    float G = CaluculateGeometricDamping(NdotH, NdotV, NdotL, NdotH);
    
    float m = PI * NdotV * NdotH;

    return max(F * D * G / m, 0.0f);
}