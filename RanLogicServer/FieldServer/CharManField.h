#ifndef _GLOGIC_SERVER_CHAR_MAN_FIELD_H_
#define _GLOGIC_SERVER_CHAR_MAN_FIELD_H_

#pragma once

#include <set>
#include "../../RanLogic/Network/NetLogicDefine.h"

class GLChar;
class CharNameCache;
struct CHAR_BASE_CACHE_DATA;

class CharManField
{
private:
    static float MAX_DELAY_TIME;
	static float FRAME_MOVE_DELAY;

public:
	CharManField(net::EMSERVERTYPE servertype);
    ~CharManField();

    typedef std::map<DWORD, DWORD>  PCID;
    typedef PCID::iterator	        PCID_ITER;
    typedef PCID::const_iterator    PCID_CITER;
	typedef PCID::value_type        PCID_VALUE;

    typedef std::map<std::string, GLChar*>	GLCHAR_MAP;    
    typedef GLCHAR_MAP::iterator            GLCHAR_MAP_ITER;
    typedef GLCHAR_MAP::const_iterator      GLCHAR_MAP_CITER;
    typedef GLCHAR_MAP::value_type          GLCHAR_MAP_VALUE;

    typedef std::map<DWORD, DWORD>    CLIENTMAP;
    typedef CLIENTMAP::iterator	      CLIENTMAP_ITER;
    typedef CLIENTMAP::const_iterator CLIENTMAP_CITER;
    typedef CLIENTMAP::value_type     CLIENTMAP_VALUE;


	typedef std::map<DWORD64, DWORD>	CLIENTMAP64;		//UNIQUEID, InstantID
    typedef CLIENTMAP64::iterator		CLIENTMAP64_ITER;
    typedef CLIENTMAP64::const_iterator	CLIENTMAP64_CITER;
    typedef CLIENTMAP64::value_type		CLIENTMAP64_VALUE;


protected:
    CRITICAL_SECTION m_SaveDbUserIdLock;
    PCID m_reqSaveDBUserID; //! DB 저장요청한 userid.

    CRITICAL_SECTION m_CreatingUserIdLock;
    PCID m_setCreatingChar;	//! 캐릭터를 생성중인 UserID 목록 - 캐릭터 생성을 처리중일 떄, 접속을 종료하고 다른 캐릭터로 접속하는 것을 방지(인도네시아 락커 해킹)

    GLCHAR_MAP		m_PCNameMap; //! Character Name/Pointer

    CLIENTMAP		m_mapCHARID; //! Character DB 번호/Gaea ID

	//mjeon.instance
	CLIENTMAP64		m_mapCHARID64;	//UniqueID <-> GaeaID

	net::EMSERVERTYPE	m_ServerType;

    CharNameCache* m_pNameCache; //! 캐릭터 이름/캐릭터 DB 번호, 캐릭터 DB 번호/캐릭터 이름

	float m_fFrameMoveTimer;

public:
    //! DB 저장요청한 UserID
    void ReqSaveDbUserAdd(DWORD UserDbNum);
    void ReqSaveDbUserDel(DWORD UserDbNum);
    BOOL ReqSaveDbUserFind(DWORD UserDbNum);

    //! 캐릭터를 생성중인 UserID
    void CreatingCharAdd(DWORD UserDbNum);
    void CreatingCharDel(DWORD UserDbNum);
    BOOL CreatingCharFind(DWORD UserDbNum);

    //! 캐릭터이름/Pointer
    GLChar* GetChaByName(const std::string& ChaName);
    bool    ChaNameExist(const std::string& ChaName);
    void    ChaNameDel(const std::string& ChaName);
    void    ChaNameAdd(const std::string& ChaName, GLChar* pChar);

    //! Character DB 번호/Gaea ID
    DWORD GetGaeaId(DWORD ChaDbNum);
    void  ChaDbNumAdd(DWORD ChaDbNum, DWORD GaeaId);
    void  ChaDbNumDel(DWORD ChaDbNum);
    const CLIENTMAP& GetCharIDMap() { return m_mapCHARID; }

	// 64-bit UniqueID / 32-bit InstantID
    DWORD GetGaeaId(DWORD64& UniqueID);
	DWORD GetGaeaId(DWORD dwServerGroupNum, DWORD dwChaDbNum);
    void  ChaUniqueIDAdd(const DWORD64& UniqueID, DWORD GaeaId);
    void  ChaUniqueIDDel(const DWORD64& UniqueID);
    const CLIENTMAP64& GetCharIDMap64() { return m_mapCHARID64; }

    //! 캐릭터 이름/캐릭터 DB 번호, 캐릭터 DB 번호/캐릭터 이름
    void ChaNameDbNumAdd(const std::string& ChaName, DWORD ChaDbNum, DWORD UserDbNum, EMUSERTYPE UserType);
	void ChaNameDbNumAdd(const std::string& ChaName, DWORD ChaDbNum, DWORD UserDbNum, EMUSERTYPE UserType, int iClass, int iLv);

    //! 캐릭터 이름/캐릭터 DB 번호
    DWORD GetChaDbNumByChaName(const std::string& ChaName);

    //! 캐릭터 DB 번호/캐릭터 이름
    std::string GetChaNameByDbNum(DWORD ChaDbNum);

    std::tr1::shared_ptr<CHAR_BASE_CACHE_DATA> GetCharCacheData(const std::string& ChaName);
    std::tr1::shared_ptr<CHAR_BASE_CACHE_DATA> GetCharCacheData(DWORD ChaDbNum);

    void FrameMove( float fElapsedTime );
	void FrameMoveSaveCharacter();
	void FrameMoveCreateCharacter();
};

#endif // _GLOGIC_SERVER_CHAR_MAN_FIELD_H_