//=============================================================================
//
// タイトル画面処理 [title.cpp]
// Author : 
//
//=============================================================================
#include "title.h"
#include "input.h"
#include "fade.h"
#include "sound.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// 背景サイズ
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(4)				// テクスチャの数

#define TEXTURE_WIDTH_LOGO			(480)			// ロゴサイズ
#define TEXTURE_HEIGHT_LOGO			(80)			// 
#define TEXTURE_PATTERN_MENU_X	(1)		// モードのテクスチャ内分割数（X)
#define TEXTURE_PATTERN_MENU_Y	(3)		// モードのテクスチャ内分割数（Y)

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/bg000.jpg",
	"data/TEXTURE/title.png",
	"data/TEXTURE/effect000.jpg",
	"data/TEXTURE/title_logo1.png",

};


static BOOL						g_Use;						// TRUE:使っている  FALSE:未使用
static float					g_w, g_h;					// 幅と高さ
static XMFLOAT3					g_Pos;						// ポリゴンの座標
static int						g_TexNo;					// テクスチャ番号

float	alpha;
BOOL	flag_alpha;

static BOOL						g_Load = FALSE;

static float	effect_dx;
static float	effect_dy;

static int		g_CheckMode = 0;		// モードの選択初期化

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitTitle(void)
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


	// 変数の初期化
	g_Use   = TRUE;
	g_w     = TEXTURE_WIDTH;
	g_h     = TEXTURE_HEIGHT;
	g_Pos   = XMFLOAT3(g_w/2, g_h/2, 0.0f);
	g_TexNo = 0;

	alpha = 1.0f;
	flag_alpha = TRUE;

	effect_dx = 100.0f;
	effect_dy = 100.0f;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitTitle(void)
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

//=============================================================================
// 更新処理
//=============================================================================
void UpdateTitle(void)
{
	if (g_CheckMode == TITLE_MODE_MAX)
	{
		g_CheckMode = 0;
	}
	//上の制限がなかったので追加
	if (g_CheckMode == -1) {
		g_CheckMode = TITLE_MODE_QUIT;
	}

	if (GetKeyboardTrigger(DIK_DOWN) || IsButtonPressed(0, BUTTON_DOWN))
	{
		g_CheckMode++;
		PlaySound(SOUND_LABEL_SE_select1);
	}

	if (GetKeyboardTrigger(DIK_UP) || IsButtonPressed(0, BUTTON_UP))
	{
		g_CheckMode--;
		PlaySound(SOUND_LABEL_SE_select1);
	}

	switch (g_CheckMode)
	{
	case 0:		
	{
		if (GetKeyboardTrigger(DIK_RETURN))
		{
			PlaySound(SOUND_LABEL_SE_select2);
			SetFade(FADE_OUT, MODE_GAME);
		}
		break;
	}

	case 1:		
	{
		if (GetKeyboardTrigger(DIK_RETURN))
		{
			PlaySound(SOUND_LABEL_SE_select2);
			exit(-1);
		}
		break;
	}
	}

	// ゲームパッドで入力処理
	if (IsButtonTriggered(0, BUTTON_START))
	{
		PlaySound(SOUND_LABEL_SE_select2);
		SetFade(FADE_OUT, MODE_GAME);
	}
	else if (IsButtonTriggered(0, BUTTON_B))
	{
		PlaySound(SOUND_LABEL_SE_select2);
		SetFade(FADE_OUT, MODE_GAME);
	}



	// セーブデータをロードする？
	if (GetKeyboardTrigger(DIK_L))
	{
		SetLoadGame(TRUE);
		SetFade(FADE_OUT, MODE_GAME);
	}



#ifdef _DEBUG	// デバッグ情報を表示する
	//PrintDebugProc("Player:↑ → ↓ ←　Space\n");

#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawTitle(void)
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

	// タイトルの背景を描画
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLeftTop(g_VertexBuffer, 0.0f, 0.0f, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	// メニュー選択
	{
		float tw = 1.0f / TEXTURE_PATTERN_MENU_X;	// テクスチャの幅
		float th = 1.0f / TEXTURE_PATTERN_MENU_Y;	// テクスチャの高さ
		float tx = 0.0f;	// テクスチャの左上X座標
		float ty = 0.0f;	// テクスチャの左上Y座標



		// ゲームスタートボタン
		{
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[3]);
				SetSpriteColor(g_VertexBuffer,
				SCREEN_CENTER_X, SCREEN_CENTER_Y, 150.0f, 80.0f,
				tx, ty, tw, th,
				XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f));
			GetDeviceContext()->Draw(4, 0);
		}

		// おわるボタン
		{
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[3]);
				SetSpriteColor(g_VertexBuffer,
				SCREEN_CENTER_X, SCREEN_CENTER_Y + 80.0f * 2, 150.0f, 80.0f,
				tx, ty + (th * 2), tw, th,
				XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f));
			GetDeviceContext()->Draw(4, 0);
			}

		if (g_CheckMode == 0)		// ゲームスタートを選択中
		{
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[3]);
				SetSpriteColor(g_VertexBuffer,
				SCREEN_CENTER_X, SCREEN_CENTER_Y, 150.0f, 80.0f,
				tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
			GetDeviceContext()->Draw(4, 0);
			}

		if (g_CheckMode == 1)		// おわるを選択中
		{
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[3]);
				SetSpriteColor(g_VertexBuffer,
				SCREEN_CENTER_X, SCREEN_CENTER_Y + 80.0f * 2, 150.0f, 80.0f,
				tx, ty + (th * 2), tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
			GetDeviceContext()->Draw(4, 0);
		}
	}

}




