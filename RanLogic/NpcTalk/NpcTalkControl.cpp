#include "../pch.h"
#include <algorithm>
#include "../../SigmaCore/File/SerialFile.h"
#include "../../EngineLib/Common/StlFunctions.h"

#include "./NpcTalk.h"
#include "./NpcTalkControl.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////////////
//
CNpcTalkControl::CNpcTalkControl ()
{
}

CNpcTalkControl::~CNpcTalkControl ()
{
	RemoveTalk ();
}
/*
BOOL CNpcTalkControl::SaveCSVFile ( std::fstream &SFile )
{
//	SFile << (WORD)VERSION  <<",";

//	SFile << (int)m_mapTalk.size () <<",";
	NPCMAP_IT iter = m_mapTalk.begin();
	NPCMAP_IT iter_end = m_mapTalk.end();	
	for ( ; iter!=iter_end; ++iter )
	{
		SNpcTalk* pNpcTalk = (*iter).second;
		pNpcTalk->SAVECSV ( SFile );
	}

	return TRUE;
}
*/
BOOL CNpcTalkControl::SaveFile ( sc::SerialFile& SFile, CString& strAnswerBuffer, const DWORD dwDlgID, const DWORD dwCnt )
{
	CString strName, strFind = "\r\n";
	
	if ( !SFile.IsOpen () )
	{
		GASSERT ( 0 && "파일이 열려있지 않습니다." );
		return FALSE;
	}

	SFile << (WORD) VERSION;
	SFile << (int) m_mapTalk.size ();	
	
	for (NPCMAP_IT iter=m_mapTalk.begin(); iter!=m_mapTalk.end(); ++iter)
	{
		std::tr1::shared_ptr<SNpcTalk> spNpcTalk = iter->second;
		spNpcTalk->SAVE(SFile);

		CString strAnswer;													// MAIN, Conversation, Answer
		strAnswer.Format( _T("ND\t%s\t%d\t%d\t%d"), spNpcTalk->m_strTalk.c_str(), dwDlgID, dwCnt, spNpcTalk->m_dwNID );

		int nFind;
		while( ( nFind = strAnswer.Find( strFind ) ) != -1 )
		{
			strAnswer.Delete( nFind, strFind.GetLength() );
			strAnswer.Insert( nFind, "\\r\\n" );
		}
		strAnswer.Append( "\r\n" );
		strAnswerBuffer += strAnswer;
	}
	return TRUE;
}

BOOL CNpcTalkControl::LOAD_0001(sc::BaseStream& SFile)
{
	int nCount = 0;
    SFile >> nCount;
	for ( int i = 0; i < nCount; i++ )
	{
		std::tr1::shared_ptr<SNpcTalk> spNpcTalk(new SNpcTalk);
		spNpcTalk->LOAD(SFile);
		AddTalk(spNpcTalk);
	}
	return TRUE;
}

BOOL CNpcTalkControl::LOAD_0002( sc::BaseStream& SFile )
{
	int nCount = 0;
    SFile >> nCount;
	for ( int i = 0; i < nCount; i++ )
	{
		std::tr1::shared_ptr<SNpcTalk> spNpcTalk(new SNpcTalk);
		spNpcTalk->LOAD(SFile);
		AddTalk(spNpcTalk);
	}
	return TRUE;
}

BOOL CNpcTalkControl::LoadFile ( sc::BaseStream& SFile )
{
	if ( !SFile.IsOpen () )
	{
		GASSERT ( 0 && "파일이 열려있지 않습니다." );
		return FALSE;
	}

	WORD wVer = 0;
	SFile >> wVer;

	switch ( wVer )
	{
	case 0x0001:	LOAD_0001(SFile);	break;
	case 0x0002:	LOAD_0002(SFile);	break;
	default:
		break;
	};

	return TRUE;
}

void CNpcTalkControl::RemoveTalk()
{
	//std::for_each(m_mapTalk.begin(), m_mapTalk.end(), std_afunc::DeleteMapObject());
	m_mapTalk.clear();
}

DWORD CNpcTalkControl::MakeTalkNID ()
{
	DWORD dwNID = 0;

	NPCMAP_IT iter = m_mapTalk.begin();
	NPCMAP_IT iter_end = m_mapTalk.end();	
	for ( ; iter!=iter_end; ++iter )
	{
		std::tr1::shared_ptr<SNpcTalk> spNpcTalk = iter->second;
		if (dwNID < spNpcTalk->m_dwNID)
			dwNID = spNpcTalk->m_dwNID;
	}
	return (dwNID+1);
}

