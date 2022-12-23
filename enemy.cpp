//=============================================================================
//
// エネミー処理 [enemy.cpp]
// Author : 
//
//=============================================================================
#include "enemy.h"
#include "bg.h"
#include "player.h"
#include "fade.h"
#include "collision.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(200/2)	// キャラサイズ
#define TEXTURE_HEIGHT				(200/2)	// 
#define TEXTURE_MAX					(2)		// テクスチャの数

#define TEXTURE_PATTERN_DIVIDE_X	(2)		// アニメパターンのテクスチャ内分割数（X)
#define TEXTURE_PATTERN_DIVIDE_Y	(2)		// アニメパターンのテクスチャ内分割数（Y)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// アニメーションパターン数
#define ANIM_WAIT					(4)		// アニメーションの切り替わるWait値


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/enemy00.png",
	"data/TEXTURE/bar_white.png",
};


static BOOL		g_Load = FALSE;			// 初期化を行ったかのフラグ
static ENEMY	g_Enemy[ENEMY_MAX];		// エネミー構造体

static int		g_EnemyCnt = ENEMY_MAX;


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitEnemy(void)
{
	ID3D11Device *pDevice = GetDevice();

	//テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TexturName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}


	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);


	// エネミー構造体の初期化
	g_EnemyCnt = 0;
	for (int i = 0; i < ENEMY_MAX; i++)
	{
		g_Enemy[i].use = TRUE;

		g_Enemy[i].enemyType = 0;

		g_Enemy[i].countAnim = 0;
		g_Enemy[i].patternAnim = 0;

		g_EnemyCnt++;
	}

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitEnemy(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (g_Texture[i])
		{
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}

	g_Load = FALSE;
}

