//=============================================================================
//
// プレイヤー処理 [player.h]
// Author : masayasu wakita
//
//=============================================================================
#ifndef _PLAYER_H_
#define _PLAYER_H_

//=============================================================================
// インクルード
//=============================================================================
#include "main.h"
#include "character.h"
#include "box.h"

//=============================================================================
// マクロ定義
//=============================================================================
#define MAX_KEY 30
#define NORMAL_SPEED 6.0f
#define SP_SPEED 2.0f

//=============================================================================
// 前方宣言
//=============================================================================
class CMeshCapsule;
class CMeshCube;
class CMeshOrbit;
class CColliderSphere;
class CColliderBox;
class CScene2D;
class CModel;
class CNumber;
class CDistanceNext;
class CUi;
class CShadow;

//=============================================================================
// クラス定義
//=============================================================================
class CPlayer : public CCharacter
{
public:
	//モーションタイプ
	typedef enum
	{
		ANIMATIONTYPE_NONE = -1,				// なし
		ANIMATIONTYPE_OVERTAKE_RIGHT,			// 追い抜く(右向き)
		ANIMATIONTYPE_OVERTAKE_LEFT,			// 追い抜く(左向き)
		ANIMATIONTYPE_RUN,						// 走る
		ANIMATIONTYPE_ATTACK_1,					// 基本攻撃1
		ANIMATIONTYPE_DAMAGE,					// ダメージ
		ANIMATIONTYPE_DETH,						// 死亡
		ANIMATIONTYPE_MAX						// アニメーションの最大数
	} ANIMATIONTYPE;							// アニメーションタイプ

	typedef enum
	{
		DRIFT_RIGHT = 0,		// ドリフト右
		DRIFT_LEFT,				// ドリフト左
		DRIFT_MAX				// ドリフト最大
	} DRIFTTYPE;				// ドリフト種類

	CPlayer(PRIORITY obj);						// プレイヤーのコンストラクタ(オブジェクトタイプ)
	~CPlayer();									// プレイヤーのデストラクタ
	HRESULT Init(void);							// 初期化処理
	void Uninit(void);							// 開放処理
	void Update(void);							// 更新処理
	void Draw(void);							// 描画処理

	static CPlayer *Create(void);				// プレイヤー生成
	static HRESULT Load(void);					// 素材データの取得
	void SetDeathblow(float nValue);			// 必殺技ポイント数の設定
	void SetEvent(bool bValue);					// イベントフラグの設定
	void SetGoalState(bool bValue);				// ゴールフラグの設定

	void OnTriggerEnter(CCollider *col);
	void OnCollisionEnter(CCollider *col);
	void ShowInspector(void) {};

	void BehaviorForMaxFrame(void);				// 最大フレーム数に到達したときの処理
	void BehaviorForMaxKey(void);				// 最大キー数に到達したときの処理

	float GetDeathblow(void) { return m_fDeathblow; }				// 必殺技ポイント数の取得
	D3DXVECTOR3 GetMove(void) { return m_move; }					// 移動量の取得
	D3DXMATRIX GetMtxWorld(void) { return m_mtxWorld; }				// ワールドマトリックスの取得
	D3DXVECTOR3 GetRotDest(void) { return m_dest; }					// 回転最終到達地点
	D3DXVECTOR3 GetCameraRot(void) { return m_cameraRot; }			// カメラの回転情報
	CDistanceNext *GetDistanceNext(void) { return m_pDistanceNext; }// 次の目標との距離を取得
	void SetCameraRot(D3DXVECTOR3 cameraRot) { m_cameraRot = cameraRot; }
	void SetSpeed(int nCntSpeed);
	void SetTurning(int nCntTurning);
	int GetNumRound(void) { return m_nRound; }
	bool GetEvent(void ) { return m_bEvent; }
	bool GetGoalState(void) { return m_bGoal; }

private:
#ifdef _DEBUG
	void Debug(void);																// デバッグ処理関数
#endif

