//=============================================================================
//
// BG���� [bg.h]
// Author : 
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "debugproc.h"
#include "sprite.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
struct BG
{
	int spriteId;
};

#define TILE_SIZE			(60)	
#define TILE_AMOUNT_W		(SCREEN_WIDTH /TILE_SIZE)
#define TILE_AMOUNT_H		(SCREEN_HEIGHT /TILE_SIZE)
#define	SKY_H	(150.0f)	// ��̃|���S���̍���


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitBG(void);
void UninitBG(void);
void UpdateBG(void);
void DrawBG(void);

BG* GetBG(void);
void SetBG(int x, int y, int id);



