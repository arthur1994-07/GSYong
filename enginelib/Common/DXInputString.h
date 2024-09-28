//-----------------------------------------------------------------------
//
//		CLASS   DXInputString
//
//		Using IME -
//		Link : "imm32.lib"
//
//		coding : 2000/07/11 ~ 12
//		by jdh
//
//		- 수정 : 2000/12/15 - 문자열 입력시 Input ScanCode 기능 제한.
//		- 수정 : 2000/12/21 - TAB, ESCAPE, RETURN 키 인식.
//                          - TAB 문자 무시.
//		- 수정 : 2002/08/08 - 싱글턴 페턴으로 수정.
//		- 수정 : 2002/08/09 - OnChar() 문제, ImmGetOpenStatus(m_hIMC)의
//							상태 값에 따라 입력 여부를 결정하게 수정.
//
//
//-----------------------------------------------------------------------
#pragma once

#include <Imm.h>
#include <string>
#include "../ServiceProviderDefine.h"
#include "IMEEdit.h"

enum EMLANGFLAG
{
	EMLANGFLAG_DEFAULT	= 0,
	EMLANGFLAG_CHN_TRA	= 1,
	EMLANGFLAG_JP		= 2,
	EMLANGFLAG_KR		= 3,
	EMLANGFLAG_CHN_SIM	= 4,
	EMLANGFLAG_TH		= 5
};

enum EMIMETYPE
{
	EMIMETYPE_DEFAULT   = 0,
	EMIMETYPE_MULTILINE = 1,
};

class DXInputString
{
public:
    DXInputString(EMSERVICE_PROVIDER emServiceProvider);
    virtual ~DXInputString();
    //static DXInputString& GetInstance();

public:
	static BOOL	m_bCaratMove;

protected:
	BOOL		m_bOn;
	std::string	m_strString;
	CWnd*		m_pParentWnd;
	CIMEEdit	m_editIME;
	CIMEEdit	m_editIMEMulti;
	CIMEEdit*	m_peditIME;
    EMSERVICE_PROVIDER m_emServiceProvider;

public:
	void Create		( CWnd *_pWnd, CRect &rtPosition );
	void Move		( CRect &rtPosition );

public:
	BOOL OnInput ( const EMIMETYPE emType=EMIMETYPE_DEFAULT );
	BOOL OffInput ();
	BOOL IsOn ();

public:
	bool CheckEnterKeyDown ()					{ return m_peditIME->CheckEnterKeyDown(); }
	bool CheckComposition ()					{ return m_peditIME->CheckComposition(); }
	bool CheckWideCaret ()						{ return m_peditIME->CheckWideCaret(); }

public:
	int GetLanguage ()							{ return m_peditIME->GetLanguage(); }

	void ConverseNativeToEng()					{ m_peditIME->ConverseNativeToEng(); }
	void ConverseEngToNative()					{ m_peditIME->ConverseEngToNative(); }
	void ConverseFullToHalf()					{ m_peditIME->ConverseFullToHalf(); }

	BOOL IsNativeMode ()						{ return m_peditIME->IsNativeMode(); }

public:
	const char* GetString ()					{ return m_peditIME->GetString(); }
    void SetString ( const char* pStr = NULL )	{ m_peditIME->SetString( pStr ); }
	void InsertString ( const char* pStr = NULL ) { m_peditIME->InsertString( pStr ); }

public:
	int GetInsertPos()							{ return m_peditIME->GetInsertPos(); }
	int SetInsertPos( int xPos )				{ return m_peditIME->SetInsertPos( xPos ); }

public:
	bool IsOnChar	 ()							{ bool bOn = m_peditIME->IsOnChar();	m_peditIME->RefreshOnChar();	return bOn; }
	UINT IsOnKeyDown ()							{ UINT nOn = m_peditIME->IsOnKeyDown(); m_peditIME->RefreshOnKeyDown(); return nOn; }

public:
	void DisableKeyInput()						{ m_peditIME->DisableKeyInput(); }
	void UsableKeyInput()						{ m_peditIME->UsableKeyInput(); }
};
