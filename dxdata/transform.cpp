#include "transform.h"


void Transform::Init() {
	m_trans_matrix = XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
	XMVECTOR quat = XMQuaternionRotationRollPitchYaw(m_rotation.x, m_rotation.y, m_rotation.z);
	XMStoreFloat4(&m_quaternion, quat);
	m_rotate_matrix = XMMatrixRotationQuaternion(XMLoadFloat4(&m_quaternion));
	m_scale_matrix = XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);
}

const XMFLOAT3& Transform::GetPosition() {
	return m_position;
}

const XMFLOAT3& Transform::SetPosition(XMFLOAT3 pos) {
	m_position = pos;
	m_trans_matrix = XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
	return m_position;
}

const XMFLOAT3& Transform::SetPosition(float x, float y, float  z) {
	m_position = XMFLOAT3(x,y,z);
	m_trans_matrix = XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
	return m_position;
}
const XMFLOAT3& Transform::GetRotation() {
	return m_rotation;
}

const XMFLOAT3& Transform::SetRotation(XMFLOAT3 rot) {
	m_rotation = rot;
	XMVECTOR quat = XMQuaternionRotationRollPitchYaw(m_rotation.x, m_rotation.y, m_rotation.z);
	XMStoreFloat4(&m_quaternion, quat);
	m_rotate_matrix = XMMatrixRotationQuaternion(XMLoadFloat4(&m_quaternion));
	return m_rotation;
}
const XMFLOAT3& Transform::SetRotation(float x, float y, float  z) {
	m_rotation = XMFLOAT3(x, y, z);
	XMVECTOR quat = XMQuaternionRotationRollPitchYaw(m_rotation.x, m_rotation.y, m_rotation.z);
	XMStoreFloat4(&m_quaternion, quat);
	m_rotate_matrix = XMMatrixRotationQuaternion(XMLoadFloat4(&m_quaternion));
	return m_rotation;
}

const XMFLOAT3& Transform::GetScale() {
	return m_scale;
}
const XMFLOAT3& Transform::SetScale(XMFLOAT3 scale) {
	m_scale = scale;
	m_scale_matrix =  XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);
	return m_scale;
}

const XMFLOAT3& Transform::SetScale(float x, float y, float  z) {
	m_scale = XMFLOAT3(x, y, z);
	m_scale_matrix = XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);
	return m_scale;
}

XMFLOAT3 Transform::GetForward() 
{
	XMFLOAT3 foward;
	XMStoreFloat3(&foward, m_rotate_matrix.r[2]);
	return foward;
}

XMVECTOR Transform::GetVectorForward()
{
	return XMVector3TransformNormal(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), m_rotate_matrix);
}

XMFLOAT3 Transform::GetRight() 
{
	XMFLOAT3 right;
	XMStoreFloat3(&right, m_rotate_matrix.r[0]);
	return right;
}

XMVECTOR Transform::GetVectorRight()
{
	return XMVector3TransformNormal(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), m_rotate_matrix);
}


XMFLOAT3 Transform::GetUp() 
{
	XMFLOAT3 Up;
	XMStoreFloat3(&Up, m_rotate_matrix.r[1]);
	return Up;
}

XMVECTOR Transform::GetVectorUp()
{
	return XMVector3TransformNormal(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), m_rotate_matrix);
}

const void Transform::SetScaleMatrix(XMMATRIX& mat) {
	m_scale_matrix = mat;
}

const void Transform::SetRotateMatrix(XMMATRIX& mat) {
	m_rotate_matrix = mat;

	{
		float pitch = asin(-DirectX::XMVectorGetY(DirectX::XMVector3TransformNormal(DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), m_rotate_matrix)));
		float yaw = atan2(DirectX::XMVectorGetX(DirectX::XMVector3TransformNormal(DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), m_rotate_matrix)), DirectX::XMVectorGetZ(DirectX::XMVector3TransformNormal(DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), m_rotate_matrix)));
		float roll = atan2(DirectX::XMVectorGetX(DirectX::XMVector3TransformNormal(DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), m_rotate_matrix)), DirectX::XMVectorGetZ(DirectX::XMVector3TransformNormal(DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), m_rotate_matrix)));

		m_rotation = XMFLOAT3(roll, pitch, yaw);
	}
}

const void Transform::SetTransMatrix(XMMATRIX& mat) {
	m_trans_matrix = mat;
}


const XMMATRIX& Transform::GetScaleMatrix() {
	return m_scale_matrix;
}

const XMMATRIX& Transform::GetRotateMatrix() {
	return m_rotate_matrix;
}
const XMMATRIX& Transform::GetTransMatrix() {
	return m_trans_matrix;
}

const XMMATRIX Transform::GetWorldMatrix() {
	return (m_scale_matrix * m_rotate_matrix * m_trans_matrix);
}

const void Transform::SetQuaternion(XMFLOAT4 quat) {
		m_quaternion = quat;
		m_rotate_matrix = XMMatrixRotationQuaternion(XMLoadFloat4(&m_quaternion));
}

const XMVECTOR Transform::GetQuaternion() {
	return XMLoadFloat4(&m_quaternion);
}