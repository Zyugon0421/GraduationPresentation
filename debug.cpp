//=============================================================================
//
// デバッグ処理 [debug.cpp]
// Author : masayasu wakita
//
//=============================================================================
#include "manager.h"
#include "renderer.h"
#include "inputMouse.h"
#include "camera.h"
#include "scene.h"
#include "circle.h"
#include "inputKeyboard.h"
#include "meshField.h"
#include "game.h"
#include "object.h"
#include "sceneX.h"
#include "collider.h"
#include "enemy.h"
#include "inputMouse.h"
#include "effect.h"
#include "player.h"
#include "wall.h"
#include "ui.h"
#include "effect.h"
#include "write.h"

//=============================================================================
// マクロ定義
//=============================================================================
#define UIMANAGER_SCRIPT "data/text/manager/manager_ui.txt"
#define CONTINUE_MAX 10													// 再計算回数

//=============================================================================
// 静的メンバ変数
//=============================================================================
char CDebugProc::m_aStr[1024] = {};										// 文字列
D3DXVECTOR3 CDebugProc::m_mouseOld = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// マウスの前のワールド座標
CCircle	*CDebugProc::m_pCircle = NULL;									// 円のポインタ
CObject	*CDebugProc::m_pSample = NULL;									// 木のポインタ
CScene *CDebugProc::m_pSelect = NULL;									// 選択したシーンのポインタ
CEnemy *CDebugProc::m_pEnemy = NULL;									// 敵のポインタ
float CDebugProc::m_fPaintSize = 1;										// 地形編集ブラシの大きさ
int CDebugProc::m_nCreateIndex = 0;										// 1フレームの生成数
bool CDebugProc::m_bDebug = false;										// デバッグモードの切替
bool CDebugProc::m_bInspectorWind = false;								// インスペクターウィンドウの表示切替
bool CDebugProc::m_bMouseCursorPosition = false;						// マウスカーソル座標の使用フラグ
int CDebugProc::m_nCntGeneration = 0;									// 生成数
int	CDebugProc::m_nMode = 0;											// モード選択
int CDebugProc::m_nParticleShape = PARTICLESHAPE_CONE;					// パーティクル形状
int	CDebugProc::m_nCntContinue = 0;										// 再確認回数
D3DXVECTOR2 CDebugProc::m_CreateRand = D3DXVECTOR2(0.0f, 0.0f);			// 床の量
D3DXVECTOR2 CDebugProc::m_CreateRandOld = D3DXVECTOR2(0.0f, 0.0f);		// 床の量
CMeshField *CDebugProc::m_apMeshField[FLOOR_LIMIT * FLOOR_LIMIT] = {};
D3DXVECTOR3 CDebugProc::m_createPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
float CDebugProc::m_fSliderPow = 1.0f;

std::string CDebugProc::m_currentModel = {};		// モデルのアドレス
std::string CDebugProc::m_currentTexture = {};		// テクスチャのアドレス
std::string CDebugProc::m_currentUi = {};			// UIのアドレス

bool CDebugProc::m_bHeightCalculation = false;
HWND CDebugProc::m_hWnd = NULL;								// ウィンドウハンドル

// エフェクト作成関連
int CDebugProc::m_particleLife = 0;							// パーティクルの生存時間
int CDebugProc::m_nCreate = 0;								// 生成数
int CDebugProc::m_nInterval = 0;							// インターバル
int CDebugProc::m_nEmissionType = 0;

float CDebugProc::m_fStartRadius = 0.0f;					// 始まりの
float CDebugProc::m_fRadius = 0.0f;							// 半径
float CDebugProc::m_fMinSpeed = 0.0f;						// 最低スピード
float CDebugProc::m_fSpeed = 0.0f;							// スピード
float CDebugProc::m_fResistance = 0.0f;						// 抵抗値
float CDebugProc::m_fGravity = 0.0f;

D3DXVECTOR3	CDebugProc::m_createRot = D3DXVECTOR3_ZERO;		// 回転量
D3DXVECTOR3 CDebugProc::m_size = D3DXVECTOR3_ZERO;			// サイズ
D3DXVECTOR3 CDebugProc::m_moveSize = D3DXVECTOR3_ZERO;		// 大きさの変化量
D3DXVECTOR3 CDebugProc::m_moveRot = D3DXVECTOR3_ZERO;		// 回転の変化量
D3DXVECTOR3 CDebugProc::m_centerPos = D3DXVECTOR3(1.0f, 1.0f, 1.0f);	// 中心位置
D3DXVECTOR2 CDebugProc::m_sprite = D3DXVECTOR2(1.0f, 1.0f);	// 分割数
D3DXCOLOR	CDebugProc::m_col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
D3DXCOLOR	CDebugProc::m_moveCol = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);

float CDebugProc::m_fAngle = 0.0f;							// 角度
float CDebugProc::m_fDistance = 0.0f;						// 距離
float CDebugProc::m_fRotationSpeed = 0.0f;					// 回転速度
float CDebugProc::m_fMaxSpeed = 0.0;						// スピードの最大値

bool CDebugProc::m_bLoop = false;							// 生成を繰り返す
bool CDebugProc::m_bRandomSpeed = false;					// スピードランダム化の有無
bool CDebugProc::m_bAlpha = false;							// アルファブレンディングの有無
bool CDebugProc::m_bZBuffer = false;						// Zバッファの有無
bool CDebugProc::m_bFadeOut = false;						// フェードアウトの有無
bool CDebugProc::m_bBillboard = true;						// ビルボードの有無
bool CDebugProc::m_bRandAngle = false;						// 角度のランダム

char CDebugProc::m_effectTag[NAME_SIZE] = {};				// タグの初期化

CUi *CDebugProc::m_pCreateUi = NULL;						// UIの初期化
char CDebugProc::m_CreateName[NAME_SIZE] = {};				// 生成名
std::vector<std::string> CDebugProc::m_AddUi = {};			// アドレスの配列(UI)


//=============================================================================
// コンストラクタ
//=============================================================================
CDebugProc::CDebugProc()
{
	m_pSelect = NULL;
	m_bInspectorWind = false;
}

