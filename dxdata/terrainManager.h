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
	/// 現在居るテレインのポインタを返す
	/// ※テレイン内に居ない場合,Nullを返す
	/// </summary>
	/// <param name="ThisObject"></param>
	/// <returns></returns>
	GameObject* GetTerrain(GameObject* ThisObject);

};