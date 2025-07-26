#pragma once

class GameObject;

class ITerrain {
public:
	virtual ~ITerrain() = 0;
	virtual bool CheckInTerrain(GameObject* Target) =  0;
	virtual float GetHeight(XMFLOAT3& Position) = 0;
};