//=============================================================================
// デストラクタ
//=============================================================================
CDebugProc::~CDebugProc()
{

}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CDebugProc::Init(HWND hWnd)
{
	CRenderer *Renderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice;
	pDevice = Renderer->GetDevice();

	// ウィンドウハンドルを保管
	m_hWnd = hWnd;

	// デバッグ表示用フォントを設定
	D3DXCreateFont(pDevice, 18, 0, 0, 0, FALSE, SHIFTJIS_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "ＭＳ ゴシック", &m_pFont);

	//IMGUIの初期化
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	io.Fonts->AddFontFromFileTTF("data\\fonts\\Meiryo.ttf", 16.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer bindings
	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX9_Init(pDevice);

	// デバッグ用床ポインタの初期化
	for (int nCount = 0; nCount < FLOOR_LIMIT * FLOOR_LIMIT; nCount++)
	{
		m_apMeshField[nCount] = NULL;
	}

	// 円の作成
	m_pCircle = CCircle::Create();

	LoadAddWithUI();
	return S_OK;
}

//=============================================================================
// 開放処理
//=============================================================================
void CDebugProc::Uninit(void)
{
	if (m_pCircle != NULL)
	{// 円のポインタが開放されていないとき
		m_pCircle->Uninit();														// 開放処理
		delete m_pCircle;															// 円の開放
		m_pCircle = NULL;															// 円のポインタをNULLに
	}

	// 床ポインタの開放処理
	for (int nCount = 0; nCount < FLOOR_LIMIT * FLOOR_LIMIT; nCount++)
	{
		if (m_apMeshField[nCount] != NULL)
		{// デバッグ用の床が存在していたとき
			m_apMeshField[nCount] = NULL;
		}
	}
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	if (m_pFont != NULL)
	{// デバッグ表示用フォントの開放
		m_pFont->Release();
		m_pFont = NULL;
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void CDebugProc::Update(void)
{
	// フレーム開始
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

#ifdef _DEBUG
	// 自分で作成した簡単なウィンドウを表示します。 Begin / Endペアを使用して、名前付きウィンドウを作成します。
	ImGui::Begin("System");                          // 「System」というウィンドウを作成しますそれに追加します。

	ImGui::SameLine();
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

	ImGui::Text("rand = %d", CManager::GetRand(10));								// プレイヤーの現在位置を表示

	if (m_pSelect != NULL)
	{
		if (m_bInspectorWind)
		{
			ShowInspector();
		}
	}

	CInputMouse *pMouse = CManager::GetInputMouse();
	CInputKeyboard *pKeyboard = CManager::GetInputKeyboard();
	if (pKeyboard->GetPressKeyboard(DIK_F1))
	{
		if (pMouse->GetTriggerMouse(MOUSE_LEFT))
		{
			SelectModel();
		}
	}

	ImGui::End();
#endif

	Debug();
}

//=============================================================================
// 描画処理
//=============================================================================
void CDebugProc::Draw(void)
{
	CRenderer *pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();	// デバイスの取得

	D3DXVECTOR3 pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	int nTime = 0;

	RECT rect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

	// 円の描画
	if (m_bDebug && m_nMode != DEBUGMODE_INDIVIDUAL)
	{
		pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);				// 裏面(左回り)をカリングする
		m_pCircle->Draw();
		pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);				// 裏面(左回り)をカリングする
	}

	//ImGuiの描画
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

	// テキスト描画
	m_pFont->DrawText(NULL, &m_aStr[0], -1, &rect, DT_LEFT, D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f));
	*m_aStr = NULL;
}

//=============================================================================
// ログ表示処理
//=============================================================================
void CDebugProc::Log(char* frm, ...)
{
	va_list args;			// リストの取得
	char* c = "\n";			// 改行用

	va_start(args, frm);		// リストの先頭を取得

	vsprintf(&m_aStr[strlen(m_aStr)], frm, args);		// 文字に当てはめる

	va_end(args);						// リストを開放する
}

//=============================================================================
// デバッグ処理
//=============================================================================
void CDebugProc::SelectModel(void)
{
	CScene *pSceneNext = NULL;														//次回アップデート対象
	CScene *pSceneNow = NULL;
	CScene *pSelect = NULL;
	float fDistance = 10000000.0f;

	//for (int nCount = 0; nCount < CScene::PRIORITY_MODEL; nCount++)
	{
		// treeのオブジェクトのポジションを参照
		pSceneNow = CScene::GetScene(CScene::PRIORITY_MODEL);

		//次がなくなるまで繰り返す
		while (pSceneNow != NULL)
		{
			pSceneNext = CScene::GetSceneNext(pSceneNow, (CScene::PRIORITY_MODEL));							//次回アップデート対象を控える
			CObject *pObj = (CObject*)pSceneNow;

			D3DXMATRIX mWorldView;
			D3DXMATRIX m;
			D3DXVECTOR3 vRayDir;
			D3DXVECTOR3 vStartPoint;
			D3DXVECTOR3 v = CManager::GetCursorPosWithCenter();
			D3DXMATRIX mtxWorld, view;
			D3DXVECTOR3 pos = pObj->GetPosition();

			CRenderer *pRenderer = CManager::GetRenderer();
			LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();	// デバイスの取得
			D3DXMatrixTranslation(&mtxWorld, pos.x, pos.y, pos.z);

			pDevice->GetTransform(D3DTS_VIEW, &view);
			mWorldView = mtxWorld * view;
			D3DXMatrixInverse(&m, NULL, &mWorldView);

			//ｖRayDirはレイを飛ばす方向ベクトル
			vRayDir.x = v.x * m._11 + v.y * m._21 + v.z * m._31;
			vRayDir.y = v.x * m._12 + v.y * m._22 + v.z * m._32;
			vRayDir.z = v.x * m._13 + v.y * m._23 + v.z * m._33;

			//vStartPointはスクリーン上の２D座標（クリック位置）を3D空間座標に変換したもの
			vStartPoint.x = m._41;
			vStartPoint.y = m._42;
			vStartPoint.z = m._43;

			//レイ交差判定
			BOOL bHit;
			FLOAT fDist;
			LPD3DXMESH mesh = pObj->GetMesh();
			D3DXIntersect(mesh, &vStartPoint, &vRayDir, &bHit, NULL, NULL, NULL, &fDist, NULL, NULL);

			if (bHit)
			{
				float fWork = CManager::GetDistance(vStartPoint, pObj->GetPosition());
				if (fDistance > fWork)
				{
					// 選択したオブジェクトを格納
					m_pSelect = pObj;
					m_bInspectorWind = true;
					fDistance = fWork;
				}
			}

			pSceneNow = pSceneNext;													//次回アップデート対象を格納
		}
	}
}

