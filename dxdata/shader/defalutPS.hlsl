#include "common.hlsl" //�K���C���N���[�h

//C���ꑤ�Őݒ肵���T���v���[�ݒ���󂯎��
//���x�Ȑݒ�̓V�F�[�_���ł͏o���Ȃ�
SamplerState g_SamplerState : register(s0); //�e�N�X�`���T���v���[�@0��

//�e�N�X�`�������󂯎��
Texture2D g_Texture2d : register(t0); //�e�N�X�`��0��


void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
	//���̃s�N�Z���Ɏg���e�N�Z�����擾
    outDiffuse = g_Texture2d.Sample(g_SamplerState, In.TexCoord);
    outDiffuse *= In.Diffuse; //�A���t�@�l���|���Z
}
