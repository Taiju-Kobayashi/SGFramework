#pragma once
#include "gameObject.h"
#include "component.h"

class TerrainManager :public GameObject {
	std::list<GameObject*>_Terrains;
public:
	void Init()override;
	void UnInit()override;
	void Update()override;
	void Draw()override;

	/// <summary>
	/// ���݋���e���C���̃|�C���^��Ԃ�
	/// ���e���C�����ɋ��Ȃ��ꍇ,Null��Ԃ�
	/// </summary>
	/// <param name="ThisObject"></param>
	/// <returns></returns>
	GameObject* GetTerrain(GameObject* ThisObject);

};