//=============================================================================
// デバッグ処理
//=============================================================================
void CDebugProc::Debug(void)
{
	CInputKeyboard *pKeyboard = CManager::GetInputKeyboard();		// キーボードの取得
	CCamera *pCamera = CManager::GetCamera();		// カメラ の取得

#ifdef _DEBUG
	ImGui::Begin("System");			// Systemウィンドウ の生成またはアクセス

	//ImGui::BeginChild("Scrolling");
	//for (int n = 0; n < 50; n++)
	//	ImGui::Text("%04d: Some text", n);
	//ImGui::EndChild();

	//const char* listbox_items[] = { "Apple", "Banana", "Cherry", "Kiwi", "Mango", "Orange", "Pineapple", "Strawberry", "Watermelon" };
	//static int listbox_item_current = 1;
	//ImGui::ListBox("listbox\n(single select)", &listbox_item_current, listbox_items, IM_ARRAYSIZE(listbox_items), 4);

	if (ImGui::Checkbox("DebugMode", &m_bDebug))		// 地形編集モード切り替え
	{
		for (int nCount = 0; nCount < FLOOR_LIMIT * FLOOR_LIMIT; nCount++)
		{
			if (m_apMeshField[nCount] == NULL)
			{// 床が存在していたとき
				m_apMeshField[nCount] = CMeshField::Create();
			}

			if (m_apMeshField[nCount] != NULL)
			{
				if (m_apMeshField[nCount]->GetActive())
				{// 表示していたとき
					m_apMeshField[nCount]->SetDebugRand(true);
					m_apMeshField[nCount]->SetActive(false);
				}
			}
		}

		// オブジェクトマップ
		std::map<std::string, MODEL_INFO> modelMap = CManager::GetModelMap();
		auto itr = modelMap.begin();
		m_currentModel = itr->first.c_str();
	}

	//デバッグ処理を終了
	ImGui::End();
#endif

	// デバッグウィンドウの生成
	if (m_bDebug)
	{
		D3DXVECTOR3 worldPos = D3DXVECTOR3_ZERO;

		if (pCamera != NULL)
		{
			worldPos = pCamera->GetWorldPos();			// マウスのワールド座標を取得
		}

		m_pCircle->SetPosition(worldPos);						// 生成範囲の位置を設定
		m_pCircle->SetRadius(m_fPaintSize);						// 生成範囲の大きさを設定
		m_pCircle->MakeVertex();								// 円の頂点生成

		ImGui::Begin("Debug", &m_bDebug, ImGuiWindowFlags_MenuBar);   // デバッグウィンドウ生成

		CRenderer *pRenderer = CManager::GetRenderer();
		LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();	// デバイスの取得

		pDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);

		if (ImGui::BeginMenuBar())
		{// メニューバーの生成
			MenuBar();					// メニューバー処理
			ImGui::EndMenuBar();		// メニューバーの更新終了
		}

		ImGui::Text("[R]Press is Create");			// デバッグモードテキスト表示

		ImGui::BeginTabBar("General");
		TabBar(worldPos);				// タブ処理
		ImGui::EndTabBar();				// タブの終了処理

		if (pKeyboard->GetTriggerKeyboard(DIK_RCONTROL))
		{
			m_nMode = DEBUGMODE_NONE;
		}

		ImGui::Text("Rand DebugMode");			// デバッグモードテキスト表示

		CDebugProc::Log("マウスのワールド座標 : %f, %f, %f\n",			// マウスのワールド座標を出力
			worldPos.x,
			worldPos.y,
			worldPos.z);

		if (m_nMode != DEBUGMODE_INDIVIDUAL)
		{// 個々配置モードじゃないとき
			ImGui::SliderFloat("paintSize", &m_fPaintSize, 1.0f, 10000.0f);			// スライダーを使用して1つのフロートを編集します
		}

		if (m_nMode != DEBUGMODE_RAND && m_nMode != DEBUGMODE_UI)
		{// 地形編集モードじゃないとき
			SelectAssetWithModel();
		}

		if (m_nMode == DEBUGMODE_UI)
		{// UI作成モードのとき
		}

		if (m_nMode != DEBUGMODE_INDIVIDUAL)
		{// 個々配置モードではなかったとき
			if (m_pSample != NULL)
			{// ポインタが存在していたとき
				m_pSample->Release();				// 開放予約
				m_pSample = NULL;
			}
		}

		if (m_nMode != DEBUGMODE_UI)
		{// UI生成モードではなかったとき
			if (m_pCreateUi != NULL)
			{// UIが存在していたとき
				m_pCreateUi->Uninit();				// 開放処理
				delete m_pCreateUi;					// 削除
				m_pCreateUi = NULL;					// NULLを代入
			}
		}

		if (m_nMode != DEBUGMODE_ENEMY)
		{// エネミー配置モードではなかったとき
			if (m_pEnemy != NULL)
			{// ポインタが存在していたとき
				m_pEnemy->Release();
				m_pEnemy = NULL;
			}
		}

		if (ImGui::Button("Reset"))
		{// Reset ボタンが押されたとき
			CScene::ResetFloor();					// 床の高低をリセット
		}

		if (m_nMode == DEBUGMODE_NONE)
		{// 何もしないモードだったとき
			ImGui::Text("None");

			// デバッグ終了ボタン
			if (ImGui::Button("Debug End"))
			{
				m_bDebug = false;

				if (m_pSample != NULL)
				{// 見本用のモデルがあったとき
					m_pSample->Uninit();		// 終了処理
					m_pSample->Release();		// メモリの開放
					m_pSample = NULL;			// NULLを代入
				}

				// デバッグ用床の開放処理
				for (int nCount = 0; nCount < FLOOR_LIMIT * FLOOR_LIMIT; nCount++)
				{
					if (m_apMeshField[nCount] != NULL)
					{// デバッグ用の床が存在していたとき
						m_apMeshField[nCount]->Release();
						m_apMeshField[nCount] = NULL;
					}
				}
			}
		}
		else if (m_nMode == DEBUGMODE_RAND)
		{// 地形編集モードだったとき
		}
		else if (m_nMode == DEBUGMODE_MANY)
		{// 範囲内多数生成モードだったとき
		}
		else if (m_nMode == DEBUGMODE_INDIVIDUAL)
		{// 個々配置モードだったとき
		}
		else if (m_nMode == DEBUGMODE_DELETE)
		{// 削除モードだったとき
		}
		else if (m_nMode == DEBUGMODE_PAINT)
		{// 地形編集モードだったとき
		}
		else if (m_nMode == DEBUGMODE_ENEMY)
		{
		}
		// 更新終了
		ImGui::End();
	}
}

//=============================================================================
// メニューバー処理
//=============================================================================
void CDebugProc::MenuBar(void)
{
	if (ImGui::BeginMenu("File"))
	{// ファイルタブの生成
		if (ImGui::BeginMenu("Load"))
		{// セーブタブの生成
			if (ImGui::MenuItem("Rand"))
			{// ロード
			 // 床情報の読み込み
				CMeshField::LoadRand("data/stage/rand.txt", false);
			}
			if (ImGui::MenuItem("Model"))
			{// ロード
			 // モデル情報の読み込み
				CObject::LoadModelTest("data/text/model.txt");
			}
			if (ImGui::MenuItem("wall"))
			{// ロード
			 // モデル情報の読み込み
				CMeshWall::LoadWall("data/text/wall.txt", false);
			}
			if (ImGui::MenuItem("All"))
			{// セーブ
			 // 床情報の読み込み
				CMeshField::LoadRand("data/stage/rand.txt", false);

				// モデル情報の読み込み
				CObject::LoadModelTest("data/text/model.txt");
			}
			ImGui::EndMenu();			// メニューの更新終了
		}

		if (ImGui::BeginMenu("Save"))
		{// セーブタブの生成
			if (ImGui::MenuItem("Rand"))
			{// ロード
			 // 床情報の書き込み
				CScene::SaveRand();
			}
			if (ImGui::MenuItem("Model"))
			{// ロード
			 // モデル情報の書き込み
				CScene::SaveModel();
			}
			if (ImGui::MenuItem("wall"))
			{// ロード
			 // モデル情報の書き込み
				CScene::SaveWall();
			}
			if (ImGui::MenuItem("All"))
			{// セーブ
			 // 床情報の書き込み
				CScene::SaveRand();

				// モデル情報の書き込み
				CScene::SaveModel();
			}
			ImGui::EndMenu();			// メニューの更新終了
		}

		ImGui::EndMenu();			// メニューの更新終了
	}
}

