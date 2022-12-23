//=============================================================================
//
// �T�E���h���� [sound.h]
//
//=============================================================================
#pragma once

#include <windows.h>
#include "xaudio2.h"						// �T�E���h�����ŕK�v

//*****************************************************************************
// �T�E���h�t�@�C��
//*****************************************************************************
enum
{
	SOUND_LABEL_BGM_resultKati,	// BGM0
	SOUND_LABEL_BGM_resultMake,	// BGM1
	SOUND_LABEL_BGM_game,		// BGM2
	SOUND_LABEL_BGM_title,		// BGM3

	SOUND_LABEL_SE_select1,	// �I���P
	SOUND_LABEL_SE_select2 ,// �I���Q

	SOUND_LABEL_SE_wana1,// 㩔����^�C�v�P 
	SOUND_LABEL_SE_wana2,// 㩔����^�C�v�Q
	SOUND_LABEL_SE_wana3,// 㩔����^�C�v�R



	SOUND_LABEL_MAX,
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
bool InitSound(HWND hWnd);
void UninitSound(void);
void PlaySound(int label);
void StopSound(int label);
void StopSound(void);

