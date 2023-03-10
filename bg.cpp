//=============================================================================
//
// BG処理 [bg.cpp]
// Author : 
//
//=============================================================================
#include "bg.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(8000)			// (SCREEN_WIDTH)	// 背景サイズ
#define TEXTURE_HEIGHT				(1080)			// (SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(4)				// テクスチャの数

		
#define TILE_DRAW_SIZE		(10)
//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/yuka01.jpg",
	"data/TEXTURE/sky000.jpg",
	"data/TEXTURE/sky001.jpg",
	"data/TEXTURE/gokiburi_hoihoi.png",
};


static BOOL	g_Load = FALSE;		// 初期化を行ったかのフラグ
static BG	g_BG[SCREEN_HEIGHT /TILE_SIZE][SCREEN_WIDTH / TILE_SIZE];



//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitBG(void)
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

	for (int y = 0; y < SCREEN_HEIGHT / TILE_SIZE; y++) {
		for (int x = 0; x < SCREEN_WIDTH / TILE_SIZE; x++) {
			g_BG[y][x].spriteId = 0;
			{
				//if (y == 0) {
				//	g_BG[y][x].spriteId = 1;
				//}
				//if (x == 0) {
				//	g_BG[y][x].spriteId = 1;
				//}
				//if (y == (SCREEN_HEIGHT / TILE_SIZE) - 1) {
				//	g_BG[y][x].spriteId = 1;
				//}
				//if (x == (SCREEN_WIDTH / TILE_SIZE) - 1) {
				//	g_BG[y][x].spriteId = 1;
				//}
			}
		}
	}


	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitBG(void)
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
void UpdateBG(void)
{



	//g_BG.scrl -= 0.0f;		// 0.005f;		// スクロール


#ifdef _DEBUG	// デバッグ情報を表示する


#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawBG(void)
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

	for (int y = 0; y < SCREEN_HEIGHT / TILE_SIZE; y++) {
		for (int x = 0; x < SCREEN_WIDTH / TILE_SIZE; x++) {
			BG* bg = GetBG();

			
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteLTColor(g_VertexBuffer,
				TILE_SIZE * x, TILE_SIZE * y, TILE_SIZE, TILE_SIZE,
				0.0f, 0.0f, 1.0f, 1.0f,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);

			if (g_BG[y][x].spriteId != 0) {

				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_BG[y][x].spriteId]);

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteLTColor(g_VertexBuffer,
					TILE_SIZE * x, TILE_SIZE * y, TILE_SIZE, TILE_SIZE,
					0.0f, 0.0f, 1.0f, 1.0f,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}

		}
	}

}


//=============================================================================
// BG構造体の先頭アドレスを取得
//=============================================================================
BG* GetBG()
{
	return &g_BG[0][0];
}

void SetBG(int x, int y, int id) {
	g_BG[y][x].spriteId = id;
}



