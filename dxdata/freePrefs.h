/*
* FreePrefs.h
* �ݒ�̕ۑ��A�擾�A�폜���s�����߂̐ÓI���\�b�h��񋟂��A�ݒ�t�@�C���̓ǂݍ��݂Ə������݂��Ǘ����܂��B
* date: 2025/06/28 
* author: Kobayashi
*/

#pragma once
#ifndef _FREEPREFS_H_
#define _FREEPREFS_H_

class FreePrefs {
public:
	//	���݂̐ݒ���t�@�C���ɕۑ�����
	static bool PrefsSave();
	//	�L�[�ƒl�̃y�A�����݂��邩�ǂ������m�F����
	static bool HasKey(std::string key);
	//	�S�ẴL�[�ƒl�̃y�A���폜����
	static void DeleteAll();
	//	�L�[�ƒl�̃y�A���폜����
	static void DeleteKey(std::string key);
	//	�L�[�ƒl�̃y�A��ݒ肷��
	static void SetInt(std::string key, int value);
	//	Returns 0 if the key does not exist
	static int GetInt(std::string key);
	//	�L�[�ƒl�̃y�A��ݒ肷��
	static void SetFloat(std::string key, float value);
	//	Returns 0.0f if the key does not exist
	static float GetFloat(std::string key);
	//	�L�[�ƒl�̃y�A��ݒ肷��
	static void SetString(std::string key, std::string value);
	//	Returns an empty string if the key does not exist
	static std::string GetString(std::string key);


	//	�������֐��B�t�@�C������ݒ��ǂݍ���
	static void Init(const char* path);
	//	�I���֐��B�ݒ���t�@�C���ɕۑ����āA���������������
	static void Uninit();
private:
	static std::string m_filepath;// �ݒ�t�@�C���̃p�X
	//	�ݒ��ۑ����邽�߂̃}�b�v
	static std::unordered_map<std::string, int>m_IntMap;
	static std::unordered_map<std::string, float>m_floatMap;
	static std::unordered_map<std::string, std::string>m_StringMap;
};


#endif // !_FREEPREFS_H_
