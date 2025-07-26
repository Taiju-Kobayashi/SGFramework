//���_�V�F�[�_
//���ɉ������Ȃ�
//�X�v���C�g&�e�N�X�`���t��
#include "common.hlsl" //�K���C���N���[�h

void main(in VS_IN In, out PS_IN Out)
{
    Out = (PS_IN) 0;
 //���_�ϊ�����
    matrix wvp; //World View Projection�s��
    wvp = mul(World, View); //wvp = World* View;
    wvp = mul(wvp, Projection); //wvp = wvp * Projection
   
    Out.Position = mul(In.Position, wvp); //���_���W��ϊ����ďo��
    
    //���_�F���o��
    Out.Diffuse = In.Diffuse;
    
    //�e�N�X�`�����W�o��
    Out.TexCoord = In.TexCoord;
        
}