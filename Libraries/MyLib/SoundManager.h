/*
	@file	SoundManager.h
	@brief	サウンド管理クラス
*/
#pragma once
#include <unordered_map>
#include "Game/Other/Sounds.h"

namespace mylib
{
	class SoundManager
	{
	private:
		using BgmMap = std::unordered_map<XACT_WAVEBANK_SOUNDS, std::unique_ptr<DirectX::SoundEffectInstance>>;

		std::unique_ptr<DirectX::AudioEngine> m_audioEngine;
		std::unique_ptr<DirectX::WaveBank> m_waveBank;
		BgmMap m_bgms;
		DirectX::SoundEffectInstance* m_currentBGM;

	public:
		SoundManager();
		~SoundManager();

		void Initialize();
		void Update();

		void PlaySE(XACT_WAVEBANK_SOUNDS seName);
		void PlayBGM(XACT_WAVEBANK_SOUNDS bgmName);
		void StopBGM();
	};
}
