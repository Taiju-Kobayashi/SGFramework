#include "main.h"
#include "manager.h"
#include "renderer.h"
#include "modelRenderer.h"
#include "audio.h"
#include "scene.h"
#include "terrain.h"
#include "camera.h"
#include "input.h"
#include "stone.h"
#include "splasheEmitter.h"
#include "water.h"
#include "shadowvolume.h"
#include "xmextended.h"
#include "resultInstance.h"
#include "resultscene.h"
#include "animationModel.h"

namespace {
    constexpr float GRAVITY = -9.8f;
    constexpr float FRICTION = 0.5f;
    constexpr float ROTATION_IMPULSE_STRENGTH = 0.002f;
    constexpr float COLLISION_SPEED_THRESHOLD = 10.0f;
}

void Stone::Init() {
	m_modelrenderer = new AnimationModel(this);
	ResultInstance* instance = Manager::GetManager().GetGameInstance<ResultInstance>();

	if (!instance->m_choicestone_path)
		static_cast<AnimationModel*>(m_modelrenderer)->Load_NoBone("asset\\model\\stone.fbx");
	else
		static_cast<AnimationModel*>(m_modelrenderer)->Load_NoBone(instance->m_choicestone_path);

	m_size = static_cast<AnimationModel*>(m_modelrenderer)->GetHarfModelSize();

	m_se = new Audio(this);
	m_se->Load("asset\\sound\\pisya.wav");
	m_size = ((AnimationModel*)m_modelrenderer)->GetHarfModelSize();


	Renderer::GetRenderer().CreateVertexShader(&m_vertex_shader, &m_VertexLayout,
		"shader\\vertexLightingVS.cso");
	Renderer::GetRenderer().CreatePixelShader(&m_pixel_shader,
		"shader\\vertexLightingPS.cso");
	m_transform->SetScale(XMFLOAT3(0.5f, 0.5f, 0.5f));
	m_transform->SetPosition(XMFLOAT3(0.0f, 18.0f, -50.0f));
	m_size.x *= m_transform->GetScale().x;
	m_size.y *= m_transform->GetScale().y;
	m_size.z *= m_transform->GetScale().z;

	m_old_position = m_transform->GetPosition();
	m_start_position = m_transform->GetPosition();
	m_transform->SetRotation(XMFLOAT3(XMConvertToRadians(-20.0f), 0.0f, 0.0f));
	_field = GetParentScene().GetGameObject<Water>();
	m_shadow = GetParentScene().GetGameObject<ShadowVolume>();

	XMFLOAT3 shpos = m_old_position;
	shpos.y = _field->GetHeight(m_old_position);
	if (m_shadow) {
		m_shadow->GetTransform()->SetPosition(shpos);
	}
}

void Stone::UnInit() {
    if (m_modelrenderer) {
        m_modelrenderer->UnInit();
        delete m_modelrenderer;
        m_modelrenderer = nullptr;
    }
    if (m_VertexLayout) {
        m_VertexLayout->Release();
        m_VertexLayout = nullptr;
    }
    if (m_vertex_shader) {
        m_vertex_shader->Release();
        m_vertex_shader = nullptr;
    }
    if (m_pixel_shader) {
        m_pixel_shader->Release();
        m_pixel_shader = nullptr;
    }
}

//�x�N�g���̒������v�Z����
static float get_vector_length(XMFLOAT3 v) {
	return pow((v.x * v.x) + (v.y * v.y) + (v.z * v.z), 0.5);
}


static float IsParallel(const XMFLOAT3& vector1, const XMFLOAT3& vector2) {
	float dotx = (vector1.x * vector2.x + vector1.y * vector2.y + vector1.z * vector2.z);
	float cos = dotx / (get_vector_length(vector1) * get_vector_length(vector2));
	return cos;//XMConvertToDegrees(acosf(cos)); // ������臒l�ŕ��s����
}

