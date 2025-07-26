#include "main.h"
#include "manager.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "scene.h"

void Camera::Init() {
	auto trf = GetTransform();
	m_target = XMFLOAT3(0, 0, 0);
	trf->SetPosition(XMFLOAT3(0, -1.5f, -3.0f)); // �����J�����ʒu���������炷

	// �J�����̉�]�N�H�[�^�j�I���̏�����
   // ������Ԃł͉�]�Ȃ��i�P�ʃN�H�[�^�j�I���j
	XMStoreFloat4(&m_cameraRotationQuaternion, XMQuaternionIdentity());
	m_cameraDistance = 25.0f; // ��ʑ̂���̏�������
}
void Camera::UnInit() {
}

void Camera::Update() {


    // --- ��]�N�H�[�^�j�I���̍X�V ---
    XMVECTOR currentQuaternion = XMLoadFloat4(&m_cameraRotationQuaternion);

#ifdef _DEBUG
    // --- ���[�U�[���͂ɂ��J������]�E�Y�[������ (��: �}�E�X����) ---
    float rotationSpeed = 0.005f; // �}�E�X���x�����p
    float zoomSpeed = 0.1f;       // �Y�[�����x�̒���



    // �f�o�b�O�p
    float mouseDeltaX = 0.0f;
    float mouseDeltaY = 0.0f;
    float mouseWheelDelta = 0.0f;

    if (Input::GetKeyPress('K')) { // �E��]
        mouseDeltaX = 5.0f;
    }
    if (Input::GetKeyPress('H')) { // ����]
        mouseDeltaX = -5.0f;
    }
    if (Input::GetKeyPress('U')) { // ���]
        mouseDeltaY = -5.0f; // DirectX�ł�Y������������A�����������Ȃ̂ŁA��������ɂ̓}�C�i�X
    }
    if (Input::GetKeyPress('J')) { // ����]
        mouseDeltaY = 5.0f;
    }

    if (Input::GetKeyPress('I')) { // �Y�[���C��
        mouseWheelDelta += 1.0f;
    }
    if (Input::GetKeyPress('O')) { // �Y�[���A�E�g
        mouseWheelDelta += -1.0f;
    }



    // Y���i���[���h�̏�����j����̉�] (���[)
    // �}�E�X��X�ړ��ʂɑΉ�
    XMVECTOR yawQuaternion = XMQuaternionRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), mouseDeltaX * rotationSpeed);

    // X���i�J�����̃��[�J��X���j����̉�] (�s�b�`)
    // �}�E�X��Y�ړ��ʂɑΉ�
    // �������d�v: �s�b�`��]�́A���[���hX���ł͂Ȃ��A���݂̃J�����́u�E�v�����i���[�J��X���j����ɍs���B
    // ���̂��߂ɂ́A���݂̃N�H�[�^�j�I������]�s��ɕϊ����A����X���x�N�g�����擾���邩�A
    // �܂��� Yaw ��K�p������̃��[�J��X������]���Ƃ��܂��B
    // ��ʓI�ȃ^���u���J�����ł́A��Ƀ��[��]�ŃJ�����̍��E���߁A���̃��[�J��X���Ńs�b�`��������̂����R�ł��B

    // ����̓V���v���ɁA���[���hX������̉�]���������Ă݂܂��B
    // �W���o�����b�N�̃��X�N�͌���܂����A�}�E�XY�ړ��ɑ΂��锽���������قȂ�ꍇ������܂��B
    XMVECTOR pitchQuaternion = XMQuaternionRotationAxis(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), mouseDeltaY * rotationSpeed);


    // ��]�N�H�[�^�j�I���̍���
    // �V������]�������̃N�H�[�^�j�I���ɓK�p���܂��B
    // XMQuaternionMultiply(Q1, Q2) �� Q1 * Q2 �ŁAQ2����ɓK�p�����
    currentQuaternion = XMQuaternionMultiply(pitchQuaternion, currentQuaternion); // �s�b�`���ɓK�p
    currentQuaternion = XMQuaternionMultiply(currentQuaternion, yawQuaternion);   // ���[����ɓK�p

    // �N�H�[�^�j�I���̐��K�� (���������_�덷�̒~�ς�h������)
    currentQuaternion = XMQuaternionNormalize(currentQuaternion);
    XMStoreFloat4(&m_cameraRotationQuaternion, currentQuaternion);

    // --- �Y�[���C��/�A�E�g (�J���������̕ύX) ---
    m_cameraDistance -= mouseWheelDelta * zoomSpeed;
#endif // !DEBUG

    m_cameraDistance = XMMin(m_cameraDistance, m_distance_range.y); // �ő勗��
    m_cameraDistance = XMMax(m_cameraDistance, m_distance_range.x);   // �ŏ�����

    // --- �J�����̈ʒu�̌v�Z ---
    XMVECTOR lookAt = XMLoadFloat3(&m_target);

    // �����J�����ʒu (��ʑ̂���Z����������g_cameraDistance���ꂽ�ʒu)
    XMVECTOR initialCameraPosRelative = XMVectorSet(0.0f, 0.0f, -m_cameraDistance, 1.0f);

    // �N�H�[�^�j�I�����g���āA�������Έʒu����]
    // XMVector3Rotate(V, Q) �̓x�N�g�� V ���N�H�[�^�j�I�� Q �ŉ�]������
    XMVECTOR rotatedCameraPosRelative = XMVector3Rotate(initialCameraPosRelative, currentQuaternion);

    // ��ʑ̂̈ʒu���l�����āA���ۂ̃��[���h���W�ɂ�����J�����ʒu������
    XMVECTOR cameraPos = rotatedCameraPosRelative + lookAt;

    // GetTransform() �Ɉʒu��ݒ�
    GetTransform()->SetPosition(XMFLOAT3(XMVectorGetX(cameraPos), XMVectorGetY(cameraPos), XMVectorGetZ(cameraPos)));
}

