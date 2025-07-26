/**
 * @file   manager.h
 * @brief  ゲームオブジェクトの基本機能を纏めた基底クラスのヘッダー
 * @author Taiju Kobayashi
 * @date   2024/07
 */
#pragma once
class Component;
class Scene;
#include "transform.h"

/// @brief　ゲームオブジェクトの基本機能を纏めた基底クラス
class GameObject {

public:
	virtual ~GameObject() {};
	virtual void Init() = 0;
	virtual void UnInit() = 0;
	virtual void Update() = 0;
	virtual void PreDraw() {};
	virtual void Draw() = 0;

	void SetTransform(Transform* trf) {
		if (!m_transform) {
			m_transform = trf;
		}
	}
	Transform* GetTransform() { return m_transform; }




	void SetParent(GameObject* parent){
		if (m_parent_object) {
			m_parent_object->SetChild(nullptr);
		}
		m_parent_object = parent;
	}
	void SetChild(GameObject* Child){
		if (!Child) {
			m_child_object = Child;
	}
		else if (!m_child_object) {
			m_child_object = Child;
		}
	}

	GameObject* GetParent() { return m_parent_object; }
	GameObject* GetChild() { return m_child_object; }

	void SetDestroy() {
		m_destroy_flag = true;
	}

	bool Destroy() {
		if (m_destroy_flag) {
			UnInit();
			delete this;
			return true;
		}
		else
		{
			return false;
		}
	}

	//IDが設定されていた場合、登録済みの数値が返ってくる
	int SetGetGeneId(int num) {

		if (m_gene_id < 0) {
			m_gene_id = num;
			return num;
		}
		return m_gene_id;
	}
	int SetGetGeneId() {
		return m_gene_id;
	}
	void SetParentScene(Scene* scene) {
		if (!m_parent_scene) {
			m_parent_scene = scene;
		}
	}
	void AddComponent(Component* comp) {
		m_Components.push_back(comp);
	}
	void ClearComponent() {
		for (auto* comp : m_Components) {
			comp->UnInit();
		}
		m_Components.clear();
		m_transform->UnInit();
		delete m_transform;
	}


protected:
	bool m_destroy_flag = false;

	XMFLOAT3 m_size = XMFLOAT3(1.0f, 1.0f, 1.0f);

	std::vector< Component*> m_Components{};
	Scene& GetParentScene() { return *m_parent_scene; }

	Transform* m_transform = nullptr;

	template <typename T>
	T* GetComponent()
	{
		//指定されたクラスのコンポーネントを
		//リスト内から最初に見つかったもののポインタを返す
		for (Component* obj : m_Components)
		{
			T* ret = dynamic_cast<T*>(obj);
			if (ret != nullptr)
				return ret;
		}
		return nullptr;
	}
	private:
		int m_gene_id = -1;
		Scene* m_parent_scene = nullptr;

		GameObject* m_parent_object = nullptr;
		GameObject* m_child_object = nullptr;

};

