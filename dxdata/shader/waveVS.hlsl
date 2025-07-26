#include "common.hlsl"

void main(in VS_IN In, out PS_IN Out)
{
    Out = (PS_IN) 0;
    matrix wvp; //�s��ϐ����쐬
    wvp = mul(World, View); //wvp = ���[���h�s�񁖃J�����s��
    wvp = mul(wvp, Projection); //wvp = wvp *�v���W�F�N�V�����s��
    Out.Position = mul(In.Position, wvp); //�ϊ����ʂ��o�͂���

    //���_�@�������[���h�s��ŕϊ�
    float4 worldNormal, normal;
    normal = float4(In.Normal.xyz, 0.0f);
    worldNormal = mul(normal, World);
    worldNormal = normalize(worldNormal);
    Out.Normal = worldNormal;
    
    Out.Diffuse = In.Diffuse;
    Out.TexCoord = In.TexCoord;

    Out.WorldPosition = mul(In.Position, World);
    
    float4 worldTangent, Tangent;
    Tangent = float4(In.Normal.xyz, 0.0f); //���_�@���𕽍s�ړ����Ȃ��悤�ɂ���
    worldTangent = mul(Tangent, World); //�@�������̏�ŉ�]������
    worldTangent = normalize(worldTangent);
    Out.Tangent = worldTangent;
    
    //�o�C�m�[�}�������߂� Tangent��Normal�̊O��
    float4 worldBinormal;
    worldBinormal = 0; //cross �O��
    worldBinormal.xyz = cross(worldTangent.xyz, worldNormal.xyz);
    worldBinormal = normalize(worldBinormal);
    Out.BiNormal = worldBinormal;
}