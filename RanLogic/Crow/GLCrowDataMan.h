#ifndef _GL_CROW_DATA_MAN_H_
#define _GL_CROW_DATA_MAN_H_

#pragma once

#include <string> // "./GLCrowData.h"
#include <boost/function.hpp>
#include "../../EngineLib/G-Logic/GLDefine.h"
#include "../../InternalCommonLib/dxstdafx.h"

namespace sc
{   
    class Excel;
}

struct SCROWDATA;

class GLCrowDataMan
{
public:
    enum
    {
        ENCODE_VER = 0x0102,

        VERSION = 0x0102,
        MAX_CROW_MID = 1024,
        MAX_CROW_SID = 1024
    };

public:
    GLCrowDataMan();
    ~GLCrowDataMan();
    static GLCrowDataMan& GetInstance();

public:
    static const char*	_FILEHEAD;
    static const char*	_LOGFILE;
    static const char*	_STRINGTABLE;

protected:
    char				m_szFileName[MAX_PATH];
    SCROWDATA***		m_ppPCROWDATA;
    bool				m_bModify;
    LPDIRECT3DDEVICEQ	m_pd3dDevice;

public:
    SCROWDATA* GetMainCrowData(WORD wMainID);

    SCROWDATA* GetCrowData(WORD wMID, WORD wSID);
    SCROWDATA* GetCrowData(const SNATIVEID& sNativeID);
    SCROWDATA* GetCrowData(DWORD NpcId);

    const std::string GetCrowName(WORD wMID, WORD wSID);
    const std::string GetCrowName(SNATIVEID sNativeID);
    const std::string GetCrowName(DWORD NpcId);

    bool	ValidData();
    bool	IsModify() const { return m_bModify; }

    //! 해당 Talk 가 어디서 사용되는지 조사한다.
    //! 전체 검색/속도 느림/툴에서만 사용하세요.
    SCROWDATA* IsUsingTalkFile(const std::string& TalkFile);

    WORD FindFreeCrowDataMID ();
    WORD FindFreeCrowDataSID ( WORD wMainID );
	size_t GetCrowMaxMID() {return MAX_CROW_MID;}
	size_t GetCrowMaxSID()	 {return MAX_CROW_SID;}

    BOOL InsertCrowData ( WORD wMID, WORD wSID, SCROWDATA* pCrowData, bool binner=false );
    BOOL InsertCrowData ( SNATIVEID sNativeID, SCROWDATA* pCrowData, bool binner=false )
    {
        return InsertCrowData ( sNativeID.wMainID, sNativeID.wSubID, pCrowData, binner );
    }

    BOOL DeleteCrowData ( WORD wMID, WORD wSID );
    BOOL DeleteCrowData ( SNATIVEID sNativeID );

    HRESULT OneTimeSceneInit ();
    HRESULT FinalCleanup ();

    HRESULT	SyncStringTable();
    HRESULT SyncUpdateData ();
    
    HRESULT SaveFile(const char* szFile);
    HRESULT SaveCsvFile( CWnd* pWnd );
    void SaveCrowDataSimple(sc::Excel& Xls, int Sheet, const std::string& SheetName);

    HRESULT LoadFile(const std::string& FileName, BOOL bServer, bool bPastLoad, boost::function<void (std::string)> PercentCallbackFunc=NULL);
    HRESULT LoadCsvFile( CWnd* pWnd );

	HRESULT LoadStrTblExcelFile(const char* szPath);
	HRESULT SaveStrTblExcelFile(std::string& strSavePath);
};

#endif // _GL_CROW_DATA_MAN_H_