//=============================================================================
//
// エネミー処理 [enemy.h]
// Author : 
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "debugproc.h"
#include "sprite.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define ENEMY_MAX		(2)		// エネミーのMax人数


//*****************************************************************************
// 構造体定義
//*****************************************************************************
enum dir {
	UP,
	RIGHT,
	DOWN,
	LEFT
};

struct ENEMY
{
	BOOL		use;			// true:使っている  false:未使用
	bool		isTrapped;		// true:動けない  false:動ける
	int			Dir;			//	向いている方向
	int			x, y;			//	マップ上の座標
	int			distance;
	int			targetID;		// 向かっているエネミーの番号
	float		countAnim;		// アニメーションカウント
	int			patternAnim;	// アニメーションパターンナンバー
	int			enemyType;		// エネミー番号
	int			wait;
};




//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitEnemy(void);
void UninitEnemy(void);
void UpdateEnemy(void);
void DrawEnemy(void);
int GetMDistance(int x1, int y1, int x2, int y2);
ENEMY* GetEnemy(void);