//=============================================================================
// タブ処理
//=============================================================================
void CDebugProc::TabBar(D3DXVECTOR3 &worldPos)
{
	if (ImGui::BeginTabItem(u8"ランドスケープ"))
	{// ランドスケープモード
		if (m_nMode != DEBUGMODE_RAND)
		{
			m_nMode = DEBUGMODE_RAND;
		}

		LandScape(worldPos);
		ImGui::Text(u8"ランドスケープ");				// 現在のデバッグモードの表示
		ImGui::EndTabItem();
	}
	if (ImGui::BeginTabItem(u8"壁の頂点編集"))
	{// 壁の頂点編集モード
		if (m_nMode != DEBUGMODE_WALL)
		{
			m_nMode = DEBUGMODE_WALL;
		}

		EditWallVertex();
		ImGui::Text(u8"壁の頂点編集");				// 現在のデバッグモードの表示
		ImGui::EndTabItem();
	}
	if (ImGui::BeginTabItem(u8"フォリッジ"))
	{// フォリッジモード
		if (m_nMode != DEBUGMODE_MANY)
		{
			m_nMode = DEBUGMODE_MANY;
		}

		ImGui::SliderInt("Generation", &m_nCntGeneration, 0, 50);
		CreateObject(worldPos);					// 多数配置モードの実行
		ImGui::Text("Many Debug");
		ImGui::EndTabItem();
	}
	if (ImGui::BeginTabItem(u8"個々配置"))
	{// 個々配置モード
		if (m_nMode != DEBUGMODE_INDIVIDUAL)
		{
			m_nMode = DEBUGMODE_INDIVIDUAL;
		}

		CreateIndividual(worldPos);				// 個々配置モードの実行

		if (m_pSample == NULL)
		{
			m_pSample = CObject::Create();						// 見本用オブジェクトを作成

			if (m_pSample != NULL)
			{
				m_pSample->BindModel(m_currentModel);
				m_pSample->SetPosition(worldPos);								// 位置をマウスのワールド座標に設定
				m_pSample->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f));			// 色の変更
			}
		}
		ImGui::SameLine();															// 改行回避
		ImGui::RadioButton("delete", &m_nMode, DEBUGMODE_DELETE);					// 選択肢 範囲内多数生成モード を追加

		// 現在のデバッグタイプを表示
		ImGui::Text("individual Debug");
		ImGui::EndTabItem();
	}
	if (ImGui::BeginTabItem(u8"UI作成モード"))
	{// UI作成モード
		if (m_nMode != DEBUGMODE_UI)
		{
			m_nMode = DEBUGMODE_UI;
		}

		if (m_pCreateUi != NULL)
		{
			D3DXVECTOR3 pos = m_pCreateUi->GetPosition();
			ImGui::DragFloat3("pos_ui", (float*)&pos);
			m_pCreateUi->SetPosition(pos);

			if (ImGui::CollapsingHeader("OpenFile"))
			{// UIを開く
			 // UIのスクリプト選択
				SelectAssetWithUI();

				if (ImGui::Button("Open"))
				{// 開く
					m_pCreateUi->LoadScript(m_currentUi);				// スクリプトの読み込み
				}
			}

			if (ImGui::CollapsingHeader("CreateTexture"))
			{// テクスチャ生成
				// UIのスクリプト選択
				SelectAssetWithTexture();

				if (ImGui::Button("AssetCreate"))
				{// アセット作成
					m_pCreateUi->CreateTexture(m_currentTexture);		// アセットの作成
				}
			}

			m_pCreateUi->SceneDebug();

			// 書き出し処理
			if (ImGui::CollapsingHeader("System"))
			{// テクスチャ生成
				ImGui::Text(u8"書き出し処理");
				ImGui::InputText("name", m_CreateName, NAME_SIZE);

				if (ImGui::Button("Export"))
				{// 出力ボタン
					m_pCreateUi->SaveScript(m_CreateName);		// スクリプトに書き込み処理
				}

				ImGui::Text(u8"終了処理");
				if (ImGui::Button("End"))
				{// 終了ボタン
					m_pCreateUi->Uninit();				// 表示中のアセットを開放
					m_pCreateUi->Release();
					m_pCreateUi = NULL;
				}
			}
		}
		else
		{
			m_pCreateUi = CUi::Create();
		}

		// 現在のデバッグタイプを表示
		ImGui::Text(u8"UI作成モード");
		ImGui::EndTabItem();
	}
	if (ImGui::BeginTabItem("delete"))
	{// オブジェクト削除モード
		if (m_nMode != DEBUGMODE_DELETE)
		{
			m_nMode = DEBUGMODE_DELETE;
		}

		DeleteObject(worldPos);					// 削除モードの実行
		// 現在のデバッグタイプを表示
		ImGui::Text(u8"削除モード");
		ImGui::EndTabItem();
	}
	if (ImGui::BeginTabItem("Randpaint"))
	{// 頂点カラーの変更モード
		if (m_nMode != DEBUGMODE_PAINT)
		{
			m_nMode = DEBUGMODE_PAINT;
		}

		Paint(worldPos);
		ImGui::Text("Paint Debug");				// 現在のデバッグモードの表示
		ImGui::EndTabItem();
	}
	if (ImGui::BeginTabItem("Enemy"))
	{// 頂点カラーの変更モード
		if (m_nMode != DEBUGMODE_ENEMY)
		{
			m_nMode = DEBUGMODE_ENEMY;
		}

		if (m_pEnemy == NULL)
		{
			m_pEnemy = CEnemy::Create();			// 見本用オブジェクトを作成

			if (m_pEnemy != NULL)
			{
				m_pEnemy->SetPosition(worldPos);	// 位置をマウスのワールド座標に設定
			}
		}

		CreateEnemy(worldPos);
		ImGui::EndTabItem();
	}
	if (ImGui::BeginTabItem("Particle"))
	{// 頂点カラーの変更モード
		if (m_nMode != DEBUGMODE_PARTICLE)
		{
			m_nMode = DEBUGMODE_PARTICLE;
		}

		// タグの設定
		ImGui::InputText("Tag", m_effectTag, NAME_SIZE);

		// UIのスクリプト選択
		SelectAssetWithTexture();

		// エフェクトの放出タイプ選択
		ImGui::Combo("ParticleType", &m_nEmissionType, "Cone\0Sphere\0Box\0");

		ImGui::DragInt(u8"生成数", &m_nCreate);		// 生成数
		ImGui::DragFloat3(u8"位置", (float*)&m_createPos);
		ImGui::DragFloat3(u8"回転量", (float*)&m_createRot);
		ImGui::DragFloat3(u8"大きさ", (float*)m_size);
		ImGui::DragFloat3(u8"大きさの変化量", (float*)&m_moveSize);
		ImGui::DragFloat3(u8"回転の変化量", (float*)&m_moveRot);
		ImGui::DragFloat4(u8"カラー", (float*)&m_col);
		ImGui::DragFloat4(u8"色の変化量", (float*)&m_moveCol);
		ImGui::DragFloat2(u8"画像分割数", (float*)&m_sprite);

		ImGui::DragInt(u8"生存時間", &m_particleLife);
		ImGui::DragInt(u8"形状", &m_nParticleShape);

		ImGui::Checkbox(u8"角度のランダム", &m_bRandAngle);

		ImGui::DragFloat(u8"スピードの最大値", &m_fMaxSpeed);

		if (m_bRandAngle)
		{
			// 角度
			m_fAngle = CEffect::GetRandomAngle();		// ランダムに取得
		}
		else { ImGui::DragFloat(u8"角度", &m_fAngle); }

		float fSpeed = 0.0f;
		if (!m_bRandomSpeed)
		{// スピードがランダムじゃなかったとき
			fSpeed = m_fMaxSpeed;
		}

		ImGui::DragFloat(u8"距離", &m_fDistance);
		ImGui::DragFloat(u8"回転速度", &m_fRotationSpeed);
		ImGui::DragFloat(u8"重力加速度", &m_fGravity);

		ImGui::Checkbox(u8"繰り返す", &m_bLoop);
		ImGui::Checkbox(u8"ランダムスピード", &m_bRandomSpeed);
		ImGui::Checkbox(u8"フェードアウト", &m_bFadeOut);
		ImGui::Checkbox(u8"加算合成", &m_bAlpha);

		switch ((PARTICLESHAPE)m_nEmissionType)
		{
		case PARTICLESHAPE_CONE:
			ImGui::DragFloat(u8"半径", &m_fRadius);
			break;
		case PARTICLESHAPE_SPHERE:
			break;
		case PARTICLESHAPE_BOX:
			break;
		}

		if (ImGui::Button(u8"生成"))
		{
			for (int nCount = 0; nCount < m_nCreate; nCount++)
			{
				float fAngle = float(CManager::GetRand(314)) - float(CManager::GetRand(314));
				float fAngle_x = float(CManager::GetRand(314)) - float(CManager::GetRand(314));

				D3DXVECTOR3 particlePos;
				float fRadius;

				switch ((PARTICLESHAPE)m_nEmissionType)
				{
				case PARTICLESHAPE_CONE:
					fRadius = float(CManager::GetRand(10 * 100)) / 100.0f - float(CManager::GetRand(10 * 100)) / 100.0f;

					particlePos = CEffect::GetRandomPosWithCone(m_fRadius);

					// 位置の計算
					particlePos.x = sinf(D3DX_PI * fAngle) * (fRadius);
					particlePos.y = m_fMaxSpeed;
					particlePos.z = cosf(D3DX_PI * fAngle) * (fRadius);
					break;
				case PARTICLESHAPE_SPHERE:
					particlePos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

					particlePos.x = cosf(D3DX_PI + fAngle) * cosf(D3DX_PI + fAngle_x);
					particlePos.y = sinf(D3DX_PI + fAngle_x);
					particlePos.z = sinf(D3DX_PI + fAngle) * cosf(D3DX_PI + fAngle_x);

					fAngle = float(CManager::GetRand(314)) / 100.0f - float(CManager::GetRand(314)) / 100.0f;
					break;
				case PARTICLESHAPE_BOX:
					break;
				}

				D3DXVECTOR3 rot = D3DXVECTOR3_ZERO;
				//rot = D3DXVECTOR3(sinf(fAngle) * 10, cosf(fAngle) * 10, 0.0f);

				// パーティクル全体の位置計算
				D3DXVECTOR3 vecPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
				D3DXMATRIX mtxMeshRot, mtxMeshTrans;				// 計算用マトリックス
				D3DXMATRIX mtx;										// 武器のマトリックス
				D3DXMATRIX mtxPlayer;

				// ワールドマトリックスの初期化
				D3DXMatrixIdentity(&mtx);

				// ワールドマトリックスの初期化
				D3DXMatrixIdentity(&mtxPlayer);

				// 回転を反映
				D3DXMatrixRotationYawPitchRoll(&mtxMeshRot, rot.y, rot.x, rot.z);
				D3DXMatrixMultiply(&mtxPlayer, &mtxPlayer, &mtxMeshRot);

				// 位置を反映
				D3DXMatrixTranslation(&mtxMeshTrans, vecPos.x, vecPos.y, vecPos.z);
				D3DXMatrixMultiply(&mtxPlayer, &mtxPlayer, &mtxMeshTrans);

				// 回転を反映
				D3DXMatrixRotationYawPitchRoll(&mtxMeshRot, 0.0f, 0.0f, 0.0f);
				D3DXMatrixMultiply(&mtx, &mtx, &mtxMeshRot);

				// 位置を反映
				D3DXMatrixTranslation(&mtxMeshTrans, particlePos.x, particlePos.y, particlePos.z);
				D3DXMatrixMultiply(&mtx, &mtx, &mtxMeshTrans);

				D3DXMatrixMultiply(&mtx, &mtx, &mtxPlayer);

				CEffect::SetEffect
				(
					m_currentTexture,		// パーティクルのタイプ
					m_createPos,			// 発生位置
					m_size,					// サイズ
					D3DXVECTOR3(mtx._41, mtx._42, mtx._43) * fSpeed,	// 方向ベクトル
					m_moveSize,				// 大きさの変化量
					m_moveRot,				// 回転の変化量
					m_moveCol,				// 色の変化量
					EASINGTYPE_NONE,
					rot,					// テクスチャの回転
					m_col,					// カラー
					m_particleLife,			// パーティクルの生存カウント数
					m_fGravity,				// 重力
					m_fResistance,			// 抵抗
					m_bBillboard,			// ビルボード
					0,						// 表示する箇所(横)
					0,						// 表示する箇所(縦)
					m_centerPos,			// 中心位置
					m_fAngle,				// 角度
					m_fDistance,			// 距離
					m_fRotationSpeed,		// 回転速度
					m_sprite,				// 画像の分割数
					m_bAlpha,				// 加算合成の有無
					m_bZBuffer,				// Zバッファの比較有無
					m_bFadeOut				// フェード
				);
			}
		}

		if (ImGui::Button("Export"))
		{
			SaveParticle();
		}

		ImGui::EndTabItem();
	}
}

