//=============================================================================
//
// �G�l�~�[���� [enemy.h]
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
#define ENEMY_MAX		(2)		// �G�l�~�[��Max�l��


//*****************************************************************************
// �\���̒�`
//*****************************************************************************
enum dir {
	UP,
	RIGHT,
	DOWN,
	LEFT
};

struct ENEMY
{
	BOOL		use;			// true:�g���Ă���  false:���g�p
	bool		isTrapped;		// true:�����Ȃ�  false:������
	int			Dir;			//	�����Ă������
	int			x, y;			//	�}�b�v��̍��W
	int			distance;
	int			targetID;		// �������Ă���G�l�~�[�̔ԍ�
	float		countAnim;		// �A�j���[�V�����J�E���g
	int			patternAnim;	// �A�j���[�V�����p�^�[���i���o�[
	int			enemyType;		// �G�l�~�[�ԍ�
	int			wait;
};




//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitEnemy(void);
void UninitEnemy(void);
void UpdateEnemy(void);
void DrawEnemy(void);
int GetMDistance(int x1, int y1, int x2, int y2);
ENEMY* GetEnemy(void);



