#ifndef GLQUESTMAN_H_
#define GLQUESTMAN_H_

#pragma once

#include <map>
#include "./GLQuest.h"
#include "../../SigmaCore/Memory/ObjectPool.h"

class GLQUEST;
class GLQUESTPROG;

class GLQuestMan
{
public:
	typedef std::multimap<DWORD,DWORD>				QUESTGEN;
	typedef QUESTGEN::iterator						QUESTGEN_ITER;
	typedef std::pair<QUESTGEN_ITER,QUESTGEN_ITER>	QUESTGEN_RANGE;

	typedef std::map<WORD,SNATIVEID>					RNATTENDANCEREWARD;
	typedef RNATTENDANCEREWARD::iterator			RNATTENDANCEREWARD_ITER;
	typedef std::pair<WORD, SNATIVEID> PAIR;

public:
	struct SNODE
	{
		std::string strFILE;
		GLQUEST*	pQUEST;

		SNODE()
            : pQUEST(NULL)
		{
		}

        ~SNODE()
        {
        }
		
		SNODE(const SNODE& sNODE)
		{
			strFILE = sNODE.strFILE;
			pQUEST = sNODE.pQUEST;
		}

		SNODE& operator= (const SNODE& sNODE)
		{
			strFILE = sNODE.strFILE;
			pQUEST = sNODE.pQUEST;
			return *this;
		}
	};

	struct SQUESTGROUP
	{
		DWORD m_dwFromQuestID;
		WORD m_nBeforeSchool;
		DWORD	m_dwClass;
		DWORD m_dwToQuestID;
		WORD m_nAfterSchool;

		SQUESTGROUP()
		{
			m_dwFromQuestID = 0;
			m_nBeforeSchool = 0;
			m_dwClass = 0;
			m_dwToQuestID = 0;
			m_nAfterSchool = 0;
		}

        ~SQUESTGROUP()
        {
        }

		SQUESTGROUP& operator= (const SQUESTGROUP& sQuest)
		{
			m_dwFromQuestID = sQuest.m_dwFromQuestID;
			m_nBeforeSchool = sQuest.m_nBeforeSchool;
			m_dwClass = sQuest.m_dwClass;
			m_dwToQuestID = sQuest.m_dwToQuestID;
			m_nAfterSchool = sQuest.m_nAfterSchool;

			return *this;
		}

		int Check(DWORD dwFromQuestId, WORD wBeforeSchool, DWORD dwClass, WORD wAfterSchool)
		{
			//if( m_dwFromQuestID == dwFromQuestId && m_nBeforeSchool == wBeforeSchool && m_nAfterSchool == wAfterSchool && ( m_dwClass != 0 ? dwClass == m_dwClass : true ) )
			if( m_dwFromQuestID == dwFromQuestId && m_nBeforeSchool == wBeforeSchool && m_nAfterSchool == wAfterSchool && ( m_dwClass != 0 ? (m_dwClass & dwClass) : true ) )
				return m_dwToQuestID;
			
			return -1;
		}
	};

public:
	enum { VERSION = 0x0001, };

	typedef std::map<DWORD,SNODE>		MAPQUEST;
	typedef MAPQUEST::iterator			MAPQUEST_ITER;
	typedef std::vector<SQUESTGROUP>	VEC_QUESTGROUP;
	typedef VEC_QUESTGROUP::iterator	VEC_QUESTGROUP_ITER;

private:
    GLQuestMan();

public:
    ~GLQuestMan();

    static GLQuestMan& GetInstance();
	static void ReleaseInstance();

public:
	// QUEST_BUFFER_OVER
	// ����Ʈ �ϳ��� ũ�Ⱑ �����̻��ΰ�� �������� Ŭ���̾�Ʈ�� �������� ���ϴ� ���� �߻� ( ���� ũ�� �̻��� ������ return )
	// ������ �ϸ� ������ �����ص� ���߿� �����ص� �뷮�� �Ѿ�� ��� �� ������ �߻���.
	// ��Ŷ�� �̹� �����ؼ� ������ ������ ������ �����͸� �����ص� ȿ���� ������...
	// �������� Ŭ���̾�Ʈ�� ����Ʈ ������ ������ ��ũ�����尡 �Ѱ��̹Ƿ� ���ÿ� ���� ����Ʈ�� ���ҵǾ� �ü� �����Ƿ� ���� ���ͳ� ���� ������� ����
	se::ByteStream	m_sQuestByteStream;
	int				m_QuestByteStreamID;

protected:
	std::string	m_strPATH;

public:
	void SetPath(const char* szPath) { m_strPATH = szPath; }
	const char* GetPath() const { return m_strPATH.c_str(); }
    