BOOL CNpcTalkControl::AddTalk(std::tr1::shared_ptr<SNpcTalk> spTalk)
{
	NPCMAP_IT found = m_mapTalk.find(spTalk->m_dwNID);
	if (found == m_mapTalk.end())
	{
		m_mapTalk.insert(NPCMAP_VALUE(spTalk->m_dwNID, spTalk));
		return TRUE;
	}

	return FALSE;
}

void CNpcTalkControl::DelTalk(DWORD NativeID)
{
	NPCMAP_IT found = m_mapTalk.find(NativeID);
	if (found != m_mapTalk.end())
	{
		std::tr1::shared_ptr<SNpcTalk> spNpcTalk = found->second;
		//SAFE_DELETE(spNpcTalk);
		m_mapTalk.erase(found);
	}
}

std::tr1::shared_ptr<SNpcTalk> CNpcTalkControl::GetTalk(DWORD NativeID)
{
	NPCMAP_IT found = m_mapTalk.find(NativeID);
	if (found != m_mapTalk.end())
    {
		return found->second;
    }
    else
    {
        static std::tr1::shared_ptr<SNpcTalk> NullPtr;
	    return NullPtr;
    }
}

std::tr1::shared_ptr<SNpcTalk> CNpcTalkControl::GetTalk2(DWORD NativeNum)
{
	int iFoundNum = 0;

	NPCMAP_IT iter = m_mapTalk.begin();
	NPCMAP_IT iter_end = m_mapTalk.end();	
	for ( ; iter!=iter_end; ++iter )
	{
		if (iFoundNum == NativeNum)
			return iter->second;
		iFoundNum++;
	}

    static std::tr1::shared_ptr<SNpcTalk> NullPtr;
	return NullPtr;
}

DWORD CNpcTalkControl::ToUp ( DWORD dwNID )
{
	if (m_mapTalk.empty())
        return UINT_MAX;

	NPCMAP_IT iter = m_mapTalk.find(dwNID);
	if (iter==m_mapTalk.begin())
        return UINT_MAX;

	NPCMAP_IT prev = iter;
	--prev;

    std::tr1::shared_ptr<SNpcTalk> spTalk_iter = iter->second;
	std::tr1::shared_ptr<SNpcTalk> spTalk_prev = prev->second;

	DWORD dwNID_iter = spTalk_iter->m_dwNID;
	DWORD dwNID_prev = spTalk_prev->m_dwNID;

	spTalk_prev->m_dwNID = dwNID_iter;
	iter->second = spTalk_prev;

	spTalk_iter->m_dwNID = dwNID_prev;
	prev->second = spTalk_iter;

	return spTalk_iter->m_dwNID;
}

DWORD CNpcTalkControl::ToDown(DWORD dwNID)
{
	if (m_mapTalk.empty())
        return UINT_MAX;

	NPCMAP_IT iter = m_mapTalk.find(dwNID);
	if (iter == m_mapTalk.end())
        return UINT_MAX;

	std::tr1::shared_ptr<SNpcTalk> spTalk_iter = iter->second; 
	std::tr1::shared_ptr<SNpcTalk> spTalk_end = m_mapTalk.rbegin()->second;

	if (spTalk_iter->m_dwNID == spTalk_end->m_dwNID)
        return UINT_MAX;
	
    NPCMAP_IT next = iter;
	++next;
	
	std::tr1::shared_ptr<SNpcTalk> spTalk_next = next->second;

	DWORD dwNID_iter = spTalk_iter->m_dwNID;
	DWORD dwNID_next = spTalk_next->m_dwNID;

	spTalk_next->m_dwNID = dwNID_iter;
	iter->second = spTalk_next;

	spTalk_iter->m_dwNID = dwNID_next;
	next->second = spTalk_iter;

	return spTalk_iter->m_dwNID;
}

CNpcTalkControl& CNpcTalkControl::operator= ( const CNpcTalkControl& rNpcTalkControl )
{
	NPCMAP_CIT iter = rNpcTalkControl.m_mapTalk.begin();
	NPCMAP_CIT iter_end = rNpcTalkControl.m_mapTalk.end();
	for ( ; iter!=iter_end; ++iter )
	{
		std::tr1::shared_ptr<SNpcTalk> spNpcTalk = iter->second;
		std::tr1::shared_ptr<SNpcTalk> spNewTalk(new SNpcTalk);
		*spNewTalk.get() = *spNpcTalk.get();
		AddTalk(spNewTalk);
	}
	return *this;
}

CNpcTalkControl::NPCMAP* CNpcTalkControl::GetTalkMap ()
{
	return &m_mapTalk;
}

DWORD CNpcTalkControl::GetTalkNum ()
{
	return (DWORD) m_mapTalk.size();
}