	void MoveNearEnemy(void);														// 近くにいる敵に移動する処理
	void Collision(void);															// 当たり判定処理
	void Input(void);																// キー入力情報関数
	bool CollisionWall(void);														// 壁の当たり判定
	bool CollisionWallWithRay(void);												// レイによる壁の当たり判定
	bool SlopeMove(void);															// 坂の処理

	/*================= プレイヤー関連 =================*/
	D3DXVECTOR3						m_dest;											// モデルの最終到達点
	D3DXVECTOR3						m_difference;									// モデルの最大回転
	D3DXVECTOR3						m_move;											// 移動量
	D3DXVECTOR3						m_size;											// 大きさ
	D3DXVECTOR3						m_rot;											// 回転量
	D3DXVECTOR3						m_cameraRot;									// カメラの回転情報
	D3DXCOLOR						m_color;										// 色
	D3DXVECTOR3						m_vectorOld;									// 前回の方向ベクトル
	float							m_fSpeed;										// スピード
	float							m_fPuzzleSpeed[Piece_Num];						// パズルのスピード
	float							m_fPuzzleMaxSPeed;								// パズルのスピード
	float							m_fPuzzleRate[Piece_Num];						// スピード上昇率
	float							m_fPuzzleMaxRate;								// スピード上昇率
	float							m_fPuzzleTurning[Piece_Num];					// 旋回速度
	float							m_fPuzzleMaxTurning;							// 旋回速度
	float							m_fPuzzleDecay[Piece_Num];						// 減衰率取得
	float							m_fPuzzleMaxDecay;								// 減衰率取得
	int								m_fPuzzlePower[Piece_Num];						// パワー
	float							m_fPuzzleMaxPower;								// パワー
	float							m_fDeathblow;									// 必殺技ポイント
	float							m_fAcceleration;								// 加速度
	float							m_fSlopeSpeed;									// 坂の速度
	int								m_nLife;										// 体力
	int								m_nActionCount;									// 次のアクションまでのカウンタ
	int								m_nParticleCount;								// パーティクル生成までのカウンタ
	int								m_nPointNum;									// ポイント番号
	bool							m_bJump;										// ジャンプ
	bool							m_bEvent;										// イベント発生フラグ
	bool							m_bGoal;										// ゴールフラグ
	bool							m_bColliderWithWall;							// 壁の当たり判定
	bool							m_bHit;											// ヒット判定
	bool							m_bDrift[DRIFT_MAX];							// プレイヤーのドリフトフラグ
	bool							m_bMove;										// 現在動いているかのフラグ
	bool							m_bAccel;										// アクセルを押しているかどうか
	bool							m_bRankingSign[Piece_Num];
	bool							m_bRanking;
	bool							m_bSlope;										// 坂にいるかどうかフラグ
	CNumber							*m_pRank;										// ランキング用UI
	CDistanceNext					*m_pDistanceNext;								// 次のプレイヤーとの距離のUI
	CShadow							*m_pShadow;										// 影の情報ポインタ

	// レースゲーム関連
	int								m_nRound;										// 現在の周回回数

	/*=============== 3Dレンダリング関連 ===============*/
	LPDIRECT3DVERTEXBUFFER9			m_pVtxBuff;										// 頂点バッファへのポインタ
	LPD3DXMESH						m_pMesh;										// メッシュ情報へのポインタ
	DWORD							m_nNumMat;										// マテリアル情報の数
	LPD3DXBUFFER					m_pBuffMat;										// マテリアル情報へのポインタ
	D3DXMATRIX						m_mtxWorld;										// ワールドマトリックス

	CMeshOrbit						*m_pMeshOrbit;									// 軌跡のポインタ

	/*===============当たり判定===============*/
	CColliderSphere					*m_pColPlayerSphere;							// プレイヤーの当たり判定のポインタ
};
#endif