//=============================================================================
// ランドスケープ処理
//=============================================================================
void CDebugProc::LandScape(D3DXVECTOR3 &worldPos)
{
	CInputMouse *pMouse = CManager::GetInputMouse();								// マウスの取得
	CInputKeyboard *pKeyboard = CManager::GetInputKeyboard();						// キーボードの取得

	// 現在マウスの座標の範囲内にいる床を取得する
	CScene *pSceneNext = NULL;														// 初期化
	CScene *pSceneNow = CScene::GetScene(CScene::PRIORITY_FLOOR);					// 床の先頭アドレスの取得

	//次がなくなるまで繰り返す
	while (pSceneNow != NULL)
	{
		pSceneNext = CScene::GetSceneNext(pSceneNow, CScene::PRIORITY_FLOOR);		//次回アップデート対象を控える
		CMeshField *pField = (CMeshField*)pSceneNow;
		if (!pField->GetDebugRand())
		{
			if (pField->CollisionRangeToCircle(worldPos, m_fPaintSize))					// 円が床の範囲内に入っているかどうか
			{
				if (m_mouseOld != worldPos)
				{// 前の座標と今回の座標が違うとき
					pField->CreateRand(worldPos, m_fPaintSize);
				}
			}
		}
		pSceneNow = pSceneNext;														//次回アップデート対象を格納
	}

	m_CreateRandOld = m_CreateRand;						// 前のデータを代入

	// 各種スライダーの設定
	ImGui::DragFloat(u8"生成位置のスライドパワー", &m_fSliderPow, 0.5f, 0.1f, 50.0f, "%.1f", 0.5f);

	if (ImGui::DragFloat3(u8"生成位置", (float*)&m_createPos, m_fSliderPow))
	{
		DebugFloorCreate((int)m_CreateRand.x, (int)m_CreateRand.y, m_createPos);		// デバッグ用の床の再配置
	}

	if (ImGui::DragFloat2(u8"生成枚数", (float*)&m_CreateRand, 1.0f, 0.0f, 20.0f, "%.0f", 1.0f))
	{
		DebugFloorCreate((int)m_CreateRand.x, (int)m_CreateRand.y, m_createPos);		// デバッグ用の床の再配置
	}

	if (ImGui::Button(u8"生成"))
	{// 生成ボタンが押されたとき
		CMeshField::CreateRand((int)m_CreateRand.x, (int)m_CreateRand.y, m_createPos);	// ちゃんとした床を生成
	}

	if (!pKeyboard->GetPressKeyboard(DIK_LALT))
	{// 左Altキーが押されていないとき
		if (pKeyboard->GetPressKeyboard(DIK_R))
		{// Rキーが押されているとき
			if (pMouse->GetReleaseMouse(MOUSE_RIGHT))
			{// マウスの右ボタンが離されたとき
				pSceneNow = CScene::GetScene(CScene::PRIORITY_FLOOR);					// 床の先頭アドレスの取得

				//次がなくなるまで繰り返す
				while (pSceneNow != NULL)
				{
					pSceneNext = CScene::GetSceneNext(pSceneNow, CScene::PRIORITY_FLOOR);		//次回アップデート対象を控える
					CMeshField *pField = (CMeshField*)pSceneNow;

					if (!pField->GetDebugRand())
					{
						pField->CalculationNormalize();
					}

					pSceneNow = pSceneNext;														//次回アップデート対象を格納
				}
			}
			else if (pMouse->GetReleaseMouse(MOUSE_LEFT))
			{// マウスの左ボタンが離されたとき
				pSceneNow = CScene::GetScene(CScene::PRIORITY_FLOOR);					// 床の先頭アドレスの取得

				//次がなくなるまで繰り返す
				while (pSceneNow != NULL)
				{
					pSceneNext = CScene::GetSceneNext(pSceneNow, CScene::PRIORITY_FLOOR);		//次回アップデート対象を控える
					CMeshField *pField = (CMeshField*)pSceneNow;

					if (!pField->GetDebugRand())
					{
						pField->CalculationNormalize();
					}

					pSceneNow = pSceneNext;														//次回アップデート対象を格納
				}
			}
		}
	}

	m_mouseOld = worldPos;
}

