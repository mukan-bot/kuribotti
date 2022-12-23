//=============================================================================
//
// サウンド処理 [sound.h]
//
//=============================================================================
#pragma once

#include <windows.h>
#include "xaudio2.h"						// サウンド処理で必要

//*****************************************************************************
// サウンドファイル
//*****************************************************************************
enum
{
	SOUND_LABEL_BGM_resultKati,	// BGM0
	SOUND_LABEL_BGM_resultMake,	// BGM1
	SOUND_LABEL_BGM_game,		// BGM2
	SOUND_LABEL_BGM_title,		// BGM3

	SOUND_LABEL_SE_select1,	// 選択１
	SOUND_LABEL_SE_select2 ,// 選択２

	SOUND_LABEL_SE_wana1,// 罠発動タイプ１ 
	SOUND_LABEL_SE_wana2,// 罠発動タイプ２
	SOUND_LABEL_SE_wana3,// 罠発動タイプ３



	SOUND_LABEL_MAX,
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
bool InitSound(HWND hWnd);
void UninitSound(void);
void PlaySound(int label);
void StopSound(int label);
void StopSound(void);

