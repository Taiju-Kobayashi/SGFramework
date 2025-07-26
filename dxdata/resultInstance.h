/**
 * @file   resultInstance.h
 * @brief  ���U���g��ʂɂĕ\������X�R�A����ێ�����N���X
 * @author Taiju Kobayashi
 * @date   2025/01
 */
#pragma once
#ifndef _RESULTINSTANCE_H_
#define _RESULTINSTANCE_H_
#include "dataInstance.h"

 /// @brief�@���U���g��ʂɂĕ\������X�R�A����ێ�����N���X
class ResultInstance : public DataInstance {
public:
	using DataInstance::DataInstance;
	int m_bound;
	float m_length;
	bool m_move_end;
	const char* m_choicestone_path = nullptr;
	//���v���C�p
	XMFLOAT3 m_stoneforce{};
	XMFLOAT3 m_rotforce{};
	XMFLOAT3 m_rotate{};

	
	ResultInstance() :m_length(0.0f), m_bound(0), m_move_end(false),m_choicestone_path(nullptr){}
};

#endif // !_RESULT_INSTANCE_H_
