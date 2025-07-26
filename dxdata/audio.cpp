
#include "main.h"
#include "audio.h"





IXAudio2*				Audio::m_xaudio = NULL;
IXAudio2MasteringVoice*	Audio::m_mastering_voice = NULL;

void Audio::InitMaster()
{
	DWORD dwChannelMask;

	// COM初期化
	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	// XAudio生成
	XAudio2Create(&m_xaudio, 0);

	// マスタリングボイス生成
	m_xaudio->CreateMasteringVoice(&m_mastering_voice);
}


void Audio::UninitMaster()
{
	m_mastering_voice->DestroyVoice();
	m_xaudio->Release();
	CoUninitialize();
}









void Audio::Load(const char *FileName)
{

	// サウンドデータ読込
	WAVEFORMATEX wfx = { 0 };

	{
		HMMIO hmmio = NULL;
		MMIOINFO mmioinfo = { 0 };
		MMCKINFO riffchunkinfo = { 0 };
		MMCKINFO datachunkinfo = { 0 };
		MMCKINFO mmckinfo = { 0 };
		UINT32 buflen;
		LONG readlen;


		hmmio = mmioOpen((LPSTR)FileName, &mmioinfo, MMIO_READ);
		assert(hmmio);

		riffchunkinfo.fccType = mmioFOURCC('W', 'A', 'V', 'E');
		mmioDescend(hmmio, &riffchunkinfo, NULL, MMIO_FINDRIFF);

		mmckinfo.ckid = mmioFOURCC('f', 'm', 't', ' ');
		mmioDescend(hmmio, &mmckinfo, &riffchunkinfo, MMIO_FINDCHUNK);

		if (mmckinfo.cksize >= sizeof(WAVEFORMATEX))
		{
			mmioRead(hmmio, (HPSTR)&wfx, sizeof(wfx));
		}
		else
		{
			PCMWAVEFORMAT pcmwf = { 0 };
			mmioRead(hmmio, (HPSTR)&pcmwf, sizeof(pcmwf));
			memset(&wfx, 0x00, sizeof(wfx));
			memcpy(&wfx, &pcmwf, sizeof(pcmwf));
			wfx.cbSize = 0;
		}
		mmioAscend(hmmio, &mmckinfo, 0);

		datachunkinfo.ckid = mmioFOURCC('d', 'a', 't', 'a');
		mmioDescend(hmmio, &datachunkinfo, &riffchunkinfo, MMIO_FINDCHUNK);



		buflen = datachunkinfo.cksize;
		m_sound_data = new unsigned char[buflen];
		readlen = mmioRead(hmmio, (HPSTR)m_sound_data, buflen);


		m_length = readlen;
		m_play_length = readlen / wfx.nBlockAlign;


		mmioClose(hmmio, 0);
	}


	// サウンドソース生成
	m_xaudio->CreateSourceVoice(&m_source_voice, &wfx);
	assert(m_source_voice);
}


void Audio::UnInit()
{
	m_source_voice->Stop();
	m_source_voice->DestroyVoice();

	delete[] m_sound_data;
}





void Audio::Play(bool Loop)
{
	m_source_voice->Stop();
	m_source_voice->FlushSourceBuffers();


	// バッファ設定
	XAUDIO2_BUFFER bufinfo;

	memset(&bufinfo, 0x00, sizeof(bufinfo));
	bufinfo.AudioBytes = m_length;
	bufinfo.pAudioData = m_sound_data;
	bufinfo.PlayBegin = 0;
	bufinfo.PlayLength = m_play_length;

	// ループ設定
	if (Loop)
	{
		bufinfo.LoopBegin = 0;
		bufinfo.LoopLength = m_play_length;
		bufinfo.LoopCount = XAUDIO2_LOOP_INFINITE;
	}

	m_source_voice->SubmitSourceBuffer(&bufinfo, NULL);


	float outputMatrix[4] = { 0.0f , 0.0f, 1.0f , 0.0f };
	m_source_voice->SetOutputMatrix(m_mastering_voice, 2, 2, outputMatrix);
	m_source_voice->SetVolume(m_volume);

	// 再生
	m_source_voice->Start();
}

void Audio::Restart() {
	m_source_voice->Start();
}

void Audio::Stop() {
	m_source_voice->Stop();
}