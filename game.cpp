//=============================================================================
//
// ゲーム処理 [game.cpp]
// Author : masayasu wakita
//
//=============================================================================
#include "game.h"
#include "fade.h"
#include "renderer.h"
#include "scene.h"
#include "manager.h"
#include "inputKeyboard.h"
#include "meshField.h"
#include "player.h"
#include "scene3D.h"
#include "meshSphere.h"
#include "meshCube.h"
#include "meshCapsule.h"
#include "enemy.h"
#include "sky.h"
#include "tree.h"
#include "object.h"
#include "circle.h"
#include "stage.h"
#include "meshOrbit.h"
#include "effect.h"
#include "tree.h"
#include "number.h"
#include "time.h"
#include "result.h"
#include "ui.h"
#include "network.h"
#include "wall.h"
#include "speed.h"
#include "GuideSign.h"
#include "puzzle.h"
#include "network.h"
#include "startSignal.h"
#include "shadow.h"
#include "finishUi.h"
#include "characterSelect.h"
#include "camera.h"
#include "counter.h"

//=============================================================================
// 静的メンバ変数
//=============================================================================
CMeshField	*CGame::m_pMeshField = NULL;		// メッシュフィールドのポインタ
CPlayer		*CGame::m_pPlayer = NULL;			// プレイヤーのポインタ
CMeshSphere *CGame::m_pMeshSphere = NULL;		// メッシュスフィアのポインタ
CEnemy		*CGame::m_pEnemy = NULL;			// 敵のポインタ
CSky		*CGame::m_pSky = NULL;				// 空のポインタ
CHouse		*CGame::m_pHouse = NULL;			// 家のポインタ
CSpeed		*CGame::m_pSpeed = NULL;			// 時速のポインタ

CUi			*CGame::m_pUi = NULL;				// UIのポインタ
CTime		*CGame::m_pTime = NULL;				// カウンタのポインタ
CUi			*CGame::m_pLaps = NULL;				// ラップ数のポインタ

//=============================================================================
// コンストラクタ
//=============================================================================
CGame::CGame()
{

}

//=============================================================================
// デストラクタ
//=============================================================================
CGame::~CGame()
{

}

//=============================================================================
//Init処理
//=============================================================================
HRESULT CGame::Init(void)
{
	CCamera *pCamera = CManager::GetCamera();

	if (pCamera != NULL)
	{
		pCamera->SetStoker(true);
	}

	// エフェクトの生成
	CEffect::Create();
	m_bRate = false;
	// プレイヤーの生成
	m_pPlayer = NULL;
	m_pPlayer = CPlayer::Create();

	if (m_pPlayer != NULL)
	{
		// プレイヤーモデル情報の読み込み
		switch (CCharacterSelect::GetCarType())
		{
		case 0:
			m_pPlayer->LoadScript(SCRIPT_CAR01, CPlayer::ANIMATIONTYPE_MAX);
			break;
		case 1:
			m_pPlayer->LoadScript(SCRIPT_CAR02, CPlayer::ANIMATIONTYPE_MAX);
			break;
		case 2:
			m_pPlayer->LoadScript(SCRIPT_CAR03, CPlayer::ANIMATIONTYPE_MAX);
			break;
		case 3:
			m_pPlayer->LoadScript(SCRIPT_CAR04, CPlayer::ANIMATIONTYPE_MAX);
			break;
		}
	}

	// 空の作成
	m_pSky = CSky::Create();

	CObject::LoadScript();

	// 各種アセットの生成＆設置
	//CMeshField::LoadRand("data/stage/rand.csv", false);			// 床情報の読込
	//CObject::LoadModel("data/stage/object.csv");					// モデル情報の読込
	//CEnemy::LoadEnemy("data/stage/enemy.csv");					// 敵情報の読込

	// 時間のクリエイト処理
	CTime::Create();

	// 時速の生成
	CSpeed::Create();

	//// モデル情報の読み込み
	//CObject::LoadModelTest("data/text/stage/stage_1.txt");

	// モデル情報の読み込み
	CObject::LoadModelTest("data/text/model.txt");

	// 内壁情報の読み込み
	CMeshWall::LoadWall("data/text/stage/wall.txt", false);
	// 外壁情報の読み込み
	CMeshWall::LoadWall("data/text/stage/wall02.txt", false);

	int nCntPiece = CPuzzle::GetPieceNum();

	for (int nCnt = 0; nCnt < nCntPiece; nCnt++)
	{
		m_bGuideSign[nCnt] = false;
	}
	for (int nCnt = 0; nCnt < nCntPiece; nCnt++)
	{
		m_bGuideSign[nCnt] = CPuzzle::GetRoute(nCnt);
		if (m_bGuideSign[nCnt] == true)
		{
			m_bRate = true;
		}
	}
	if (m_bRate == true)
	{
		// 案内矢印の生成
		CGuideSign::Create();
	}

	// ネットワークでのゲーム時初期化処理
	CManager::GetNetwork()->InitGame();

	m_pLaps = CUi::Create();

	if (m_pLaps != NULL)
	{
		m_pLaps->LoadScript("data/text/ui/UI_Laps.txt");

		// 周回回数の最大値を設定
		CCounter *pCounter = m_pLaps->GetCounter("MaxLaps");
		if (pCounter != NULL)
		{
			pCounter->SetNumber(3);
		}

		// 周回回数の現在値を設定
		pCounter = m_pLaps->GetCounter("NowLaps");
		if (pCounter != NULL)
		{
			pCounter->SetNumber(1);
		}

		// UIの位置を設定
		m_pLaps->SetPosition(D3DXVECTOR3(382.0f, 682.0f, 0.0f));
	}

	return S_OK;
}

//=============================================================================
// Update関数
//=============================================================================
void CGame::Update(void)
{
	CNetwork *pNetwork = CManager::GetNetwork();

	if (pNetwork != NULL)
	{
		pNetwork->Create();
	}

#ifdef _DEBUG
	CInputKeyboard *pKeyboard = CManager::GetInputKeyboard();

	if (pKeyboard->GetTriggerKeyboard(DIK_0))
	{
		CStartSignal::Create();
	}
#endif
}

//=============================================================================
// Draw関数
//=============================================================================
void CGame::Draw(void)
{

}

//=============================================================================
// Uninit関数
//=============================================================================
void CGame::Uninit(void)
{
	CNetwork *pNetwork = CManager::GetNetwork();

	CObject::ReleaseCheckPoint();

	if (pNetwork != NULL)
	{// ネットワークが存在していたとき
		pNetwork->StopUpdate();				// 更新停止予約
		pNetwork->CloseTCP();				// サーバーとの窓口を閉める
	}

	CObject::Unload();

	// ポリゴンの開放
	CScene::ReleaseAll();
}

//=============================================================================
// アセットの読み込み
//=============================================================================
void CGame::LoadAsset(void)
{
	// =============== アセットの読み込み ===============//
	CPlayer::Load();
	CSky::Load();
	CMeshField::Load();
	CMeshOrbit::Load();
	CEnemy::Load();
	CObject::Load();
	CMeshSphere::Load();
	CEffect::Load();
	CNumber::Load();
	CGuideSign::Load();
	CShadow::Load();
}