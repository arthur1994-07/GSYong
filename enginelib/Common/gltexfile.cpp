#include "pch.h"
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include "../../SigmaCore/String/basestring.h"
#include "../../SigmaCore/String/StringUtil.h"
#include "./GLTexFile.h"
#include "./gassert.h"
#include "./StlFunctions.h"
#include "../G-Logic/GLogic.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

gltexfile::gltexfile()
    : m_pFOUND_VEC(NULL)
    , m_bZipFile(FALSE)
{    
}

gltexfile::~gltexfile(void)
{
	clean();
}

void gltexfile::reg_sep(const std::string& Seperate)
{   
    m_Seperator = Seperate;
	//m_cSeperator.Register ( cVAL );
}

bool gltexfile::open(const std::string& strFile, bool bDEC, BOOL bPack )
{
	m_strFileName = strFile;
	m_pFOUND_VEC = NULL;

	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	_splitpath(m_strFileName.c_str(), NULL, NULL, fname, ext);
	StringCchCat(fname, _MAX_FNAME, ext);

    std::auto_ptr<sc::CBaseString> pBString(
        GLOGIC::openfile_basestring(
            m_bZipFile, 
            m_strZipFileName,
            m_strFileName.c_str(), 
            fname, 
            bDEC, 
            bPack));

	if (!pBString.get())
	{
		//CDebugSet::ToLogFile ( "ERROR : gltexfile::open(), File Open %s", fname );
		return false;
	}

    sc::CBaseString &cBString = *pBString;

	m_bZipFile = FALSE;
	
    typedef std::vector<std::wstring> VECSTRINGW;
    typedef VECSTRINGW::iterator VECSTRINGW_ITER;

    std::wstring Seperator(sc::string::ansiToUnicode(m_Seperator));
	CString strLine;
	while ( cBString.GetNextLine(strLine) )
	{
        std::wstring NewLine(sc::string::ansiToUnicode(strLine.GetString()));

		VECSTRINGW* pVECSTR = new VECSTRINGW;
		//m_cSeperator.DoSeparate ( strLine.GetString(), *pVECSTR );
        boost::split(
            *pVECSTR,
            NewLine,
            boost::algorithm::is_any_of(Seperator),
            boost::algorithm::token_compress_on);

        // 문자열 앞과 뒤에 seperator 가 존재하면 개수가 틀려진다. 공백문자열이 추가됨.
        // 최종적으로 공백문자열을 삭제해 준다.
        // 참고 URL : http://stackoverflow.com/questions/5389404/boost-split-usage
        // 참고 URL : http://stackoverflow.com/questions/5117201/multiple-split-tokens-using-boostis-any-of
        for (VECSTRINGW_ITER iter=pVECSTR->begin(); iter!=pVECSTR->end(); )
        {
            if ((*iter).empty())
                iter = pVECSTR->erase(iter);
            else
                ++iter;
        }

		VECSTRINGW_ITER iter = pVECSTR->begin();
		VECSTRINGW_ITER iter_end = pVECSTR->end();
		for ( ; iter!=iter_end; ++iter )
		{
            std::wstring &strVAL = (*iter);
			size_t nPOS = strVAL.find(L"//");
			
			//	문자열 앞부분 부터 주석 부분일 때.
			if ( 0==nPOS )
			{
				//	이 부분 부터 끝가지 지워줌.
				pVECSTR->erase ( iter, iter_end );
				break;
			}

			//	문자열 중간에 주석 부가 시작될 때.
            if ( std::wstring::npos!=nPOS )
			{
                std::wstring strTEMP(strVAL.begin(),strVAL.begin()+nPOS);
				strVAL = strTEMP;	//	주석 부분을 짤라낸다.

				pVECSTR->erase ( iter+1, iter_end );
				break;
			}
		}

		if ( true==pVECSTR->empty() )
		{
			delete pVECSTR;
			continue;
		}

		//DWORD dwNUM = (DWORD) pVECSTR->size();

        VECSTRING* pResult = new VECSTRING;
        for (VECSTRINGW_ITER iter=pVECSTR->begin(); iter!=pVECSTR->end(); ++iter)
        {
            std::string MbcsStr(sc::string::unicodeToAnsi(*iter));
            pResult->push_back(MbcsStr);
        }
		m_mapFlags.insert(MAPFLAG_VALUE((*pResult->begin()), pResult));
        delete pVECSTR;
	}

	return true;
}

void gltexfile::clean ()
{
	m_pFOUND_VEC = NULL;
	//m_cSeperator.Clear();
    m_Seperator.clear();
	std::for_each(m_mapFlags.begin(),m_mapFlags.end(), std_afunc::DeleteMapObject() );
	m_mapFlags.clear();
}

DWORD gltexfile::getflagnum ( const std::string& strFLAG )
{
	MAPFLAG_PAIR pair = m_mapFlags.equal_range ( strFLAG );
	if ( pair.first == pair.second )
        return 0;

	return (DWORD)std::distance ( pair.first, pair.second );
}