//=============================================================================
// 壁の頂点編集処理
//=============================================================================
void CDebugProc::EditWallVertex(void)
{
	CScene *pSceneNext = NULL;	//次回アップデート対象
	CScene *pSceneNow = NULL;

	// 壁の情報 //
	// treeのオブジェクトのポジションを参照
	pSceneNow = CScene::GetScene(CScene::PRIORITY_WALL);

	//次がなくなるまで繰り返す
	while (pSceneNow != NULL)
	{
		pSceneNext = CScene::GetSceneNext(pSceneNow, CScene::PRIORITY_WALL);						//次回アップデート対象を控える
		CMeshWall *pMeshWall = (CMeshWall*)pSceneNow;
		pMeshWall->EditWallVertex();			// 壁の頂点情報編集処理
		pSceneNow = pSceneNext;													// 次回アップデート対象を格納
	}
}

//=============================================================================
// 頂点色変更処理
//=============================================================================
void CDebugProc::Paint(D3DXVECTOR3 & worldPos)
{
	CInputMouse *pMouse = CManager::GetInputMouse();								// マウスの取得
	CInputKeyboard *pKeyboard = CManager::GetInputKeyboard();						// キーボードの取得

	// 現在マウスの座標の範囲内にいる床を取得する
	CScene *pSceneNext = NULL;														// 初期化
	CScene *pSceneNow = CScene::GetScene(CScene::PRIORITY_FLOOR);					// 床の先頭アドレスの取得

	// 次がなくなるまで繰り返す
	while (pSceneNow != NULL)
	{
		pSceneNext = CScene::GetSceneNext(pSceneNow, CScene::PRIORITY_FLOOR);		// 次回アップデート対象を控える
		CMeshField *pField = (CMeshField*)pSceneNow;
		if (pField->CollisionRangeToCircle(worldPos, m_fPaintSize))					// 円が床の範囲内に入っているかどうか
		{
			if (m_mouseOld != worldPos)
			{// 前の座標と今回の座標が違うとき
				pField->Paint(worldPos, m_fPaintSize);
			}
		}
		pSceneNow = pSceneNext;														// 次回アップデート対象を格納
	}

	m_mouseOld = worldPos;
}

//=============================================================================
// 敵配置処理
//=============================================================================
void CDebugProc::CreateEnemy(D3DXVECTOR3 &worldPos)
{
	CInputMouse *pMouse = CManager::GetInputMouse();			// マウスの取得
	CInputKeyboard *pKeyboard = CManager::GetInputKeyboard();	// キーボードの取得
	CScene *pScene = CScene::NowFloor(worldPos);				// 現在いる場所のフィールドを取得

	D3DXVECTOR3 pos = worldPos;

	if (pScene != NULL)
	{// 床が存在していたとき
		CMeshField *pMeshField = (CMeshField*)pScene;			// 床の取得
		pos.y = pMeshField->GetHeight(worldPos);				// 床の高さを取得
	}

	if (m_pEnemy != NULL)
	{
		m_pEnemy->SetPosition(pos);								// 見本用モデルの位置を現在のマウス座標に設定
	}

	if (!pKeyboard->GetPressKeyboard(DIK_LALT))
	{// Altキー が押されていないとき
		if (pKeyboard->GetPressKeyboard(DIK_R))
		{
			if (pMouse->GetTriggerMouse(MOUSE_LEFT))
			{// マウスの左ボタンが押されたとき
				CEnemy *pEnemy = CEnemy::Create();				// 現在の見本を作成
				pEnemy->SetPosition(worldPos);					// 見本の場所に設置
			}
		}
	}
}

//=============================================================================
// 複数配置処理
//=============================================================================
void CDebugProc::CreateObject(D3DXVECTOR3 &worldPos)
{
	CInputMouse *pMouse = CManager::GetInputMouse();				// マウスの取得
	CInputKeyboard *pKeyboard = CManager::GetInputKeyboard();		// キーボードの取得
	CCamera *pCamera = CManager::GetCamera();						// カメラの取得
	CMeshField *pMeshField = CGame::GetMesh();
	CObject *pObject = NULL;

	if (!pKeyboard->GetPressKeyboard(DIK_LALT))
	{// 左Altキーが押されていないとき
		if (pKeyboard->GetPressKeyboard(DIK_R))
		{
			if (pMouse->GetPressMouse(MOUSE_LEFT))
			{// マウスの左ボタンが押されているとき
				D3DXVECTOR3 Difference;				// 差分
				float fDistance;					// 距離

				// 前の位置と現在の位置との距離を算出
				Difference.x = m_mouseOld.x - worldPos.x;
				Difference.z = m_mouseOld.z - worldPos.z;
				fDistance = (float)sqrt(Difference.x * Difference.x + Difference.z * Difference.z);

				if (fDistance > m_fPaintSize)
				{// 前の座標と今回の座標が違うとき
					// モデルの範囲配置
					for (int nCount = 0; nCount < m_nCntGeneration; nCount++)
					{
						float fAngle = float(rand() % 314) / 100.0f - float(rand() % 314) / 100.0f;
						D3DXVECTOR3 pos = D3DXVECTOR3(sinf(D3DX_PI + 6.28f * fAngle) * ((float)CManager::GetRand((int)m_fPaintSize * 10) * 0.1f) + worldPos.x,
							0.0f,
							cosf(D3DX_PI + 6.28f * fAngle) * ((float)CManager::GetRand((int)m_fPaintSize * 10) * 0.1f) + worldPos.z);

						//床の高さを取得する
						CScene *pSceneNext = NULL;			// 初期化
						CScene *pSceneNow = CScene::GetScene(CScene::PRIORITY_MODEL);			// シーンの先頭アドレスを取得

						//次がなくなるまで繰り返す
						while (pSceneNow != NULL)
						{
							pSceneNext = CScene::GetSceneNext(pSceneNow, CScene::PRIORITY_MODEL);		// 次回アップデート対象を控える
							CMeshField *pField = (CMeshField*)pSceneNow;								// クラスチェンジ(床)

							if (m_nCntContinue > CONTINUE_MAX)
							{// 再計算回数が10回を超えたとき
								break;
							}
							else if (50.0f > CManager::GetDistance(pSceneNow->GetPosition(), pos))
							{
								// 値の再取得
								float fAngle = float(rand() % 314) / 100.0f - float(rand() % 314) / 100.0f;
								D3DXVECTOR3 pos = D3DXVECTOR3(sinf(D3DX_PI + 6.28f * fAngle) * ((float)CManager::GetRand((int)m_fPaintSize * 10) * 0.1f) + worldPos.x,
									0.0f,
									cosf(D3DX_PI + 6.28f * fAngle) * ((float)CManager::GetRand((int)m_fPaintSize * 10) * 0.1f) + worldPos.z);

								pSceneNext = CScene::GetScene(CScene::PRIORITY_MODEL);			// シーンの先頭アドレスを取得
								m_nCntContinue++;					// 再確認回数にプラスする
							}

							pSceneNow = pSceneNext;				//次回アップデート対象を格納
						}

						if (m_nCntContinue < CONTINUE_MAX)
						{
							// オブジェクトの作成
							pObject = CObject::Create();
							if (pObject != NULL)
							{
								pObject->BindModel(m_currentModel);
							}
						}

						m_nCntContinue = 0;

						if (pObject != NULL)
						{
							//床の高さを取得する
							CScene *pSceneNext = NULL;			// 初期化
							CScene *pSceneNow = CScene::GetScene(CScene::PRIORITY_FLOOR);			// シーンの先頭アドレスを取得

							//次がなくなるまで繰り返す
							while (pSceneNow != NULL)
							{
								pSceneNext = CScene::GetSceneNext(pSceneNow, CScene::PRIORITY_FLOOR);		// 次回アップデート対象を控える
								CMeshField *pField = (CMeshField*)pSceneNow;								// クラスチェンジ(床)

								if (pField->CollisionRange(pos))
								{// オブジェクト が床に乗っていたとき
									pos.y = pField->GetHeight(pos);										// 床の高さを求める
									pObject->SetPosition(pos);
									break;
								}

								pSceneNow = pSceneNext;				//次回アップデート対象を格納
							}
						}
					}

					m_mouseOld = worldPos;					// 最後の加工位置を保存
				}
			}
		}
	}
}

