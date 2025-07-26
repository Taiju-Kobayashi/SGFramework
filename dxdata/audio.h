/**
 * @file   audio.h
 * @brief  効果音、BGM等の読込と再生処理を行うクラスのヘッダー
 * @author Taiju Kobayashi
 * @date   2024/07
 */
#pragma once

#include <xaudio2.h>
#include "gameObject.h"
#include "component.h"
 /// @brief　効果音、BGM等の読込と再生処理を行うクラス
class Audio : public Component
{
public:
	static void InitMaster();
	static void UninitMaster();

	using Component::Component;

	void UnInit()override;

	void Load(const char *FileName);
	void Play(bool Loop = false);
	void Restart();
	void Stop();
private:
	static IXAudio2* m_xaudio;
	static IXAudio2MasteringVoice* m_mastering_voice;

	IXAudio2SourceVoice* m_source_voice{};
	BYTE* m_sound_data{};

	int						m_length{};
	int						m_play_length{};

	float m_volume = 1.0f;
};