void Stone::Update() {

	if (!m_release)return;

	float dt = Manager::GetManager().GetDeltaTime();
	Scene& scene = Manager::GetManager().GetNowScene();

	XMFLOAT3 position = m_transform->GetPosition();


	//=====�����������
		//���C
	m_velocity.x -= (m_velocity.x * FRICTION) * dt;
	m_velocity.z -= (m_velocity.z * FRICTION) * dt;

	m_rotation_speed.x -= (m_rotation_speed.x * FRICTION) * dt;
	m_rotation_speed.y -= (m_rotation_speed.y * FRICTION) * dt;
	m_rotation_speed.z -= (m_rotation_speed.z * FRICTION) * dt;

	//�d�͉����x
	m_velocity.y += GRAVITY * dt;

	//�ړ�����
	XMFLOAT3 mscale = m_transform->GetScale();
	XMFLOAT3 oldposition = position;
	XMFLOAT3 terrpos = position; // terrpos��position�̃R�s�[�Ƃ��ď�����

	// ��]�����i���ɑ��݂����]���x��K�p�j
	XMFLOAT3 rot = m_transform->GetRotation();
	// m_rotation_speed���I�C���[�p�̍����ł���Ɖ���
	rot.x += m_rotation_speed.x * dt;
	rot.y += m_rotation_speed.y * dt;
	rot.z += m_rotation_speed.z * dt;
	m_transform->SetRotation(rot); // ��]���x�ɂ���]��K�p
	XMFLOAT3 fnormal;

	// --- ��������V�����@���v�Z���W�b�N�̓K�p ---
	XMFLOAT3 calculatedFnormal; // �V�����@���v�Z���ʂ��i�[����ϐ�

	// 3�_�̃I�t�Z�b�g���`
	XMFLOAT3 offset_center = XMFLOAT3(0.0f, 0.0f, m_size.z); // OBB�̒��S
	XMFLOAT3 offset_bow = XMFLOAT3(m_size.x, 0.0f, 0.0f);   // ���������i+x�����j
	XMFLOAT3 offset_starboard = XMFLOAT3(-m_size.x, 0.0f, 0.0f); // �E�������i-X�����j

	// 3�_���畽�ʂ̖@�����v�Z
	fnormal = _field->CalculateWaveNormalFrom3Points(
		offset_center, offset_bow, offset_starboard,position);
	
	// --- �V�����@���v�Z���W�b�N�����܂� ---

	// ������terrpos�͔g�̍����Ɩ@�����擾���邽�߂̎Q�Ɠ_�Ƃ��Ďg�p
	// _field->GetHeightAndNormal��position�Ɋ�Â���fnormal���X�V���Ă���邱�Ƃ�����
	// fnormal�́A�I�u�W�F�N�g�̏d�S�ʒu�ɂ�����g�̖@���Ƃ��ė��p����܂��B
	// ��͂̂悤�ɑ傫���ꍇ�́A�d�S�ʒu�����łȂ��A�ڒn���Ă���͈͂̕��ϓI�Ȗ@�����v�Z��������ǂ��ł��B
	// �����GetHeightAndNormal��position�̓_�ɂ�����Ǐ��I�Ȗ@���Ȃ̂ŁA�傫�ȃI�u�W�F�N�g�ɂ͖�肪�����܂��B
	// �����ł́A��Ufnormal���I�u�W�F�N�g�S�̂ɂƂ��đÓ��Ȕg�̖@���ł���Ɖ��肵�Đi�߂܂��B
	terrpos.y = _field->GetHeight(position); // terrpos.y���g�̍����ɂȂ�Afnormal���g�̖@���ɂȂ�

	if (m_shadow) {
		if (position.y < terrpos.y) {
			m_shadow->GetTransform()->SetPosition(position); // �V���h�E�̈ʒu�����݂̃I�u�W�F�N�g�ʒu�ɍ��킹��
		}
		else {
			m_shadow->GetTransform()->SetPosition(position); // ��Lif/else�u���b�N�͓��������Ȃ̂Ŋȗ����\
		}
	}

	if (m_shadow) {
		m_shadow->GetTransform()->SetPosition(position);
	}

	// �Փ˔���͑��x�������ꍇ�̂ݍs��
	if (XMFloat3Magnitude(m_velocity) > COLLISION_SPEED_THRESHOLD) {
		// OBB�Ɣg�i�܂��̓e���C���j�Ƃ̏Փ˔���
		// direction��terrpos�i�g�̕\�ʂ̍����j��position�i�I�u�W�F�N�g��Y�ʒu�j�̍�����v�Z�����
		XMFLOAT3 direction = XMFloat3Subtract(terrpos, position);

		float length = get_vector_length(direction); // �I�u�W�F�N�g�̒��S����g�̕\�ʂ܂ł̋���

		// OBB�̎��擾
		XMFLOAT3 axisX = m_transform->GetRight();
		XMFLOAT3 axisY = m_transform->GetUp();
		XMFLOAT3 axisZ = m_transform->GetForward();

		// �������̌����Ɋ�Â������όv�Z�iOBB�̎��Ɩ@���̓��e���j
		// rad�́A�@�������ւ�OBB�̓��e���a�ɑ���
		float rad = 0.0f;
		XMFLOAT3 dox(axisX.x * m_size.x, axisX.y * m_size.x, axisX.z * m_size.x);
		rad += fabs(XMFloat3Dot(dox, fnormal));
		dox = XMFLOAT3(axisY.x * m_size.y, axisY.y * m_size.y, axisY.z * m_size.y);
		rad += fabs(XMFloat3Dot(dox, fnormal));
		dox = XMFLOAT3(axisZ.x * m_size.z, axisZ.y * m_size.z, axisZ.z * m_size.z);
		rad += fabs(XMFloat3Dot(dox, fnormal));

		// �Փ˔���: �I�u�W�F�N�g�̒��S����g�̕\�ʂ܂ł̋�����OBB�̓��e���a�����������ꍇ
		if (fabs(length) < rad) {
			XMFLOAT3 outNormal, outAxis;
			outNormal = XMFloat3Normalize(fnormal); // �Փ˂����ʂ̖@���i�g�̖@���j
			outAxis = XMFloat3Normalize(axisZ);     // �I�u�W�F�N�g�̃t�H���[�h��

			float Culout = fabsf(IsParallel(outAxis, outNormal)); // �Փˊp�x�̕��s�x�����H

			// --- ���������]�Ɋւ��郍�W�b�N��ǉ�/�C�� ---

			// �Փ˂ɂ�锽�����x�̌v�Z�i�����j
			XMFLOAT3 velo{};
			velo.x = m_velocity.x - (1 + (m_reboundrate - Culout)) * XMFloat3Dot(fnormal, m_velocity) * fnormal.x;
			velo.y = m_velocity.y - (1 + (m_reboundrate - Culout)) * XMFloat3Dot(fnormal, m_velocity) * fnormal.y;
			velo.z = m_velocity.z - (1 + (m_reboundrate - Culout)) * XMFloat3Dot(fnormal, m_velocity) * fnormal.z;

			// �I�u�W�F�N�g�̌��݂̑��x�ƏՓ˖@��fnormal���g�p���āA�Փ˂ɂ���]�i�p�^���ʁj���v�Z
			// �ȗ������ꂽ�������f��
			XMVECTOR currentVelocity = XMLoadFloat3(&m_velocity);
			XMVECTOR collisionNormal = XMLoadFloat3(&fnormal);

			// �Փ˓_�ƃI�u�W�F�N�g�d�S�Ԃ̑��Έʒu�x�N�g���i�ȗ����FOBB�̒�ʒ��S����d�S�ցj
			// ���ۂɂ͏Փ˂���OBB�̍ł��Ⴂ�_����d�S�ւ̃x�N�g�������z
			// �����ł͕֋X��A���������̏Ռ��ɒ��ڂ��A�Փ˓_�Əd�S�̐��������̃Y���𖳎����܂��B
			// ����m_transform->GetPosition()���d�S�ł���΁Aposition��collisionPoint(�ڐG�����_)�̍���p����ׂ��B
			// �����ł́A�Փ˖@���Ɛ������x�̊O�ς��g���N�Ƃ��Ĉ����B
			XMVECTOR horizontalVelocity = XMVectorSet(currentVelocity.m128_f32[0], 0.0f, currentVelocity.m128_f32[2], 0.0f);

			// �Փ˂ɂ���]���̕������v�Z�i�g���N�̕����j
			// �������x�x�N�g���ƏՓ˖@���̊O�ς��A��]���̕����������B
			// ��F�E���獶�ւԂ���A���ʂ����ɌX���Ă���ꍇ�A��O�ɉ�]������g���N����������
			XMVECTOR impulseRotationAxis = XMVector3Normalize(XMVector3Cross(horizontalVelocity, collisionNormal));

			// �Փ˂ɂ���]���x�̑傫���i�X�J���[�j
			// �Փ˂̋����i�@�������̑��x�����j�ƁA���������̑��x�A�I�u�W�F�N�g�̃T�C�Y�Ȃǂ��l��
			float impactSpeed = XMFloat3Dot(fnormal, m_velocity); // �@�������̏Փˑ��x
			float impulseMagnitude = XMVector3Length(horizontalVelocity).m128_f32[0] * fabs(impactSpeed);

			// �����Œ����p�����[�^��ݒ�B���l���������قǉ�]���ɂ���
			// ROTATION_IMPULSE_STRENGTH���g�p
			float rotationImpulseStrength = ROTATION_IMPULSE_STRENGTH; // �萔���g�p

			// �p���x�̑���
			// m_rotation_speed�̓I�C���[�p�̉�]���x�Ɖ��肳��Ă��邽�߁A
			// ���̃g���N�𒼐�m_rotation_speed�ɕϊ�����̂͏����g���b�L�[�B
			// �ł��ȒP�ȕ��@�́AimpulseRotationAxis��m_rotation_speed�ɒǉ����邱�ƁB
			// �������AXMVECTOR��XMFLOAT3�ɕϊ����Ēǉ�����K�v������B

			XMFLOAT3 addedRotationSpeed;
			XMStoreFloat3(&addedRotationSpeed, impulseRotationAxis * impulseMagnitude * rotationImpulseStrength);

			// ������m_rotation_speed�ɉ��Z
			// ������m_rotation_speed��X,Y,Z������̉�]���x���Ӗ����Ă���Ɖ���
			m_rotation_speed.x += addedRotationSpeed.x;
			m_rotation_speed.y += addedRotationSpeed.y;
			m_rotation_speed.z += addedRotationSpeed.z;

			// --- ��]�Ɋւ��郍�W�b�N�I�� ---

			m_velocity = velo; // �X�V���ꂽ���x��K�p
			m_se->Play(); // ���ʉ��Đ�

			if (!m_iswater_touch) {
				auto emit = scene.AddGameObject<splasheEmitter>(1);
				emit->GetTransform()->SetPosition(position);
				emit->SetForse(fabs(velo.y) / 3);
				emit->SetUse(true);
				m_bound_count++;
				_field->AddWave(position); // ���ʂɔg���ǉ�
			}
			m_iswater_touch = true;
		}
		else {
			m_iswater_touch = false;
		}
	}

	// �ړ������i�X�V���ꂽ���x�ňʒu���X�V�j
	position.x += m_velocity.x * dt;
	position.y += m_velocity.y * dt;
	position.z += m_velocity.z * dt;
	m_transform->SetPosition(position); // �����ŃI�u�W�F�N�g�̈ʒu���X�V

	// �n�ʁi�܂��͐��ʁj�Ƃ̐ڒn����ƒ���
	Terrain* _field_check = GetParentScene().GetGameObject<Terrain>(); // �V�����ϐ����ŏՓ˖h�~
	if (_field_check) {
		if (!_field_check->CheckInTerrain(this)) return;

		// �I�u�W�F�N�g�̌��݂̈ʒu�ɂ�����g�̍������Ď擾
		// �����Ŏ擾����fnormal�́A�I�u�W�F�N�g��Y�ʒu���킹�Ɏg��
		// ��]�p�ɂ͏�L�̏Փ˔���u���b�N�Ōv�Z���ꂽfnormal���g���܂��B
		// ��͂̂悤�ȑ傫�ȃI�u�W�F�N�g�̏ꍇ�Aposition�̒P��̓_�ł͂Ȃ��A
		// �D�̂̍L������l���������ϓI�Ȕg�̖@����ʓr�v�Z���A
		// ����ɃI�u�W�F�N�g�̉�]�����킹��������R�ł��B
		// �����GetHeightAndNormal�͋Ǐ��I�Ȓl�Ȃ̂ŁA��q�́u�����I�ȌX���v�Z�N�V�������Q�Ƃ��Ă��������B
		XMFLOAT3 currentSurfaceNormal; // ���̎��_�̖@���́A�I�u�W�F�N�g��Y�ʒu���킹�Ɏg��
		float GroundHeight = _field_check->GetHeightAndNormal(position, currentSurfaceNormal) + (mscale.y * 0.5f);

		if (position.y <= GroundHeight) {
			position.y = GroundHeight; // �g�̕\�ʂ�Y���W���Œ�
			m_velocity.y = 0.0f;       // Y���x���[����

			ResultInstance* instance = Manager::GetManager().GetGameInstance<ResultInstance>();
			instance->m_bound = m_bound_count;
			instance->m_length = get_vector_length(XMFloat3Subtract(m_transform->GetPosition(), m_start_position));
			Manager::GetManager().ChangeScene();
		}
	}
	// �ŏI�I�Ȉʒu�̓K�p�i��Lif�u���b�N��position.y���ύX���ꂽ�ꍇ���l���j
	m_transform->SetPosition(position);
}

void Stone::Draw() {
	//���̓��C�A�E�g
	Renderer::GetRenderer().GetDeviceContext()->IASetInputLayout(m_VertexLayout);

	//shader�ݒ�
	Renderer::GetRenderer().GetDeviceContext()->VSSetShader(m_vertex_shader, NULL, 0);
	Renderer::GetRenderer().GetDeviceContext()->PSSetShader(m_pixel_shader, NULL, 0);

	//�}�g���N�X�ݒ�
	Renderer::GetRenderer().SetWorldMatrix(m_transform->GetWorldMatrix());


	//�}�e���A��
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	material.TextureEnable = true;
	Renderer::GetRenderer().SetMaterial(material);

	m_modelrenderer->Draw();
}

void Stone::AddForce(const XMFLOAT3& force) {
	m_velocity = XMFloat3Add(m_velocity, force);
}

void Stone::AddRotForce(const XMFLOAT3& force) {
	m_rotation_speed = force;
}
