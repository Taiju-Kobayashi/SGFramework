/**
 * @file   titlescene.h
 * @brief  �Q�[���̃^�C�g���V�[���̃w�b�_�[
 * @author Taiju Kobayashi
 * @date   2025/01
 */
#pragma once
#include "scene.h"

 /// @brief�@�Q�[���̃^�C�g���V�[���̃N���X
class TitleScene :
    public Scene
{

    void Init()override;
    void Update()override;
private:
    bool m_flag = false;

    // �T���v�����O����
    float dt = 0.1f;
    // ��ԕϐ�
    XMVECTOR xEst = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f); // Roll, Pitch, ���p��2������0
    XMMATRIX PEst = XMMatrixIdentity() * (0.0174f * dt * dt);

    // �m�C�Y�����U�s��
    XMMATRIX Q = XMMatrixIdentity() * (0.0174f * dt * dt);  // �v���Z�X�m�C�Y
    XMMATRIX R = XMMatrixIdentity() * (dt * dt);            // �ϑ��m�C�Y

    // �ϑ��s��
    XMMATRIX H = XMMatrixIdentity();
};

