#include "common.hlsl"


Texture2D g_Texture : register(t0);
Texture2D g_HeightTexture : register(t1);
//RWTexture2D<float4> texture : register(u0);
SamplerState g_SamplerState : register(s0);

void main(in PS_IN In, out float4 OutDiffuse : SV_Target)
{
    float2 texPos = float2(In.TexCoord + float2(ParameterOne.x * ParameterOne.y, 0.0f));
   float4   normal = normalize(In.Normal);
    
    
//���̂�������ւ̃x�N�g�����v�Z
    float4 lv = In.WorldPosition - Light.Position;
    lv = normalize(lv);
    //float4 lightIntensity = saturate(dot(normal, lv));

    //float4 texcolor = g_Texture.Sample(g_SamplerState, texPos);
    
    //float4 color = saturate(In.Diffuse * lightIntensity);
    
    //OutDiffuse = texcolor * color;
    //OutDiffuse.a = 1.0f;

//�����v�Z�@�n�[�t�����o�[�g
    float light = 0.5f * dot(normal.xyz, lv.xyz) + 0.5f;
    OutDiffuse = g_Texture.Sample(g_SamplerState, texPos);
    OutDiffuse.rgb *= (In.Diffuse.rgb * light); //���邳����Z
    OutDiffuse.a = In.Diffuse.a; //a�l�ɖ��邳�͉e����^���Ȃ���

    OutDiffuse.rgb += Light.Ambient.rgb; //�������Z

//����vector�̍쐬
    float3 eyev = CameraPosition.xyz - In.WorldPosition.xyz;
    eyev = normalize(eyev);

    
// �t���l�����˂̌v�Z
    float fresnel = pow(1.0 - max(0.0, dot(eyev, In.Normal.xyz)), 5.0);
    fresnel = lerp(0.1, 1.0, fresnel); // �t���l�����ʂ̋��x�𒲐�
    
//�n�[�tvector�����Z
    float3 harfv = eyev.xyz + lv.xyz;
    harfv = normalize(harfv);

//�n�[�tvector�Ɩ@���̓��ς��v�Z
    float specular = dot(harfv, normal.xyz);
    specular = saturate(specular); //�l�������ヌ�[�g

    specular = pow(specular, 10);
    
    // �t���l�����˂�K�p
    specular *= fresnel;

    OutDiffuse.rgb += specular; // �X�y�L�����[���˂�Diffuse�ɒǉ�
    //OutDiffuse.a *= fresnel;
}