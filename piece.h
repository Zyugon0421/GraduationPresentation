//==================================================================================================================
//
// ピース[piece.h]
// Author:Ryouma Inoue
//
//==================================================================================================================
#ifndef _PIECE_H_
#define _PIECE_H_

//==================================================================================================================
// インクルードファイル
//==================================================================================================================
#include "main.h"
#include "scene.h"

//==================================================================================================================
// マクロ定義
//==================================================================================================================
#define Piece_Depth 8																// 縦のポリゴン数
#define Piece_Width 15																// 横のブロック数

//==================================================================================================================
// 前方宣言
//==================================================================================================================
class CPlayer;
class CScene2D;

//==================================================================================================================
//
// クラスの定義
//
//==================================================================================================================
class CPiece : public CScene
{
public:
	enum PieceStatus
	{
		PieceStatus_None = -1,
		PieceStatus_Plaacement,
		PieceStatus_Ok,
		PieceStatus_Not,
		PieceStatus_Max
	};

	CPiece(PRIORITY type);															// コンストラクタ
	~CPiece();																		// デストラクタ
	HRESULT Init(void);																// 初期化処理
	void Uninit(void);																// 終了処理
	void Update(void);																// 更新処理
	void Draw(void);																// 描画処理

	void OnTriggerEnter(CCollider *col) {};
	void OnCollisionEnter(CCollider *col) {};
	bool GetPlaacement(void) { return m_bCreate; }									// 配置情報取得

	void SetPlaacement(bool bCreate) { m_bCreate = bCreate; }
	static CPiece *Create(void);													// 生成処理
	static HRESULT Load(void);														// テクスチャ情報ロード
	static void Unload(void);														// テクスチャ情報アンロード

protected:

private:
	static LPDIRECT3DTEXTURE9 m_pTexture;											// テクスチャへのポインタ
	bool m_bPuzzle[Piece_Depth][Piece_Width];										// 使用しているかどうか
	CScene2D *m_pBlock[Piece_Depth][Piece_Width];									// シーン2Dのポインタ

	D3DXVECTOR3 m_pos;																// 位置

	int m_nCntMove_X;																// 移動カウントX
	int m_nCntMove_Y;																// 移動カウントY
	PieceStatus m_Status;															// ピースの状態
	bool m_bPlacement;																// 配置しているかどうか
	bool m_bCreate;																	// 生成するかどうか
	bool m_bMove;																	// 動くかどうか
};
#endif