DWORD gltexfile::getparamnum ( const std::string& strFLAG )
{
	m_pFOUND_VEC = findflag ( strFLAG );
	if ( NULL==m_pFOUND_VEC )
	{
		//	여기서는 오류 출력을 하지 않는다.
		return NULL;
	}

	return (DWORD)m_pFOUND_VEC->size();
}

DWORD gltexfile::getparamnum ( const DWORD dwITER, const std::string& strFLAG )
{
	MAPFLAG_PAIR pair = m_mapFlags.equal_range ( strFLAG );

	MAPFLAG_ITER iter = pair.first;
	for ( DWORD i=0; i<dwITER; ++i )	++iter;

	m_pFOUND_VEC = (*iter).second;

	return (DWORD)m_pFOUND_VEC->size();
}

gltexfile::VECSTRING* gltexfile::findflag ( const std::string& strFlag )
{
	MAPFLAG_ITER iter = m_mapFlags.find ( strFlag );
	if ( iter!=m_mapFlags.end() )
	{
		m_pFOUND_VEC = (*iter).second;
		return m_pFOUND_VEC;
	}

    if ( m_strFileName.find( "default.charclass" ) != STRING::npos )
    {
        // Note : 현재 default.charclass에서만 flag가 없을 때 확인 메세지를 출력한다.
        char szErrorMsg[128] = "";
        StringCchPrintf( szErrorMsg, 128, "file : %s, keyword : %s, Not exist.",
            m_strFileName.c_str(), strFlag.c_str() );

        MessageBox( NULL, szErrorMsg, "Info", MB_OK );
    }

	return NULL;
}

gltexfile::VECSTRING* gltexfile::validflag(
    const std::string& strFLAG,
    const int nIDX,
    const int nSIZE,
    bool bMsgBox)
{
	if ( nIDX>=(nSIZE+1) )
	{
		char szErrorMsg[128] = "";
		StringCchPrintf( szErrorMsg, 128, "file : %s, keyword : %s, data number setting error.",
			m_strFileName.c_str(), strFLAG.c_str() );

        if (bMsgBox)
		    MessageBox(NULL, szErrorMsg, "ERROR", MB_OK);

		return NULL;
	}

	//	찾아놓은 스트링 백터가 지금 요청한 FLAG와 같은지 확인.
	if ( NULL!=m_pFOUND_VEC )
	{
		STRING &strVAL = (*m_pFOUND_VEC->begin());
		if ( strFLAG != strVAL )	m_pFOUND_VEC = NULL;
	}

	//	스트링 백터가 무효할 경우 다시 찾음.
	if ( NULL==m_pFOUND_VEC )
	{
		m_pFOUND_VEC = findflag ( strFLAG );
		if ( NULL==m_pFOUND_VEC )
		{
			//	여기서는 오류 출력을 하지 않는다.
			return NULL;
		}
	}

	if ( m_pFOUND_VEC->size() != (nSIZE+1) )
	{
		char szErrorMsg[128] = "";
		StringCchPrintf( szErrorMsg, 128, "file : %s, keyword : %s, data number setting error.",
			m_strFileName.c_str(), strFLAG.c_str() );
        if (bMsgBox)
		    MessageBox(NULL, szErrorMsg, "ERROR", MB_OK);
		return NULL;
	}

	return m_pFOUND_VEC;
}

gltexfile::VECSTRING* gltexfile::validflag(
    const DWORD dwITER,
    const std::string& strFLAG,
    const int nIDX,
    const int nSIZE,
    bool bMsgBox)
{
	if ( nIDX>=(nSIZE+1) )
	{
		char szErrorMsg[128] = "";
		StringCchPrintf(
            szErrorMsg,
            128,
            "file : %s, keyword : %s, data number setting error.",
			m_strFileName.c_str(),
            strFLAG.c_str());
        if (bMsgBox)
		    MessageBox(NULL, szErrorMsg, "ERROR", MB_OK);
		return NULL;
	}

	DWORD dwNUM = getflagnum ( strFLAG );
	if ( dwNUM<=dwITER )
	{
		char szErrorMsg[128] = "";
		StringCchPrintf(
            szErrorMsg,
            128,
            "file : %s, keyword : %s, data number setting error.",
			m_strFileName.c_str(),
            strFLAG.c_str());
        if (bMsgBox)
		    MessageBox(NULL, szErrorMsg, "ERROR", MB_OK);
		return NULL;
	}

	MAPFLAG_PAIR pair = m_mapFlags.equal_range ( strFLAG );
	
	MAPFLAG_ITER iter = pair.first;
	for ( DWORD i=0; i<dwITER; ++i )
        ++iter;

	m_pFOUND_VEC = (*iter).second;

	DWORD dwPARAMNUM = (DWORD) m_pFOUND_VEC->size();
	if ( dwPARAMNUM != (nSIZE+1) )
	{
		char szErrorMsg[128] = "";
		StringCchPrintf(
            szErrorMsg,
            128,
            "file : %s, keyword : %s, data number setting error.",
			m_strFileName.c_str(),
            strFLAG.c_str());
        if (bMsgBox)
		    MessageBox(NULL, szErrorMsg, "ERROR", MB_OK);
		return NULL;
	}

	return m_pFOUND_VEC;
}