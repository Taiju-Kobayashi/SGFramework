/*
* FreePrefs.h
* 設定の保存、取得、削除を行うための静的メソッドを提供し、設定ファイルの読み込みと書き込みを管理します。
* date: 2025/06/28 
* author: Kobayashi
*/

#pragma once
#ifndef _FREEPREFS_H_
#define _FREEPREFS_H_

class FreePrefs {
public:
	//	現在の設定をファイルに保存する
	static bool PrefsSave();
	//	キーと値のペアが存在するかどうかを確認する
	static bool HasKey(std::string key);
	//	全てのキーと値のペアを削除する
	static void DeleteAll();
	//	キーと値のペアを削除する
	static void DeleteKey(std::string key);
	//	キーと値のペアを設定する
	static void SetInt(std::string key, int value);
	//	Returns 0 if the key does not exist
	static int GetInt(std::string key);
	//	キーと値のペアを設定する
	static void SetFloat(std::string key, float value);
	//	Returns 0.0f if the key does not exist
	static float GetFloat(std::string key);
	//	キーと値のペアを設定する
	static void SetString(std::string key, std::string value);
	//	Returns an empty string if the key does not exist
	static std::string GetString(std::string key);


	//	初期化関数。ファイルから設定を読み込む
	static void Init(const char* path);
	//	終了関数。設定をファイルに保存して、メモリを解放する
	static void Uninit();
private:
	static std::string m_filepath;// 設定ファイルのパス
	//	設定を保存するためのマップ
	static std::unordered_map<std::string, int>m_IntMap;
	static std::unordered_map<std::string, float>m_floatMap;
	static std::unordered_map<std::string, std::string>m_StringMap;
};


#endif // !_FREEPREFS_H_
