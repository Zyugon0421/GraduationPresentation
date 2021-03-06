//=============================================================================
//
// スクリプト書き込み処理 [write.cpp]
// Author : masayasu wakita
//
//=============================================================================
//=============================================================================
// インクルードファイル
//=============================================================================
#include "write.h"

//=============================================================================
// マクロ定義
//=============================================================================
#define COMMENT_BLOCK "#==============================================================================\n"

//=============================================================================
// コンストラクタ
//=============================================================================
CWrite::CWrite()
{
	m_pFile = NULL;
}

//=============================================================================
// コンストラクタ
//=============================================================================
CWrite::~CWrite()
{

}

//=============================================================================
// 開くなければ生成する処理
//=============================================================================
bool CWrite::Open(const std::string &add)
{
	if (m_pFile != NULL) End();

	// テキストデータロード
	m_pFile = fopen(add.c_str(), "w");

	if (m_pFile == NULL)
	{// 読み込めなかったとき
		MessageBox(NULL, "ファイルの読み込みに失敗!", "警告！", MB_ICONWARNING);
	}

	return true;
}

//=============================================================================
// 書き込み処理
//=============================================================================
bool CWrite::Write(const char* frm, ...)
{
	if (m_pFile == NULL) return false;				// ファイルが無ければ終わり

	va_list args;			// リストの取得
	char text[64];

	va_start(args, frm);		// リストの先頭を取得
	vsprintf(text, frm, args);
	va_end(args);						// リストを開放する

	// 書き込み
	fputs(text, m_pFile);

	return true;
}

//=============================================================================
// タイトルの書き込み
//=============================================================================
bool CWrite::TitleWrite(const char* frm, ...)
{
	if(m_pFile == NULL) return false;

	va_list args;			// リストの取得
	std::string write;
	char text[64];

	//コメントブロック//
	write = COMMENT_BLOCK;

	//コメント改行
	write += "#\n";

	va_start(args, frm);		// リストの先頭を取得
	vsprintf(text, frm, args);
	va_end(args);						// リストを開放する

	// 内容書き込み
	write += text;

	//コメント改行
	write += "#\n";

	//コメントブロック//
	write += COMMENT_BLOCK;

	// 書き込み
	fputs(write.c_str(), m_pFile);

	return true;
}

//=============================================================================
// 見出しの書き込み
//=============================================================================
bool CWrite::IndexWrite(const char* frm, ...)
{
	if (m_pFile == NULL) return false;				// ファイルが無ければ終わり

	va_list args;			// リストの取得
	std::string write;
	char text[64];

	//コメントブロック//
	write = COMMENT_BLOCK;

	va_start(args, frm);		// リストの先頭を取得
	vsprintf(text, frm, args);
	va_end(args);				// リストを開放する

	// 内容書き込み
	write += "# ";
	write += text;
	write += "\n";

	//コメントブロック//
	write += COMMENT_BLOCK;

	// 書き込み
	fputs(write.c_str(), m_pFile);

	return true;
}

//=============================================================================
// 改行
//=============================================================================
void CWrite::NewLine(void)
{
	// 書き込み
	fputs("\n", m_pFile);
}

//=============================================================================
// 終了処理
//=============================================================================
bool CWrite::End(void)
{
	if (m_pFile == NULL) return true;				// ファイルが無ければ終わり

	//ファイル閉
	if (EOF == fclose(m_pFile))
	{
		return false;
	}

	return true;
}