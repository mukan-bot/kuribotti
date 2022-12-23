//=============================================================================
//
// �G�l�~�[���� [enemy.cpp]
// Author : 
//
//=============================================================================
#include "enemy.h"
#include "bg.h"
#include "player.h"
#include "fade.h"
#include "collision.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(200/2)	// �L�����T�C�Y
#define TEXTURE_HEIGHT				(200/2)	// 
#define TEXTURE_MAX					(2)		// �e�N�X�`���̐�

#define TEXTURE_PATTERN_DIVIDE_X	(2)		// �A�j���p�^�[���̃e�N�X�`�����������iX)
#define TEXTURE_PATTERN_DIVIDE_Y	(2)		// �A�j���p�^�[���̃e�N�X�`�����������iY)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// �A�j���[�V�����p�^�[����
#define ANIM_WAIT					(4)		// �A�j���[�V�����̐؂�ւ��Wait�l


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/enemy00.png",
	"data/TEXTURE/bar_white.png",
};


static BOOL		g_Load = FALSE;			// ���������s�������̃t���O
static ENEMY	g_Enemy[ENEMY_MAX];		// �G�l�~�[�\����

static int		g_EnemyCnt = ENEMY_MAX;


//=============================================================================
// ����������
//=============================================================================
HRESULT InitEnemy(void)
{
	ID3D11Device *pDevice = GetDevice();

	//�e�N�X�`������
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


	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);


	// �G�l�~�[�\���̂̏�����
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
// �I������
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
// �X�V����
//=============================================================================
void UpdateEnemy(void)
{
	g_EnemyCnt = 0;	// �����Ă�G�l�~�[�̐�
	BG* bg = GetBG();
	for (int i = 0; i < ENEMY_MAX; i++)
	{
		// �����Ă�G�l�~�[��������������
		if (g_Enemy[i].use == TRUE && !g_Enemy[i].isTrapped)
		{
			g_EnemyCnt++;
			// �A�j���[�V����  
			g_Enemy[i].countAnim += 1.0f;
			if (g_Enemy[i].countAnim > ANIM_WAIT)
			{
				g_Enemy[i].countAnim = 0.0f;
				// �p�^�[���̐؂�ւ�
				g_Enemy[i].patternAnim = (g_Enemy[i].patternAnim + 1) % ANIM_PATTERN_NUM;
			}

			switch (g_Enemy[i].enemyType) {
			case 0:		//���v���ɍ��W�m�F
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

			case 1:		//���肩�牓�������珈��
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



			// �ړ����I�������G�l�~�[�Ƃ̓����蔻��
			{
				if (bg[(g_Enemy[i].y) * TILE_SIZE + g_Enemy[i].x + 1].spriteId = 2) {
					g_Enemy[i].isTrapped = true;
				}
			}
		}

	}
	// �G�l�~�[�S�Ń`�F�b�N
	/*if (g_EnemyCnt <= 0)
	{
		SetFade(FADE_OUT, MODE_RESULT);
	}*/

#ifdef _DEBUG	// �f�o�b�O����\������


#endif
	
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawEnemy(void)
{
	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// �}�g���N�X�ݒ�
	SetWorldViewProjection2D();

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// �}�e���A���ݒ�
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	//BG* bg = GetBG();

	for (int i = 0; i < ENEMY_MAX; i++)
	{
		if (g_Enemy[i].use == TRUE)			// ���̃G�l�~�[���g���Ă���H
		{									// Yes
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Enemy[i].enemyType]);

			//�G�l�~�[�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = g_Enemy[i].x;	// �G�l�~�[�̕\���ʒuX
			float py = g_Enemy[i].y;	// �G�l�~�[�̕\���ʒuY
			float pw = TILE_SIZE;		// �G�l�~�[�̕\����
			float ph = TILE_SIZE;		// �G�l�~�[�̕\������

			// �A�j���[�V�����p
			float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// �e�N�X�`���̕�
			float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// �e�N�X�`���̍���
			float tx = (float)(g_Enemy[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// �e�N�X�`���̍���X���W
			float ty = (float)(g_Enemy[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// �e�N�X�`���̍���Y���W

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteLeftTop(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
	}




}


//=============================================================================
// Enemy�\���̂̐擪�A�h���X���擾
//=============================================================================
ENEMY* GetEnemy(void)
{
	return &g_Enemy[0];
}





