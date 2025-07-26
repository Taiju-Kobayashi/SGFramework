/**
 * @file   scene.h
 * @brief  �Q�[����ʓ�(�^�C�g����)�̃I�u�W�F�N�g���Ǘ�����V�[���̊��N���X
 * @author Taiju Kobayashi
 * @date   2024/12�@
 */
#pragma once
#ifndef _SCENE_H_
#define _SCENE_H_

#define MAX_LAYER_NUM (3)

#include <vector>
#include <list>
#include <algorithm>
#include <string>
#include <stdexcept>
#include "gameObject.h"
#include "camera.h"
#include "transform.h"
class GameObject;

/// @brief�@�Q�[����ʓ�(�^�C�g����)�̃I�u�W�F�N�g���Ǘ�����V�[���̊��N���X
class Scene
{
public:
	virtual ~Scene() {};

	virtual void Init() {};

	/// @brief �x�N�^�[�̗v�f�^�����擾
	/// @tparam T �x�N�^�[�̗v�f�^
	/// @param my_list �v�f�^�����擾����x�N�^�[
	/// @return �v�f�^��
	template <typename T>
	std::string GetElementType(std::vector<T> my_list) {
		std::string cname(typeid(T).name());
		return cname;
	}

	/// @brief �e���v���[�g�֐��B�w�肵�����C���[�ɃQ�[���I�u�W�F�N�g��ǉ�����B
	/// @tparam T �Q�[���I�u�W�F�N�g�̌^
	/// @param layernum ���C���[�ԍ�
	/// @return �ǉ����ꂽ�Q�[���I�u�W�F�N�g�̃|�C���^�B�ǉ��Ɏ��s�����ꍇ�� nullptr ��Ԃ��B
	template <typename T>//�e���v���[�g
	T* AddGameObject(unsigned int layernum) {
		if (MAX_LAYER_NUM <= layernum)return nullptr;
		T* obj = new T();
		obj->SetGetGeneId(_Objects[layernum].size());
		obj->SetParentScene(this);
		Transform* trf = new Transform(obj);
		trf->Init();
		obj->SetTransform(trf);
		obj->Init();
		_Objects[layernum].emplace_back(obj);
		return obj;
	}


	/// @brief �e���v���[�g�֐��B�w�肵���^�̃Q�[���I�u�W�F�N�g���擾����B�ŏ��Ɍ����������̂�Ԃ��B
   /// @tparam T �Q�[���I�u�W�F�N�g�̌^
   /// @return ���������Q�[���I�u�W�F�N�g�̃|�C���^�B������Ȃ������ꍇ�� nullptr ��Ԃ��B
	template <typename T>
	T* GetGameObject()
	{
		for (int i = 0; i < MAX_LAYER_NUM; i++)
		{
			for (GameObject* obj : _Objects[i])
			{
				T* ret = dynamic_cast<T*>(obj);
				if (ret != nullptr)
					return ret;
			}
		}

		return nullptr;
	}

	/// @brief �e���v���[�g�֐��B�w�肵���^�̃Q�[���I�u�W�F�N�g�����ׂĎ擾����B
	/// @tparam T �Q�[���I�u�W�F�N�g�̌^
	/// @return ���������Q�[���I�u�W�F�N�g�̃|�C���^�̃x�N�^�[�B
	template<typename T>
	std::vector<T*> GetGameObjects() {
		std::vector<T*> tmp{};
		for (int i = 0; i < MAX_LAYER_NUM; i++)
		{
			for (GameObject* obj : _Objects[i]) {
				T* ret = dynamic_cast<T*>(obj);
				if (ret != nullptr) {
					tmp.emplace_back(ret);
				}
			}
		}
		return tmp;
	}

	/// @brief �e���v���[�g�֐��B�w�肵�����C���[�̎w�肵���^�̃Q�[���I�u�W�F�N�g�����ׂĎ擾����B
	/// @tparam T �Q�[���I�u�W�F�N�g�̌^
	/// @param LayerNum ���C���[�ԍ�
	/// @return ���������Q�[���I�u�W�F�N�g�̃|�C���^�̃x�N�^�[�B
	template<typename T>
	std::vector<T*> GetGameObjects(int LayerNum) {
		std::vector<T*> tmp{};
		if (MAX_LAYER_NUM > LayerNum) {
			for (GameObject* obj : _Objects[LayerNum]) {
				T* ret = dynamic_cast<T*>(obj);
				if (ret != nullptr) {
					tmp.emplace_back(ret);
				}
			}
		}
		return tmp;
	}


	virtual void UnInit() {
		for (int i = 0; i < MAX_LAYER_NUM; i++)
		{
			for (GameObject* pm : _Objects[i])
			{
				pm->ClearComponent();
				pm->UnInit();//�|�����[�t�B�Y��
				delete pm;
			}
			_Objects[i].clear();

		}

	};
	virtual void Update() {};

	/// @brief	�`��O����
	virtual void PreDraw() {
		for (int i = 0; i < MAX_LAYER_NUM; i++)
		{
			for (GameObject* pm : _Objects[i])
			{
				pm->PreDraw();
			}
		}
	}

	/// @brief	�`�揈��
	virtual void Draw() {
		for (int i = 0; i < (MAX_LAYER_NUM - 1); i++)
		{
			for (GameObject* pm : _Objects[i])
			{
				pm->Draw();
			}
		}
	};

	virtual void PostDraw() {
		for (GameObject* pm : _Objects[MAX_LAYER_NUM - 1])
		{
			pm->Draw();
		}
	};

	void ParentUpdate() {

		for (int i = 0; i < MAX_LAYER_NUM; i++)
		{
			for (size_t cnt = 0; cnt < _Objects[i].size(); cnt++)
			{
				GameObject* og;
				try
				{
					og = _Objects[i].at(cnt);
				}
				catch (const std::out_of_range& ex)
				{
					break;
				}
				og->Update();
			}
			std::erase_if(_Objects[i], [](GameObject* go) {return go->Destroy(); });
		}
	};
protected:
	// �Q�[���I�u�W�F�N�g���i�[����x�N�^�[�̔z��B3�̃��C���[�����B
	std::vector<GameObject*> _Objects[MAX_LAYER_NUM];
	//�ϐ��̃��C���[�ɑΉ�����������r���̔z��(���󖢎g�p)
	std::vector<std::vector<GameObject*>> _Tests;

};

#endif // !_SCENE_H_