void Camera::Draw() {
	
     // �r���[�}�g���N�X�ݒ�
    XMVECTOR cameraPos = XMLoadFloat3(&GetTransform()->GetPosition());
    XMVECTOR lookAt = XMLoadFloat3(&m_target);
    XMVECTOR currentQuaternion = XMLoadFloat4(&m_cameraRotationQuaternion);

    // �J�����̏�����x�N�g�����N�H�[�^�j�I���ŉ�]�����ċ��߂�
    // ���[���hY������� (0, 1, 0) �������̏�����x�N�g���Ƃ��A
    // ���݂̃J�����̉�]�N�H�[�^�j�I���ŉ�]�����܂��B
    XMVECTOR initialUpVector = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    XMVECTOR up = XMVector3Rotate(initialUpVector, currentQuaternion);


    XMMATRIX viewMatrix = XMMatrixLookAtLH(cameraPos, lookAt, up);
    XMStoreFloat4x4(&m_viewmatrix, viewMatrix);

    // �v���W�F�N�V�����}�g���N�X�ݒ� (�ύX�Ȃ�)
    XMMATRIX projectionMatrix;
    projectionMatrix = XMMatrixPerspectiveFovLH(1.0f, (float)SCREEN_WIDTH / SCREEN_HEIGHT, 1.0f, 1000.0f);

    Renderer::GetRenderer().SetWorldViewProjectionData3D(viewMatrix, projectionMatrix);
    Renderer::GetRenderer().SetViewMatrix(viewMatrix);
    Renderer::GetRenderer().SetProjectionMatrix(projectionMatrix);
    Renderer::GetRenderer().SetCameraPosition(GetTransform()->GetPosition());
}


void Camera::SetPosition(XMFLOAT3 camPos, XMFLOAT3 Target) {
    // XMVector�ɕϊ�
    XMVECTOR vec1 = XMLoadFloat3(&camPos);
    XMVECTOR vec2 = XMLoadFloat3(&Target);
    XMVECTOR diff = XMVectorSubtract(vec1, vec2);
    XMVECTOR lengthSq = XMVector3LengthSq(diff); // �����̓����v�Z
    // ���ʂ��擾
    float distance;
    XMStoreFloat(&distance, lengthSq);
    if (distance < 0.1f) {
		camPos.z -= 0.1f; // �������߂�����ꍇ�́A�J�����ʒu���������炷
    }
	GetTransform()->SetPosition(camPos);
	m_target = Target;
}

void Camera::SetDistance(float distance) {
    m_cameraDistance = distance;
    // �����̃N�����v
    m_cameraDistance = XMMin(m_cameraDistance, m_distance_range.y);
    m_cameraDistance = XMMax(m_cameraDistance, m_distance_range.x);
}

void Camera::AddDistance(float deltaDistance) {
    m_cameraDistance += deltaDistance;
    // �����̃N�����v
    m_cameraDistance = XMMin(m_cameraDistance, m_distance_range.y);
    m_cameraDistance = XMMax(m_cameraDistance, m_distance_range.x);
}

// �I�C���[�p�̕ω��ʂ��N�H�[�^�j�I���ɕϊ����ēK�p����֗��Ȋ֐�
void Camera::ApplyRotationDelta(float deltaPitch, float deltaYaw, float deltaRoll) {
    XMVECTOR currentQ = XMLoadFloat4(&m_cameraRotationQuaternion);

    // �V������]�N�H�[�^�j�I���𐶐�
    XMVECTOR deltaQ = XMQuaternionRotationRollPitchYaw(deltaPitch, deltaYaw, deltaRoll);

    // ���݂̉�]�ɐV������]������
    // deltaQ * currentQ �̏����ŏ�Z�ideltaQ��currentQ�̃��[�J����ԂœK�p�����j
    currentQ = XMQuaternionMultiply(deltaQ, currentQ);

    // ���K��
    currentQ = XMQuaternionNormalize(currentQ);
    XMStoreFloat4(&m_cameraRotationQuaternion, currentQ);
}

void Camera::SetRotationDelta(float deltaPitch, float deltaYaw, float deltaRoll) {
    // �V������]�N�H�[�^�j�I���𐶐�
    XMVECTOR deltaQ = XMQuaternionRotationRollPitchYaw(deltaPitch, deltaYaw, deltaRoll);

    // ���K��
    deltaQ = XMQuaternionNormalize(deltaQ);
    XMStoreFloat4(&m_cameraRotationQuaternion, deltaQ);
}

// ���ڃN�H�[�^�j�I����ǉ�����֐�
void Camera::ApplyRotationQuaternion(const XMFLOAT4& deltaQuaternion) {
    XMVECTOR currentQ = XMLoadFloat4(&m_cameraRotationQuaternion);  
    XMVECTOR deltaQ = XMLoadFloat4(&deltaQuaternion);

    // deltaQ * currentQ �̏����ŏ�Z
    currentQ = XMQuaternionMultiply(deltaQ, currentQ);

    // ���K��
    currentQ = XMQuaternionNormalize(currentQ);
    XMStoreFloat4(&m_cameraRotationQuaternion, currentQ);
}