//=============================================================================
// 単体配置処理
//=============================================================================
void CDebugProc::CreateIndividual(D3DXVECTOR3 &worldPos)
{
	CInputMouse *pMouse = CManager::GetInputMouse();								// マウスの取得
	CInputKeyboard *pKeyboard = CManager::GetInputKeyboard();						// キーボードの取得
	CScene *pScene = CScene::NowFloor(worldPos);									// 現在いる場所のフィールドを取得
	CPlayer *pPlayer = CGame::GetPlayer();

	D3DXVECTOR3 pos = D3DXVECTOR3_ZERO;
	D3DXVECTOR3 rot = D3DXVECTOR3_ZERO;
	D3DXVECTOR3 size = D3DXVECTOR3_ZERO;

	if (m_pSample != NULL)
	{// 見本用オブジェクトが存在していたとき
		if (!m_bMouseCursorPosition)
		{
			pos = m_pSample->GetPosition();				// 位置の取得
		}
		else
		{
			if (pPlayer != NULL)
			{
				pos = pPlayer->GetPosition();
			}
		}

		rot = m_pSample->GetRotation();					// 回転値の取得
		size = m_pSample->GetSize();					// サイズの取得
	}

	ImGui::Checkbox("MousePos", &m_bMouseCursorPosition);
	ImGui::DragFloat3("pos", (float*)&pos);
	ImGui::DragFloat3("rot", (float*)&rot, 0.01f);
	ImGui::DragFloat3("size", (float*)&size, 0.01f);

	if (pScene != NULL)
	{// 床が存在していたとき
		CMeshField *pMeshField = (CMeshField*)pScene;			// 床の取得
		if (pMeshField != NULL)
		{
			pos.y = pMeshField->GetHeight(worldPos);			// 床の高さを取得
		}
	}

	if (m_pSample != NULL)
	{
		if (strcmp(m_pSample->GetAdd().c_str(), m_currentModel.c_str()) != 0)
		{// 現在のモデルタイプが前回のモデルと違うとき
			m_pSample->Uninit();										// 前のモデルを開放
			m_pSample->Release();										// 前のモデルの開放フラグを立てる
			m_pSample = NULL;											// 前のモデルのアドレスを捨てる

			m_pSample = CObject::Create();								// 新しいモデルを生成

			if (m_pSample != NULL)
			{
				m_pSample->BindModel(m_currentModel);
			}
		}
	}

	if (m_pSample != NULL)
	{// 見本用オブジェクトが存在していたとき
		m_pSample->SetPosition(pos);									// 見本用モデルの位置を現在のマウス座標に設定
		m_pSample->SetRotation(rot);									// 回転値の取得
		m_pSample->SetSize(size);
	}

	if (!pKeyboard->GetPressKeyboard(DIK_LALT))
	{// Altキー が押されていないとき
		if (pKeyboard->GetPressKeyboard(DIK_R))
		{
			if (pMouse->GetTriggerMouse(MOUSE_LEFT))
			{// マウスの左ボタンが押されたとき
				CObject *pObject = CObject::Create();					// 現在の見本を作成

				if (pObject != NULL)
				{
					pObject->BindModel(m_currentModel);
					pObject->SetPosition(pos);							// 見本の場所に設置
					pObject->SetRotation(rot);							// 回転量の設定
					pObject->SetSize(size);								// サイズの設定
				}
			}
		}
	}
}

//=============================================================================
// オブジェクト範囲内削除処理
//=============================================================================
void CDebugProc::DeleteObject(D3DXVECTOR3 &worldPos)
{
	CInputMouse *pMouse = CManager::GetInputMouse();								// マウスの取得
	CInputKeyboard *pKeyboard = CManager::GetInputKeyboard();						// キーボードの取得

	// 現在マウスの座標の範囲内にいる床を取得する
	CScene *pSceneNext = NULL;														// 初期化
	CScene *pSceneNow = CScene::GetScene(CScene::PRIORITY_MODEL);					// 床の先頭アドレスの取得

	if (pKeyboard->GetPressKeyboard(DIK_R))
	{
		if (pMouse->GetPressMouse(MOUSE_LEFT))
		{// マウスの右ボタンが離されたとき
		// 次がなくなるまで繰り返す
			while (pSceneNow != NULL)
			{
				pSceneNext = CScene::GetSceneNext(pSceneNow, CScene::PRIORITY_MODEL);		//次回アップデート対象を控える
				CSceneX *pObject = (CSceneX*)pSceneNow;

				if (CMeshField::SphereModel(pObject->GetPosition(), worldPos, m_fPaintSize))					// 円が床の範囲内に入っているかどうか
				{
					pObject->Release();
				}
				pSceneNow = pSceneNext;														//次回アップデート対象を格納
			}
		}
	}
}

//=============================================================================
// パーティクル作成
//=============================================================================
void CDebugProc::CreateParticle(void)
{

}

//=============================================================================
// デバッグ用床生成処理
//=============================================================================
void CDebugProc::DebugFloorCreate(const int &nWide, const int &nDepth, D3DXVECTOR3 &createPos)
{
	// デバッグ用床ポインタの初期化
	for (int nCount = 0; nCount < FLOOR_LIMIT * FLOOR_LIMIT; nCount++)
	{
		if (m_apMeshField[nCount] == NULL)
		{// 床が存在していたとき
			continue;
		}

		if (m_apMeshField[nCount]->GetActive())
		{// 表示していたとき
			m_apMeshField[nCount]->SetActive(false);
		}
	}

	for (int nCntDepth = 0; nCntDepth < nDepth; nCntDepth++)
	{
		for (int nCntWide = 0; nCntWide < nWide; nCntWide++)
		{
			D3DXVECTOR3 pos;

			pos.x = (WIDE_FIELD * SIZE) * nCntWide - (((WIDE_FIELD * SIZE) * nWide) / 2) + ((WIDE_FIELD * SIZE) / 2) + createPos.x;
			pos.y = createPos.y;
			pos.z = (DEPTH_FIELD * SIZE) * nCntDepth - (((DEPTH_FIELD * SIZE) * nDepth) / 2) + ((DEPTH_FIELD * SIZE) / 2) + createPos.z;

			for (int nCount = 0; nCount < FLOOR_LIMIT * FLOOR_LIMIT; nCount++)
			{
				if (m_apMeshField[nCount] == NULL)
				{
					continue;
				}

				if (m_apMeshField[nCount]->GetActive())
				{
					continue;
				}

				m_apMeshField[nCount]->SetPosition(pos);
				m_apMeshField[nCount]->MakeVertex(NULL, true);
				m_apMeshField[nCount]->SetActive(true);
				break;
			}
		}
	}
}

//=============================================================================
// インスペクターを表示
//=============================================================================
void CDebugProc::ShowInspector(void)
{
	// 自分で作成した簡単なウィンドウを表示します。 Begin / Endペアを使用して、名前付きウィンドウを作成します。
	ImGui::Begin("Inspector", &m_bInspectorWind, ImGuiWindowFlags_MenuBar);   // インスペクターウィンドウ生成
	m_pSelect->ShowInspector();							// インスペクターを表示
	ImGui::End();
}

