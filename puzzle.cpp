//=============================================================================
//
// puzzle���� [puzzle.cpp]
// Author : masayasu wakita
//
//=============================================================================
#include "puzzle.h"
#include "fade.h"
#include "renderer.h"
#include "scene.h"
#include "manager.h"
#include "inputKeyboard.h"
#include "inputController.h"
#include "network.h"
#include "pieceSelect.h"
#include "ui.h"
#include "sound.h"

//=============================================================================
// �ÓI�����o�ϐ�
//=============================================================================
float CPuzzle::m_fSpeed[Piece_Num] = {};
float CPuzzle::m_fRate[Piece_Num] = {};
float CPuzzle::m_fTurning[Piece_Num] = {};
float CPuzzle::m_fDecay[Piece_Num] = {};
int CPuzzle::m_nPower[Piece_Num] = {};
bool CPuzzle::m_bRoute[Piece_Num] = {};
bool CPuzzle::m_bRank[Piece_Num] = {};
int CPuzzle::m_nPieceNum = 0;

//=============================================================================
// �R���X�g���N�^
//=============================================================================
CPuzzle::CPuzzle()
{
	m_pPieceSelect = NULL;
}

//=============================================================================
// �f�X�g���N�^
//=============================================================================
CPuzzle::~CPuzzle()
{

}

//=============================================================================
//Init����
//=============================================================================
HRESULT CPuzzle::Init(void)
{
	LoadAsset();

	CUi *pUi = CUi::Create();

	if (pUi != NULL)
	{
		pUi->LoadScript("data/text/ui/puzzleUI.txt");
	}

	CBox::Create();

	m_pPieceSelect = CPieceSelect::Create();

	// �e��A�Z�b�g�̐������ݒu
	//CMeshField::LoadRand("data/stage/rand.csv", false);				// �����̓Ǎ�
	//CObject::LoadModel("data/stage/object.csv");						// ���f�����̓Ǎ�
	//CEnemy::LoadEnemy("data/stage/enemy.csv");						// �G���̓Ǎ�

	return S_OK;
}

//=============================================================================
// Update�֐�
//=============================================================================
void CPuzzle::Update(void)
{
	CInputKeyboard *pInputKeyboard = CManager::GetInputKeyboard();
	CInputController *pInputController = CManager::GetInputController();
	CNetwork *pNetwork = CManager::GetNetwork();

	m_nPieceNum = m_pPieceSelect->GetPieceNum();

	if (m_pPieceSelect != NULL)
	{
		for ( int nCnt = 0;nCnt < m_nPieceNum; nCnt++)
		{
			m_fSpeed[nCnt] = m_pPieceSelect->GetSpeed(nCnt);
			m_fRate[nCnt] = m_pPieceSelect->GetRate(nCnt);
			m_fTurning[nCnt] = m_pPieceSelect->GetTurning(nCnt);
			m_fDecay[nCnt] = m_pPieceSelect->GetDecay(nCnt);
			m_nPower[nCnt] = m_pPieceSelect->GetPower(nCnt);
			m_bRoute[nCnt] = m_pPieceSelect->GetRoute(nCnt);
			m_bRank[nCnt] = m_pPieceSelect->GetRank(nCnt);
		}
	}

	if (CFade::GetFade() == CFade::FADE_NONE)
	{//�t�F�[�h�����������Ă��Ȃ��Ƃ�
		if (pInputKeyboard != NULL)
		{// �L�[�{�[�h�����݂��Ă����Ƃ�
			if (pInputKeyboard->GetTriggerKeyboard(DIK_RETURN))
			{// �w��̃L�[�������ꂽ�Ƃ�
				if (pNetwork != NULL)
				{
					if (pNetwork->Connect() == S_OK)
					{
						CSound *pSound = CManager::GetSound();				// �T�E���h�̎擾

						pSound->PlaySoundA(SOUND_LABEL_SE_PuzzleComplete_1);			// �_���[�W���̍Đ�

						CFade::SetFade(CManager::MODE_GAME, CFade::FADETYPE_SLIDE);					//�t�F�[�h������
					}
				}
			}
		}
		if (pInputController->GetJoypadUse(0))
		{// �R���g���[���[����������Ă���Ƃ�
		 //�Q�[���̑J��
			if (pInputController->GetControllerTrigger(0, JOYPADKEY_START))			// �Q�[���p�b�h��START�{�^���������ꂽ�Ƃ�
			{
				if (pNetwork != NULL)
				{
					if (pNetwork->Connect() == S_OK)
					{
						CSound *pSound = CManager::GetSound();				// �T�E���h�̎擾

						pSound->PlaySoundA(SOUND_LABEL_SE_PuzzleComplete_1);			// �_���[�W���̍Đ�
						CFade::SetFade(CManager::MODE_GAME, CFade::FADETYPE_SLIDE);			//�t�F�[�h������

					}
				}
			}
		}
	}
}

//=============================================================================
// Draw�֐�
//=============================================================================
void CPuzzle::Draw(void)
{

}

//=============================================================================
// Uninit�֐�
//=============================================================================
void CPuzzle::Uninit(void)
{
	if (m_pPieceSelect != NULL)
	{
		m_pPieceSelect->Uninit();
		m_pPieceSelect->Release();
		m_pPieceSelect = NULL;
	}

	// �|���S���̊J��
	CScene::ReleaseAll();
}

//=============================================================================
// �A�Z�b�g�̓ǂݍ���
//=============================================================================
void CPuzzle::LoadAsset(void)
{
	CBox::Load();
}