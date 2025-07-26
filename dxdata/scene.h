/**
 * @file   scene.h
 * @brief  ゲーム場面内(タイトル等)のオブジェクトを管理するシーンの基底クラス
 * @author Taiju Kobayashi
 * @date   2024/12　
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

/// @brief　ゲーム場面内(タイトル等)のオブジェクトを管理するシーンの基底クラス
class Scene
{
public:
	virtual ~Scene() {};

	virtual void Init() {};

	/// @brief ベクターの要素型名を取得
	/// @tparam T ベクターの要素型
	/// @param my_list 要素型名を取得するベクター
	/// @return 要素型名
	template <typename T>
	std::string GetElementType(std::vector<T> my_list) {
		std::string cname(typeid(T).name());
		return cname;
	}

	/// @brief テンプレート関数。指定したレイヤーにゲームオブジェクトを追加する。
	/// @tparam T ゲームオブジェクトの型
	/// @param layernum レイヤー番号
	/// @return 追加されたゲームオブジェクトのポインタ。追加に失敗した場合は nullptr を返す。
	template <typename T>//テンプレート
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


	/// @brief テンプレート関数。指定した型のゲームオブジェクトを取得する。最初に見つかったものを返す。
   /// @tparam T ゲームオブジェクトの型
   /// @return 見つかったゲームオブジェクトのポインタ。見つからなかった場合は nullptr を返す。
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

	/// @brief テンプレート関数。指定した型のゲームオブジェクトをすべて取得する。
	/// @tparam T ゲームオブジェクトの型
	/// @return 見つかったゲームオブジェクトのポインタのベクター。
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

	/// @brief テンプレート関数。指定したレイヤーの指定した型のゲームオブジェクトをすべて取得する。
	/// @tparam T ゲームオブジェクトの型
	/// @param LayerNum レイヤー番号
	/// @return 見つかったゲームオブジェクトのポインタのベクター。
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
				pm->UnInit();//ポリモーフィズム
				delete pm;
			}
			_Objects[i].clear();

		}

	};
	virtual void Update() {};

	/// @brief	描画前処理
	virtual void PreDraw() {
		for (int i = 0; i < MAX_LAYER_NUM; i++)
		{
			for (GameObject* pm : _Objects[i])
			{
				pm->PreDraw();
			}
		}
	}

	/// @brief	描画処理
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
	// ゲームオブジェクトを格納するベクターの配列。3つのレイヤーを持つ。
	std::vector<GameObject*> _Objects[MAX_LAYER_NUM];
	//可変数のレイヤーに対応させる実装途中の配列(現状未使用)
	std::vector<std::vector<GameObject*>> _Tests;

};

#endif // !_SCENE_H_