//=============================================================================
// モデル選択処理
//=============================================================================
void CDebugProc::SelectAssetWithModel(void)
{
	if (ImGui::BeginCombo(u8"オブジェクトの種類", m_currentModel.c_str()))
	{
		// オブジェクトマップ
		std::map<std::string, MODEL_INFO> modelMap = CManager::GetModelMap();
		for (auto itr = modelMap.begin(); itr != modelMap.end(); itr++)
		{
			bool is_selected = (m_currentModel == itr->first.c_str());
			if (ImGui::Selectable(itr->first.c_str()))
			{
				m_currentModel = itr->first.c_str();
			}
		}

		ImGui::EndCombo();
	}
}

//=============================================================================
// テクスチャ選択処理
//=============================================================================
void CDebugProc::SelectAssetWithTexture(void)
{
	if (ImGui::BeginCombo(u8"テクスチャの種類", m_currentTexture.c_str()))
	{
		// テクスチャマップ
		std::map<std::string, LPDIRECT3DTEXTURE9> modelMap = CManager::GetTextureMap();
		for (auto itr = modelMap.begin(); itr != modelMap.end(); itr++)
		{
			bool is_selected = (m_currentTexture == itr->first.c_str());
			if (ImGui::Selectable(itr->first.c_str()))
			{
				m_currentTexture = itr->first.c_str();
			}
		}

		ImGui::EndCombo();
	}
}

//=============================================================================
// UI選択処理
//=============================================================================
void CDebugProc::SelectAssetWithUI(void)
{
	if (ImGui::BeginCombo(u8"UIの種類", m_currentUi.c_str()))
	{
		for (auto itr = m_AddUi.begin(); itr != m_AddUi.end(); itr++)
		{
			bool is_selected = (m_currentUi == itr->c_str());
			if (ImGui::Selectable(itr->c_str()))
			{
				m_currentUi = itr->c_str();
			}
		}

		ImGui::EndCombo();
	}
}

//=============================================================================
// パーティクルパラメータの書き込み
//=============================================================================
void CDebugProc::SaveParticle(void)
{
	CWrite *pWrite = new CWrite;
	if (pWrite == NULL) return;

	//変数宣言
	int nCntLoad = 0;			//ロードカウント
	char text[64];				// テキスト
	char cEqual[8] = { "=" };	//イコール用
	CScene *pSceneNext = NULL;	//次回アップデート対象
	CScene *pSceneNow = NULL;


	//開けた
	if (pWrite->Open("data/text/particle_Export.txt"))
	{
		strcpy(text, "# シーンデータスクリプト\n");
		strcat(text, "# Author : masayasu wakita\n");

		pWrite->TitleWrite(text);				// タイトルの形式で書き込む
		pWrite->Write("SCRIPT\n");				// スクリプトの終了宣言

		pWrite->IndexWrite(" エフェクトの設定");				// タイトルの形式で書き込む
		pWrite->Write("EFFECTSET\n");			// 頂点情報の書き込み開始宣言
		pWrite->Write("	TAG = %s\n", m_effectTag);		// タグの書き込み
		pWrite->Write("	EFFECT_FILENAME = %s\n", m_currentTexture.c_str());		// アドレス情報を書き込む

		if (m_bBillboard) { pWrite->Write("	BILLBOARD\n"); }			// ビルボードの有無
		if (m_bAlpha) { pWrite->Write("	ALPHABLENDING\n"); }			// アルファブレンディングの有無
		if (m_bZBuffer) { pWrite->Write("	ZBUFFER\n"); }			// Zバッファの有無
		if (m_bFadeOut) { pWrite->Write("	FADE_OUT\n"); }			// フェードアウトの有無

		pWrite->Write("	SIZE = %.2f %.2f %.2f\n", m_size.x, m_size.y, m_size.z);		// 中心位置の書き込み
		pWrite->Write("	COL = %.2f %.2f %.2f %.2f\n", m_col.r, m_col.g, m_col.b, m_col.a);		// 色の変化量

		pWrite->Write("	MOVE_SIZE = %.2f %.2f %.2f\n", m_moveSize.x, m_moveSize.y, m_moveSize.z);		// サイズの変化量
		pWrite->Write("	MOVE_ROT = %.2f %.2f %.2f\n", m_moveSize.x, m_moveSize.y, m_moveSize.z);		// 回転の変化量
		pWrite->Write("	MOVE_COL = %.2f %.2f %.2f %.2f\n", m_moveCol.r, m_moveCol.g, m_moveCol.b, m_moveCol.a);		// 色の変化量
		pWrite->Write("	LIFE = %d\n", m_particleLife);		// 生存カウント

		pWrite->Write("	GRAVITY = %.2f\n", m_fGravity);		// 重力
		pWrite->Write("	RESISTANCE = %.2f\n", m_fResistance);		// 生存カウント
		pWrite->Write("	SPEED = %.2f\n", m_fMaxSpeed);

		pWrite->Write("	CENTER_POS = %.2f %.2f %.2f\n", m_centerPos.x, m_centerPos.y, m_centerPos.z);		// 中心位置
		pWrite->Write("	ANGLE = %.2f\n", m_fAngle);		// 角度
		pWrite->Write("	DISTANCE = %.2f\n", m_fDistance);		// 距離
		pWrite->Write("	ROTATION_SPEED = %.2f\n", m_fRotationSpeed);		// 回転速度
		pWrite->Write("	SPRITE = %.2f %.2f %.2f\n", m_sprite.x, m_sprite.y);		// テクスチャ分割数

		pWrite->Write("END_EFFECTSET\n");			// 頂点情報の書き込み終了宣言

		pWrite->Write("END_SCRIPT\n");			// スクリプトの終了宣言
		pWrite->End();
	}
}

//=============================================================================
// Uiのアドレス読み込み
//=============================================================================
void CDebugProc::LoadAddWithUI(void)
{
	FILE *pFile;
	char cReadText[128];		//文字
	char cHeadText[128];		//比較
	char cDie[128];
	int nCntPointer = 0;		//ポインターの数値

	char sAdd[64];				//モデルのアドレス
	std::string Add;

	int nCntMotion = 0;			//参照するポインタの値を初期化
	int nCntKey = 0;

	int nMaxModel = 0;

	//テキストデータロード
	pFile = fopen(UIMANAGER_SCRIPT, "r");

	if (pFile != NULL)
	{
		//ポインターのリセット
		nCntPointer = 0;

		//スクリプトが来るまでループ
		while (strcmp(cHeadText, "SCRIPT") != 0)
		{
			fgets(cReadText, sizeof(cReadText), pFile);
			sscanf(cReadText, "%s", &cHeadText);
		}

		//スクリプトだったら
		if (strcmp(cHeadText, "SCRIPT") == 0)
		{
			//エンドスクリプトが来るまで
			while (strcmp(cHeadText, "END_SCRIPT") != 0)
			{
				fgets(cReadText, sizeof(cReadText), pFile);
				sscanf(cReadText, "%s", &cHeadText);

				//改行
				if (strcmp(cReadText, "\n") != 0)
				{
					if (strcmp(cHeadText, "UI_FILENAME") == 0)
					{//パーツモデルのアドレス情報のとき
						sscanf(cReadText, "%s %s %s", &cDie, &cDie, &sAdd[0]);						//アドレスの取得
						Add = sAdd;
						m_AddUi.push_back(Add);
					}
				}
			}
		}

		//ファイル閉
		fclose(pFile);
	}
	else
	{

	}
}