#include "main.h"
#include "manager.h"
#include "terrainManager.h"
#include "scene.h"
#include "terrain.h"
#include "wave.h"
#include "iTerrain.h"

void TerrainManager::Init() {

}

void TerrainManager::UnInit() {

}

void TerrainManager::Update() {

}

void TerrainManager::Draw() {


}


GameObject* TerrainManager::GetTerrain(GameObject* ThisObject) {

	float minlength = std::numeric_limits<float>::max();//“K“–‚È’l‚Å‰Šú‰»
	GameObject* minLengthObject = nullptr;
	for (auto terr : _Terrains) {
		XMFLOAT3	distance(terr->GetPosition().x - ThisObject->GetPosition().x,
			terr->GetPosition().y - ThisObject->GetPosition().y,
			terr->GetPosition().z - ThisObject->GetPosition().z);
		float	length = (distance.x * distance.x + distance.y * distance.y + distance.z * distance.z);

		if (length < minlength) {
			minLengthObject = terr;
			minlength = length;
		}
	}

	ITerrain* iter = dynamic_cast<ITerrain*>(ThisObject);
	if (iter != nullptr)
	{
		if (minLengthObject && iter->CheckInTerrain(ThisObject))
		{
			return minLengthObject;
		}
	}

	return nullptr;
}