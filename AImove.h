//=============================================================================
//
// AI動き処理 [AImove.h]
// Author : Seiya Takahashi
//
//=============================================================================
#ifndef _AIMOVE_H_
#define _AIMOVE_H_

//=============================================================================
// インクルードファイル
//=============================================================================
#include "main.h"

//=============================================================================
// 前方宣言
//=============================================================================
class CObject;

//=============================================================================
// クラス定義
//=============================================================================
class CAImove
{
public:
	CAImove();				// コンストラクタ
	~CAImove();				// デストラクタ
	HRESULT Init(void);		// 初期化処理
	void Uninit(void);		// 開放処理
	void Update(void);		// 更新処理

	D3DXVECTOR3 GetPos(void) { return m_pos; }		// 位置の取得
	static CAImove *Create(void);	// クリエイト処理

private:
	static std::vector<CObject*> m_pointObj;
	D3DXVECTOR3 m_pos;			// ライトの位置
};
#endif
