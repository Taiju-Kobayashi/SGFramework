#include "common.hlsl"


void main(in VS_IN In, out PS_IN Out)
{
    Out = (PS_IN) 0;
    matrix wvp;
    wvp = mul(World, View);
    wvp = mul(wvp, Projection);
    Out.Position = mul(In.Position, wvp);

	  //���_�@���̃��[���h�ϊ�
    float4 worldNormal, Normal;
    Normal = float4(In.Normal.xyz, 0.0f); //���_�@���𕽍s�ړ����Ȃ��悤�ɂ���
    worldNormal = mul(Normal, World); //�@�������̏�ŉ�]������
    worldNormal = normalize(worldNormal);
    Out.Normal = worldNormal;
    
	
    Out.TexCoord = In.TexCoord;
    Out.Diffuse = In.Diffuse * Material.Diffuse;
	 //���[���h�ϊ��������_���W
    Out.WorldPosition = mul(In.Position, World);
 
	   //�^���W�F���g�����[���h�ϊ�
    float4 worldTangent, Tangent;
    Tangent = float4(In.Normal.xyz, 0.0f); //���_�@���𕽍s�ړ����Ȃ��悤�ɂ���
    worldTangent = mul(Tangent, World); //�@�������̏�ŉ�]������
    worldTangent = normalize(worldTangent);
    Out.Tangent = worldTangent;
    
    //�o�C�m�[�}�������߂� Tangent��Normal�̊O��
    float4 worldBinormal;
    worldBinormal.w = 0.0; //cross �O��
    worldBinormal.xyz = cross(worldTangent.xyz, worldNormal.xyz);
    worldBinormal = normalize(worldBinormal);
    Out.BiNormal = worldBinormal;
}