    const std::string& GetQuestName(const SNATIVEID& QuestId);
    const std::string& GetQuestName(WORD MainId, WORD SubId);
    const std::string& GetQuestName(DWORD QuestId);

protected:
	bool		m_bModify;
	MAPQUEST	m_mapQuestMap;

    QUESTGEN	m_mapStartItem;
	QUESTGEN	m_mapStartSkill;
	QUESTGEN	m_mapStartLevel;
	QUESTGEN	m_mapStartJoin;

	std::vector<DWORD>	m_vecAttendancAdditionalQuest;
	std::vector<DWORD>	m_vecAttendanceQuest;
	RNATTENDANCEREWARD m_mapRnattendanceReward;
	DWORD m_dwRnattendanceQuestId;
	GLQUESTPROG* m_pRnattendanceQuestProg;

	VEC_QUESTGROUP m_vecGroupQuest;

public:
	GLQuestMan::QUESTGEN_RANGE FindStartFromGetITEM(SNATIVEID nidITEM);
	GLQuestMan::QUESTGEN_RANGE FindStartFromGetSKILL(SNATIVEID nidSKILL);
	GLQuestMan::QUESTGEN_RANGE FindStartFromGetLEVEL(WORD wLEVEL);
	GLQuestMan::QUESTGEN_RANGE FindStartFromGetCharJOIN(int nFlag);

	void ShuffleAttendanceQuest( unsigned unSeed, std::vector<DWORD> &vecQuestID );
	std::vector<DWORD> FindAttendanceQuest( WORD wLevel, int nQuestNum, unsigned unSeed );
	bool IsDuplicateType( std::vector<DWORD> vecQuestID, DWORD dwQuestID, int emATTENDANCE_TYPE ); 

	void SetRnattenadenceReward(WORD wDayNum, WORD wMid, WORD wSid );
	bool LoadRnattenadenceReward(const std::string& pPath);
	DWORD GetRnattendanceRewardNDay(int nday);
	void SetRnattendanceGQuestProg(GLQUESTPROG* pQuestProg);
	GLQUESTPROG* GetRnattendanceQuestProg();
	DWORD GetRnattendanceQuestId();
	std::vector<DWORD> GetAttendancAdditionalQuest();

public:
	GLQUEST* Find(DWORD dwQuestID);
	MAPQUEST& GetMap() { return m_mapQuestMap; }
	int GetConvertQuestGroup(DWORD dwQuestID, WORD wBeforeSchool, DWORD dwClass, WORD wAfterSchool);

public:
	DWORD MakeNewQNID ();

public:
	void Insert ( DWORD dwID, SNODE sNODE );
	bool Delete ( DWORD dwQuestID );

protected:	
    GLQUEST* LoadQuest(const std::string& FileName, DWORD dwQuestID);
public:
	void Clean ();
	bool LoadGroupQuest(const std::string& FileName);

public:
    bool LoadFile(const std::string& FileName, bool bQUEST_LOAD);
	bool SaveFile(const char* szFile);
	std::string SaveSqlFile();
    void SaveQuestDataSimple(sc::Excel& Xls, int Sheet, const std::string& SheetName);
	void SaveStrTable ( const char* szFile );

	HRESULT LoadStringTableExcel(const char* szPath);
	HRESULT SaveStringTableExcel(std::string& strSavePath);

public:
	void DoModified ()		{ m_bModify = true; }
	void ResetModified ()	{ m_bModify = false; }
	bool IsModify ()		{ return m_bModify; }

public:
	sc::BOOST_OBJECT_POOL_LOCK_CHECKER<GLQUESTPROG> m_poolGLQuestProg;
};

#endif // GLQUESTMAN_H_