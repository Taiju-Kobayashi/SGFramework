#pragma once
#ifndef _PEOBJECT_H_
#define _PEOBJECT_H_

#include "main.h"

class PostEffectObject {
public:
	PostEffectObject() = default;
	virtual ~PostEffectObject(){}
	virtual void Draw(ID3D11ShaderResourceView* ppTexture) = 0;
};




#endif