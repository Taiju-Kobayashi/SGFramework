/**
 * @file   component.h
 * @brief  コンポーネントの基底クラスのヘッダー
 * @author Taiju Kobayashi
 * @date   2024/12
 */
#pragma once
#ifndef  _COMPONENT_H_
#define _COMPONENT_H_

 /// @brief　コンポーネントの基底クラス
class Component
{
protected:
	class  GameObject* m_game_object = nullptr;

public:
	Component() = delete;
	Component(GameObject* Object):m_game_object(Object){ 
	}
	virtual ~Component(){}

	virtual void Init() {};//只の仮想関数と中身のない関数
	virtual void UnInit() {};//実装する必要がないクラスでオーバライドする必要が無くなる
	virtual void Update() {};
	virtual void Draw() {};
};


#endif //  _COMPONENT_H_