int GetMDistance(int x1, int y1, int x2, int y2) {
	return abs(x1 - x2) + abs(y1 - y2);
}
//=============================================================================
// 更新処理
//=============================================================================
void UpdateEnemy(void)
{
	g_EnemyCnt = 0;	// 生きてるエネミーの数
	BG* bg = GetBG();
	for (int i = 0; i < ENEMY_MAX; i++)
	{
		// 生きてるエネミーだけ処理をする
		if (g_Enemy[i].use == TRUE && !g_Enemy[i].isTrapped)
		{
			g_EnemyCnt++;
			// アニメーション  
			g_Enemy[i].countAnim += 1.0f;
			if (g_Enemy[i].countAnim > ANIM_WAIT)
			{
				g_Enemy[i].countAnim = 0.0f;
				// パターンの切り替え
				g_Enemy[i].patternAnim = (g_Enemy[i].patternAnim + 1) % ANIM_PATTERN_NUM;
			}

			switch (g_Enemy[i].enemyType) {
			case 0:		//時計回りに座標確認
				if (g_Enemy[i].Dir != DOWN && g_Enemy[i].y > 0 && bg[(g_Enemy[i].y - 1) * TILE_SIZE + g_Enemy[i].x].spriteId != 1) {
					g_Enemy[i].y--;
					g_Enemy[i].Dir = UP;
					break;
				}
				if (g_Enemy[i].Dir != LEFT && !bg[(g_Enemy[i].y) * TILE_SIZE + g_Enemy[i].x + 1].spriteId == 1 && g_Enemy[i].x < TILE_SIZE - 1) {
					g_Enemy[i].x++;
					g_Enemy[i].Dir = RIGHT;
					break;
				}
				if (g_Enemy[i].Dir != UP && !bg[(g_Enemy[i].y + 1) * TILE_SIZE + g_Enemy[i].x + 1].spriteId == 1 && g_Enemy[i].y < TILE_SIZE - 1) {
					g_Enemy[i].y++;
					g_Enemy[i].Dir = DOWN;
					break;
				}
				if (g_Enemy[i].Dir != RIGHT && !bg[(g_Enemy[i].y) * TILE_SIZE + g_Enemy[i].x - 1].spriteId == 1 && g_Enemy[i].x > 0) {
					g_Enemy[i].x--;
					g_Enemy[i].Dir = LEFT;
					break;
				}
				g_Enemy[i].Dir = (g_Enemy[i].Dir + 2) % 4;
				g_Enemy[i].x -= (g_Enemy[i].Dir - 2) % 2;
				g_Enemy[i].y -= (g_Enemy[i].Dir - 1) % 2;
				break;

			case 1:		//相手から遠い軸から処理
				if (g_Enemy[i].targetID == i || g_Enemy[g_Enemy[i].targetID].use == false) {
					for (int e = 0; e < ENEMY_MAX; e++) {
						if (g_Enemy[e].enemyType != g_Enemy[i].enemyType && g_Enemy[e].use == true) {
							if (g_Enemy[i].distance > GetMDistance(g_Enemy[i].x, g_Enemy[i].y, g_Enemy[e].x, g_Enemy[e].y)) {
								if (GetMDistance(g_Enemy[i].x, g_Enemy[i].y, g_Enemy[e].x, g_Enemy[e].y) == 0) {
									//endgame
								}
								g_Enemy[i].targetID = e;
							}
						}
					}
				}
				if (abs(g_Enemy[i].x - g_Enemy[g_Enemy[i].targetID].x) < abs(g_Enemy[i].y - g_Enemy[g_Enemy[i].targetID].y)) {
					if (g_Enemy[i].Dir != DOWN && g_Enemy[i].y - g_Enemy[g_Enemy[i].targetID].y < 0) {
						g_Enemy[i].y--;
						g_Enemy[i].Dir = UP;
						break;
					}
					else if (g_Enemy[i].Dir != UP) {
						g_Enemy[i].y++;
						g_Enemy[i].Dir = DOWN;
						break;

					}
				}
				else {
					if (g_Enemy[i].Dir != LEFT && g_Enemy[i].x - g_Enemy[g_Enemy[i].targetID].x < 0) {
						g_Enemy[i].x++;
						g_Enemy[i].Dir = RIGHT;
						break;
					}
					else if (g_Enemy[i].Dir != RIGHT) {
						g_Enemy[i].x--;
						g_Enemy[i].Dir = LEFT;
						break;
					}
				}
				g_Enemy[i].Dir = (g_Enemy[i].Dir + 2) % 4;
				g_Enemy[i].x -= (g_Enemy[i].Dir - 2) % 2;
				g_Enemy[i].y -= (g_Enemy[i].Dir - 1) % 2;
				break;
			default:
				break;
			}



			// 移動が終わったらエネミーとの当たり判定
			{
				if (bg[(g_Enemy[i].y) * TILE_SIZE + g_Enemy[i].x + 1].spriteId = 2) {
					g_Enemy[i].isTrapped = true;
				}
			}
		}

	}
	// エネミー全滅チェック
	/*if (g_EnemyCnt <= 0)
	{
		SetFade(FADE_OUT, MODE_RESULT);
	}*/

#ifdef _DEBUG	// デバッグ情報を表示する


#endif
	
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawEnemy(void)
{
	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// マトリクス設定
	SetWorldViewProjection2D();

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	//BG* bg = GetBG();

	for (int i = 0; i < ENEMY_MAX; i++)
	{
		if (g_Enemy[i].use == TRUE)			// このエネミーが使われている？
		{									// Yes
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Enemy[i].enemyType]);

			//エネミーの位置やテクスチャー座標を反映
			float px = g_Enemy[i].x;	// エネミーの表示位置X
			float py = g_Enemy[i].y;	// エネミーの表示位置Y
			float pw = TILE_SIZE;		// エネミーの表示幅
			float ph = TILE_SIZE;		// エネミーの表示高さ

			// アニメーション用
			float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// テクスチャの幅
			float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// テクスチャの高さ
			float tx = (float)(g_Enemy[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// テクスチャの左上X座標
			float ty = (float)(g_Enemy[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// テクスチャの左上Y座標

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteLeftTop(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
	}




}


//=============================================================================
// Enemy構造体の先頭アドレスを取得
//=============================================================================
ENEMY* GetEnemy(void)
{
	return &g_Enemy[0];
}





