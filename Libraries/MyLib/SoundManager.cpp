/*
	@file	SoundManager.cpp
	@brief	サウンド管理クラス
*/
#include "pch.h"
#include "Libraries/MyLib/SoundManager.h"

//---------------------------------------------------------
// コンストラクタ
//---------------------------------------------------------
mylib::SoundManager::SoundManager()
	:
	m_audioEngine{},
	m_waveBank{},
	m_bgms{},
	m_currentBGM{}
{
}

//---------------------------------------------------------
// デストラクタ
//---------------------------------------------------------
mylib::SoundManager::~SoundManager()
{
	// BGMを停止する
	for (auto& bgm : m_bgms)
	{
		bgm.second->Stop(true);
	}

	// オーディオエンジンの後始末
	if (m_audioEngine)
	{
		m_audioEngine->Suspend();
	}
}

//---------------------------------------------------------
// 初期化処理
//---------------------------------------------------------
void mylib::SoundManager::Initialize()
{
	// オーディオエンジンのフラグを設定する
	DirectX::AUDIO_ENGINE_FLAGS eflags = DirectX::AudioEngine_Default;
#ifdef _DEBUG
	eflags |= DirectX::AudioEngine_Debug;
#endif

	// オーディオエンジンを作成する
	m_audioEngine = std::make_unique<DirectX::AudioEngine>(eflags);

	// サウンドバンクを作成する
	m_waveBank = std::make_unique<DirectX::WaveBank>(m_audioEngine.get(), L"Resources/Sounds/sounds.xwb");

	// BGMをマップに登録する
	m_bgms.insert(BgmMap::value_type(XACT_WAVEBANK_SOUNDS_TITLESCENE_BGM,
		m_waveBank->CreateInstance(XACT_WAVEBANK_SOUNDS_TITLESCENE_BGM)));
	m_bgms.insert(BgmMap::value_type(XACT_WAVEBANK_SOUNDS_SELECTSCENE_BGM,
		m_waveBank->CreateInstance(XACT_WAVEBANK_SOUNDS_SELECTSCENE_BGM)));
	m_bgms.insert(BgmMap::value_type(XACT_WAVEBANK_SOUNDS_PLAYSCENE_BGM,
		m_waveBank->CreateInstance(XACT_WAVEBANK_SOUNDS_PLAYSCENE_BGM)));
	m_bgms.insert(BgmMap::value_type(XACT_WAVEBANK_SOUNDS_RESULTSCENEWIN_BGM,
		m_waveBank->CreateInstance(XACT_WAVEBANK_SOUNDS_RESULTSCENEWIN_BGM)));
	m_bgms.insert(BgmMap::value_type(XACT_WAVEBANK_SOUNDS_RESULTSCENELOSE_BGM,
		m_waveBank->CreateInstance(XACT_WAVEBANK_SOUNDS_RESULTSCENELOSE_BGM)));
}

//---------------------------------------------------------
// 更新処理
//---------------------------------------------------------
void mylib::SoundManager::Update()
{
	// オーディオエンジンを更新する
	if (!m_audioEngine->Update())
	{
		// No audio device is active
		if (m_audioEngine->IsCriticalError())
		{
			OutputDebugString(L"AudioEngine Error!\n");
		}
	}
}

//---------------------------------------------------------
// SEの再生
//---------------------------------------------------------
void mylib::SoundManager::PlaySE(XACT_WAVEBANK_SOUNDS seName)
{
	m_waveBank->Play(seName);
}

//---------------------------------------------------------
// BGMの再生
//---------------------------------------------------------
void mylib::SoundManager::PlayBGM(XACT_WAVEBANK_SOUNDS bgmName)
{
	// 今のBGMを停止する
	if (m_currentBGM)
	{
		m_currentBGM->Stop(true);
	}

	// 新しいBGMを再生する
	m_bgms.at(bgmName)->Play(true);

	// 新しいBGMを今のBGMとする
	m_currentBGM = m_bgms.at(bgmName).get();
}

//---------------------------------------------------------
// BGMの停止
//---------------------------------------------------------
void mylib::SoundManager::StopBGM()
{
	m_currentBGM->Stop(true);
	m_currentBGM = nullptr;
}
