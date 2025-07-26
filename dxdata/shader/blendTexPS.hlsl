
#include "common.hlsl"


Texture2D g_Texture : register(t0);
Texture2D g_Texture1 : register(t1); //�e�N�X�`���u�����h�e�X�g
Texture2D g_BlendTex : register(t2); //�e�N�X�`���u�����h�e�X�g
Texture2D g_HeightTexture : register(t3);

SamplerState g_SamplerState : register(s0);


void main(in PS_IN In, out float4 OutDiffuse : SV_Target)
{
    float4 Color1 = g_Texture.Sample(g_SamplerState, In.TexCoord);
    float4 Color2 = g_Texture1.Sample(g_SamplerState, In.TexCoord);
    
    //�u�����h
    float4 BlendRetio = g_BlendTex.Sample(g_SamplerState, In.TexCoord);
    OutDiffuse.rgb = lerp(Color1.rgb, Color2.rgb, BlendRetio.r);
    OutDiffuse.a = 1.0f;
    
        //�p�^�[��2
     // �n�C�g�}�b�v���獂�����擾
    float4 color = g_HeightTexture.Sample(g_SamplerState, In.TexCoord);
    float height = (color.r * 0.299 + color.g * 0.587 + color.b * 0.14) / 3;
    //  // ddx, ddy��p�����@���x�N�g���̌v�Z
    float dx = ddx(height);
    float dy = ddy(height);
    //float3 Localnormal = normalize(float3(-dx, -dy, 1.0));
    
    float3 crossNormal = cross(
    float3(0, ddy(height), 1),
    float3(1, ddx(height), 0));

    //// �@���}�b�v�𐶐� (0.5�𑫂���[0, 1]�͈̔͂�)
    float3 normalMap = crossNormal * 0.5f + 0.5f;
    
    {
   float light = 0.5f + 0.5f * -dot(normalMap.xyz, Light.Direction.xyz);
 
        OutDiffuse.rgb *= Light.Diffuse.rgb * light; //���邳����Z
        OutDiffuse.a *= In.Diffuse.a;


        
    //����vector�̍쐬
        float3 eyev = CameraPosition.xyz - In.WorldPosition.xyz;
        eyev = normalize(eyev);
   
    
    //���̔���vector
        float3 refv = reflect(Light.Direction.xyz, normalMap.xyz);
        refv = normalize(refv);
    
        float specular = -dot(eyev, refv); //���ʔ��ˌv�Z
        specular = saturate(specular); //�l�������ヌ�[�g
    
        specular = pow(specular, 30);
    
        OutDiffuse.rgb += specular; //�X�y�L�����[�C�l��Diffuse�Ƃ��đ���
    }
}