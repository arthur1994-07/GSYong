#include "../pch.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/Math/Random.h"
#include "../../SigmaCore/String/StringFormat.h"

#include "../../EngineLib/G-Logic/GLogic.h"


#include "../../EngineLib/Common/gltexfile.h"
#include "../Item/GLItemMan.h"
#include "../GLogicData.h"
#include "./ItemGen.h"

#include "../GLogicFile.h"


#include <boost/tokenizer.hpp>
#include <boost/function.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CGENITEM::CGENITEM()
{
}

CGENITEM::~CGENITEM()
{
}

bool CGENITEM::LoadFile( const std::string& strFileList )
{
    if( strFileList.empty() )
    {
        return false;
    }
 
    typedef boost::char_separator<char> CHARSEP;
    boost::tokenizer<CHARSEP> token(strFileList, CHARSEP(",; "));

    // 파일이름을 토큰으로 분리합니다
    for (boost::tokenizer<CHARSEP>::iterator iter = token.begin(); iter != token.end(); ++iter)
    {
        const std::string& strToken = (*iter);
        if ( strToken.empty() )
			continue;

		SGENINFO sGenInfo;
		if ( GetGenInfo("", strToken, sGenInfo) == false )
			continue;

		m_vecGenInfo.push_back(sGenInfo);
    }
    return true;
}



bool CGENITEM::GetGenInfo(const std::string& ParentsFileName, const std::string& strFileName, SGENINFO& sGenInfo)
{
    if (strFileName.empty())
    {
        std::string ErrorMsg(
            sc::string::format(
                "CGENITEM::AddFile %1% gen item fileName is empty",
                ParentsFileName));
        sc::writeLogError(ErrorMsg);
        return false;
    }    

    std::string strPath;
//    strPath = GLOGIC::GetServerPath();
	strPath = GLOGIC::GetPath();
    strPath += strFileName;	

    gltexfile glTextFile;
    glTextFile.reg_sep("\t ,{}[]()|");

    if (GLOGIC::UseLogicZipFile())
        glTextFile.SetZipFile(GLogicData::GetInstance().GetGlogicZipFile()); // GLogicFile::GetInstance().GetGlogicZipFile() );//GLOGIC::strGLOGIC_SERVER_ZIPFILE);

    if (!glTextFile.open(strPath, true))
    {        
        std::string ErrorMsg(
            sc::string::format(
                "CGENITEM::AddFile %1% gen item file not exist %2%",
                ParentsFileName,
                strFileName));
        sc::writeLogError(ErrorMsg);
        return false;
    }

    float fRate          = 0;
    float fNumRateTotal  = 0;
    float fItemRateTotal = 0;

    // 버전
    //-----------------------------------------------------------------------------------------
    sGenInfo.dwVersion = 0;
    glTextFile.getflag ( "VERSION", 1, 1, sGenInfo.dwVersion, true );

    // 발생확률
    //-----------------------------------------------------------------------------------------
    sGenInfo.fProbability = 100.0f;
    glTextFile.getflag ( "GENRATE", 1, 1, sGenInfo.fProbability, true );

    // 발생갯수
    //-----------------------------------------------------------------------------------------
    DWORD dwNum = glTextFile.getflagnum ( "GENNUM" );

    if ( dwNum == 0 )
    {
        SGENNUM sGenNum;
        sGenNum.wNum  = 1;
        sGenNum.fRate = 100.0f;

        fNumRateTotal = 100.0f;
        sGenInfo.PushGenNum( sGenNum );
    }
    else
    {
        for ( DWORD i=0; i<dwNum; ++i )
        {
            SGENNUM sGenNum;

            glTextFile.getflag ( i, "GENNUM", 1, 2, sGenNum.wNum, true );
            glTextFile.getflag ( i, "GENNUM", 2, 2, sGenNum.fRate, true );

            fNumRateTotal += sGenNum.fRate;
            sGenInfo.PushGenNum( sGenNum );
        }
    }

    // 발생 아이템 군
    //-----------------------------------------------------------------------------------------
    DWORD dwSpecNum = glTextFile.getflagnum ( "ITEMSPEC" );
    for ( DWORD i=0; i<dwSpecNum; ++i )
    {
        std::tr1::shared_ptr<SGENITEMSPEC> spItemSpec( new SGENITEMSPEC );

        DWORD dwParam = glTextFile.getparamnum( i, "ITEMSPEC" );

        if ( dwParam == 3 )
        {
            glTextFile.getflag ( i, "ITEMSPEC", 1, 2, spItemSpec->dwSpecID, true );
            glTextFile.getflag ( i, "ITEMSPEC", 2, 2, spItemSpec->fRate, true );
        }
        else if ( dwParam == 4 )
        {
            glTextFile.getflag ( i, "ITEMSPEC", 1, 3, spItemSpec->dwSpecID, true );
            glTextFile.getflag ( i, "ITEMSPEC", 2, 3, spItemSpec->fRate, true );
            glTextFile.getflag ( i, "ITEMSPEC", 3, 3, spItemSpec->wGrade, true );
        }
        else
        {
            std::string strErr = sc::string::format( "ITEMSPEC : %s invalid param (num = %d)", 
                strFileName.c_str(), dwParam );
            MessageBox ( NULL, strErr.c_str(), "ERROR", MB_OK );

            continue;
        }

        if ( spItemSpec->dwSpecID >= GLItemMan::SPECID_NUM )
        {
            std::string strErr = sc::string::format( "ITEMSPEC : %s Overflow. %d >= %d", 
                strFileName.c_str(), spItemSpec->dwSpecID, GLItemMan::SPECID_NUM );
            MessageBox ( NULL, strErr.c_str(), "ERROR", MB_OK );

            continue;
        }

        fItemRateTotal += spItemSpec->fRate;
        sGenInfo.PushGenItem( std::tr1::static_pointer_cast<SGENITEMBASE>(spItemSpec) );
    }

    // 발생 아이템 ID
    //-----------------------------------------------------------------------------------------
    DWORD dwIDNum = glTextFile.getflagnum ( "ITEMID" );
    for ( DWORD i=0; i<dwIDNum; ++i )
    {
        std::tr1::shared_ptr<SGENITEMID> spItemID( new SGENITEMID );

        DWORD dwParam = glTextFile.getparamnum( i, "ITEMID" );

        if ( dwParam == 4 )
        {
            glTextFile.getflag ( i, "ITEMID", 1, 3, spItemID->sItemID.wMainID, true );
            glTextFile.getflag ( i, "ITEMID", 2, 3, spItemID->sItemID.wSubID, true );
            glTextFile.getflag ( i, "ITEMID", 3, 3, spItemID->fRate, true );
        }
        else if ( dwParam == 5 )
        {
            glTextFile.getflag ( i, "ITEMID", 1, 4, spItemID->sItemID.wMainID, true );
            glTextFile.getflag ( i, "ITEMID", 2, 4, spItemID->sItemID.wSubID, true );
            glTextFile.getflag ( i, "ITEMID", 3, 4, spItemID->fRate, true );
            glTextFile.getflag ( i, "ITEMID", 4, 4, spItemID->wGrade, true );
        }
        else
        {
            std::string strErr = sc::string::format( "ITEMID : %s invalid param (num = %d)", 
                strFileName.c_str(), dwParam );
            MessageBox ( NULL, strErr.c_str(), "ERROR", MB_OK );

            continue;
        }

        if ( spItemID->sItemID == SNATIVEID(false) )
        {
            std::string strErr = sc::string::format( "ITEMID : %s NativeID_NULL. %d", 
                strFileName.c_str(), dwIDNum );
            MessageBox ( NULL, strErr.c_str(), "ERROR", MB_OK );

            continue;
        }

        fItemRateTotal += spItemID->fRate;
        sGenInfo.PushGenItem( std::tr1::static_pointer_cast<SGENITEMBASE>(spItemID) );
    }

    // 발생 Q box 아이템 ID
    //-----------------------------------------------------------------------------------------
    DWORD dwQIDNum = glTextFile.getflagnum ( "QTITEM" );
    for ( DWORD i=0; i<dwQIDNum; ++i )
    {
        std::tr1::shared_ptr<SGENQITEMID> spQItemID( new SGENQITEMID );

        DWORD dwParam = glTextFile.getparamnum( i, "QTITEM" );

        if ( dwParam == 4 )
        {
            glTextFile.getflag ( i, "QTITEM", 1, 3, spQItemID->sQItemID.wMainID, true );
            glTextFile.getflag ( i, "QTITEM", 2, 3, spQItemID->sQItemID.wSubID, true );
            glTextFile.getflag ( i, "QTITEM", 3, 3, spQItemID->fRate, true );
        }
        else if ( dwParam == 5 )
        {
            glTextFile.getflag ( i, "QTITEM", 1, 4, spQItemID->sQItemID.wMainID, true );
            glTextFile.getflag ( i, "QTITEM", 2, 4, spQItemID->sQItemID.wSubID, true );
            glTextFile.getflag ( i, "QTITEM", 3, 4, spQItemID->fRate, true );
            glTextFile.getflag ( i, "QTITEM", 4, 4, spQItemID->wGrade, true );
        }
        else
        {
            std::string strErr = sc::string::format( "QTITEM : %s invalid param (num = %d)", 
                strFileName.c_str(), dwParam );
            MessageBox ( NULL, strErr.c_str(), "ERROR", MB_OK );

            continue;
        }

        if ( spQItemID->sQItemID == SNATIVEID(false) )
        {
            std::string strErr = sc::string::format( "QTITEM : %s NativeID_NULL. %d", 
                strFileName.c_str(), dwQIDNum );
            MessageBox ( NULL, strErr.c_str(), "ERROR", MB_OK );

            continue;
        }

        fItemRateTotal += spQItemID->fRate;
        sGenInfo.PushGenItem( std::tr1::static_pointer_cast<SGENITEMBASE>(spQItemID) );
    }


    // 발생 금액
    //-----------------------------------------------------------------------------------------
    DWORD dwMoneyNum = glTextFile.getflagnum ( "MONEY" );
    for ( DWORD i=0; i<dwMoneyNum; ++i )
    {
        std::tr1::shared_ptr<SGENMONEY> spItemMoney( new SGENMONEY );

        DWORD dwParam = glTextFile.getparamnum( i, "MONEY" );

        if ( dwParam == 3 )
        {
            glTextFile.getflag ( i, "MONEY", 1, 2, spItemMoney->dwMoney, true );
            glTextFile.getflag ( i, "MONEY", 2, 2, spItemMoney->fRate, true );
        }
        else if ( dwParam == 4 )
        {
            glTextFile.getflag ( i, "MONEY", 1, 3, spItemMoney->dwMoney, true );
            glTextFile.getflag ( i, "MONEY", 2, 3, spItemMoney->fRate, true );
            glTextFile.getflag ( i, "MONEY", 3, 3, spItemMoney->wGrade, true );
        }
        else
        {
            std::string strErr = sc::string::format( "MONEY : %s invalid param (num = %d)", 
                strFileName.c_str(), dwParam );
            MessageBox ( NULL, strErr.c_str(), "ERROR", MB_OK );

            continue;
        }

        if ( spItemMoney->dwMoney == 0 )
        {
            std::string strErr = sc::string::format( "MONEY : %s  invalid value (value = 0)", 
                strFileName.c_str() );
            MessageBox ( NULL, strErr.c_str(), "ERROR", MB_OK );

            continue;
        }

        fItemRateTotal += spItemMoney->fRate;
        sGenInfo.PushGenItem( std::tr1::static_pointer_cast<SGENITEMBASE>(spItemMoney) );
    }

    // 발생 아이템 군( 특정 버프가 걸렸을 경우만 드롭되는 아이템 )
    //-----------------------------------------------------------------------------------------
    DWORD dwSpecSkillfactNum = glTextFile.getflagnum ( "ITEMSPECSKILLFACT" );
    for ( DWORD i=0; i<dwSpecSkillfactNum; ++i )
    {
        std::tr1::shared_ptr<SGENITEMSPECSKILLFACT> spItemSpecSkillfact( new SGENITEMSPECSKILLFACT );

        DWORD dwParam = glTextFile.getparamnum( i, "ITEMSPECSKILLFACT" );

        if ( dwParam == 5 )
        {
            glTextFile.getflag ( i, "ITEMSPECSKILLFACT", 1, 4, spItemSpecSkillfact->sSkillID.wMainID, true );
            glTextFile.getflag ( i, "ITEMSPECSKILLFACT", 2, 4, spItemSpecSkillfact->sSkillID.wSubID, true );
            glTextFile.getflag ( i, "ITEMSPECSKILLFACT", 3, 4, spItemSpecSkillfact->dwSpecID, true );
            glTextFile.getflag ( i, "ITEMSPECSKILLFACT", 4, 4, spItemSpecSkillfact->fRate, true );
        }
        else if ( dwParam == 6 )
        {
            glTextFile.getflag ( i, "ITEMSPECSKILLFACT", 1, 5, spItemSpecSkillfact->sSkillID.wMainID, true );
            glTextFile.getflag ( i, "ITEMSPECSKILLFACT", 2, 5, spItemSpecSkillfact->sSkillID.wSubID, true );
            glTextFile.getflag ( i, "ITEMSPECSKILLFACT", 3, 5, spItemSpecSkillfact->dwSpecID, true );
            glTextFile.getflag ( i, "ITEMSPECSKILLFACT", 4, 5, spItemSpecSkillfact->fRate, true );
            glTextFile.getflag ( i, "ITEMSPECSKILLFACT", 5, 5, spItemSpecSkillfact->wGrade, true );
        }
        else
        {
            std::string strErr = sc::string::format( "ITEMSPECSKILLFACT : %s invalid param (num = %d)", 
                strFileName.c_str(), dwParam );
            MessageBox ( NULL, strErr.c_str(), "ERROR", MB_OK );

            continue;
        }

        if ( spItemSpecSkillfact->dwSpecID >= GLItemMan::SPECID_NUM )
        {
            std::string strErr = sc::string::format( "ITEMSPECSKILLFACT : %s Overflow. %d >= %d", 
                strFileName.c_str(), spItemSpecSkillfact->dwSpecID, GLItemMan::SPECID_NUM );
            MessageBox ( NULL, strErr.c_str(), "ERROR", MB_OK );
            continue;
        }

        fItemRateTotal += spItemSpecSkillfact->fRate;
        sGenInfo.PushGenItem( std::tr1::static_pointer_cast<SGENITEMBASE>(spItemSpecSkillfact) );
    }

    // GENINFO의 타입을 결정한다
    //-----------------------------------------------------------------------------------------

    // QITEM 전용
    if ( dwQIDNum )
    {
        sGenInfo.emType = ( dwSpecNum || dwIDNum || dwMoneyNum || dwSpecSkillfactNum ) ? EMGENINFOTYPE_ONLY_QITEM : EMGENINFOTYPE_NONE;
    }
    // 금액 전용
    else if ( dwMoneyNum )
    {
        sGenInfo.emType = ( dwSpecNum || dwIDNum || dwQIDNum || dwSpecSkillfactNum ) ? EMGENINFOTYPE_ONLY_MONEY : EMGENINFOTYPE_NONE;
    }
    // 버프전용
    else if ( dwSpecSkillfactNum )
    {
        sGenInfo.emType = ( dwSpecNum || dwIDNum || dwMoneyNum || dwQIDNum ) ? EMGENINFOTYPE_ONLY_ITEMSKILLFACT : EMGENINFOTYPE_NONE;
    }
    // 일반(복합)
    else 
    {
        sGenInfo.emType = EMGENINFOTYPE_NONE;
    }

    // 발생 갯수 확률 총합이 100보다 작을수는 있지만 크다면 강제한다
    //-----------------------------------------------------------------------------------------
    if ( 100.0f < fNumRateTotal )
    {
        float fForceRate = 100.0f/fNumRateTotal;

        using namespace boost::lambda;
        std::for_each(sGenInfo.vecGenNum.begin(), sGenInfo.vecGenNum.end(),
            (&boost::lambda::_1)->*( &SGENNUM::fRate ) *= fForceRate );
    }

    // 아이템 확률 총합이 100보다 작을수는 있지만 크다면 강제한다
    //-----------------------------------------------------------------------------------------
    if ( 100.0f < fItemRateTotal )
    {
        float fForceRate = 100.0f/fItemRateTotal;

        using namespace boost::lambda;
        std::for_each(sGenInfo.vecGenItem.begin(), sGenInfo.vecGenItem.end(),
            (&(*boost::lambda::_1))->*( &SGENITEMBASE::fRate ) *= fForceRate );
    }

    // 확률이 낮은 순으로 정렬
    //-----------------------------------------------------------------------------------------
    {
        using namespace boost::lambda;
        std::sort(sGenInfo.vecGenItem.begin(), sGenInfo.vecGenItem.end(), boost::lambda::bind( SGENITEMBASE::LESS(), 
            &(*boost::lambda::_1),
            &(*boost::lambda::_2) ));
    }    

    return true;
}
void CGENITEM::AddGenInfo(const SGENINFO& sGenInfo)
{
	m_vecGenInfo.push_back(sGenInfo);
}

bool CGENITEM::AddInfoGenItem ( const float fProb, const SNATIVEID& sItemID )
{
    SGENINFO sGenInfo;

    //! INFO
    sGenInfo.emType       = EMGENINFOTYPE_NONE;
    sGenInfo.dwVersion    = 0;
    sGenInfo.fProbability = fProb > 100.0f ? 100.0f : fProb;

    //! GENNUM
    SGENNUM sGenNum;
    sGenNum.fRate = 100.0f;
    sGenNum.wNum  = 1;
    sGenInfo.PushGenNum( sGenNum );

    //! GENITEM
    std::tr1::shared_ptr<SGENITEMID> spItemID( new SGENITEMID );
    spItemID->sItemID = sItemID;
    spItemID->fRate   = 100.0f;
    sGenInfo.PushGenItem ( std::tr1::static_pointer_cast<SGENITEMBASE>(spItemID) );
   
    m_vecGenInfo.push_back(sGenInfo);
    
    return true;
}

bool CGENITEM::AddInfoGenMoney ( const float fProb, const DWORD dwMoney )
{
    if (dwMoney <= 0)
        return false;

    SGENINFO sGenInfo;

    //! INFO
    sGenInfo.emType       = EMGENINFOTYPE_ONLY_MONEY;
    sGenInfo.dwVersion    = 0;
    sGenInfo.fProbability = fProb > 100.0f ? 100.0f : fProb;

    //! GENNUM
    SGENNUM sGenNum;
    sGenNum.fRate = 100.0f;
    sGenNum.wNum  = 1;
    sGenInfo.PushGenNum( sGenNum );

    //! GENITEM
    std::tr1::shared_ptr<SGENMONEY> spItemMoney( new SGENMONEY );
    spItemMoney->dwMoney = dwMoney;
    spItemMoney->fRate   = 100.0f;
    sGenInfo.PushGenItem ( std::tr1::static_pointer_cast<SGENITEMBASE>(spItemMoney) );
   
    m_vecGenInfo.push_back(sGenInfo);

    return true;
}

const WORD CGENITEM::GenerateNum(const SGENINFO::VEC_SGENNUM& vecGenNum)
{
	// 0.01~99.99%
    float fNowRate = 0.0f;
    WORD  wNum     = 0;

    SGENINFO::VEC_SGENNUM_CITER iter = vecGenNum.begin();
    for ( ; iter!=vecGenNum.end(); ++iter )
    {
        const SGENNUM& sGenNum = (*iter);

		fNowRate += sGenNum.fRate;

		if ( sc::Random::getInstance().IsSucceed(fNowRate) )
		{
			wNum = sGenNum.wNum;
			break;
		}
    }

    return wNum;
}

const WORD CGENITEM::GenerateNum_RandomOnce(const SGENINFO::VEC_SGENNUM& vecGenNum)
{
	float fNowRate = 0.0f, fRate = sc::Random::getInstance().GetFloatPercent();
	WORD  wNum     = 0;

	SGENINFO::VEC_SGENNUM_CITER iter = vecGenNum.begin();
	for ( ; iter!=vecGenNum.end(); ++iter )
	{
		const SGENNUM& sGenNum = (*iter);

		fNowRate += sGenNum.fRate;
		if ( fRate < fNowRate )
		{
			wNum = sGenNum.wNum;
			break;
		}
	}
	return wNum;
}

bool CGENITEM::SaveFile(const SGENINFO& sGenInfo, std::string &_FileName, const std::vector<std::string> &_Annotation)
{

	CString szFilter = "genitem file (*.genitem)|*.genitem|";
	CFileDialog dlg( FALSE, ".genitem", _FileName.c_str(), OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter);

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
	if ( dlg.DoModal() != IDOK )
		 return false;

	CString strTemp;
	GetGenInfoStr(strTemp, sGenInfo, _Annotation);

	sc::CRijndael oRijndael;
	oRijndael.Initialize( sc::CRijndael::sm_Version[sc::CRijndael::VERSION-1].c_str(), sc::CRijndael::sm_KeyLength[sc::CRijndael::VERSION-1] );

	int nLen = oRijndael.GetEncryptStringLength( strTemp );
	char * szBuffer = new char[nLen];

	oRijndael.EncryptEx( strTemp.GetString(), szBuffer, strTemp.GetLength() );
	int nVersion = sc::CRijndael::VERSION;

	CFile file;
	file.Open( dlg.GetPathName().GetString(), CFile::modeCreate|CFile::modeWrite );	// 파일 열기
	file.Write( &nVersion, sizeof(int) );							// 버젼 쓰기
	file.Write( szBuffer, nLen );		// 파일 쓰기
	file.Close();

	delete [] szBuffer;
	return true;	
}

void CGENITEM::GetGenInfoStr(CString &_Str, const SGENINFO& _GenInfo, const std::vector<std::string> &_Annotation)
{
	CString _strComplete("");
	CString _strtmp("");
	TCHAR sztchtemp[MAX_PATH];
	memset(sztchtemp, 0x00, sizeof(TCHAR)*MAX_PATH);

	_strComplete.Append(_T("//Genrate File %\n"));	
	sprintf(sztchtemp,"GENRATE         %g\n", _GenInfo.fProbability);
	_strComplete.Append(sztchtemp);
	_strComplete.Append(_T("//Num of Gen [max : 5] %sum = 100% \n"));
	
	unsigned int _Size = _GenInfo.vecGenNum.size();
	for(unsigned int _index(0); _index < _Size; ++_index)
	{
		_strtmp.Format(_T("GENNUM       %d        %g\n"), _GenInfo.vecGenNum[_index].wNum, _GenInfo.vecGenNum[_index].fRate);
		_strComplete.Append(_strtmp);
	}

	_strComplete.Append(_T("//kind of SpecID [max : 30] %sum = 100%\n"));

	_Size = _GenInfo.vecGenItem.size();
	for(unsigned int _index(0); _index < _Size; ++_index)
	{
		switch(_GenInfo.vecGenItem[_index]->emType)
		{
			case EMGENITEMTYPE_ITEM:
				{
					std::tr1::shared_ptr<SGENITEMSPEC> pTempGen = 
						std::tr1::static_pointer_cast<SGENITEMSPEC>(_GenInfo.vecGenItem[_index]);
					_strtmp.Format(_T("ITEMSPEC       	           %d      %g"), pTempGen->dwSpecID, pTempGen->fRate);
					_strComplete.Append(_strtmp);	

					if(_Annotation.size() != 0)
					{
						if(_Annotation[_index] != "")
						{	
							_strtmp.Format("   //");
							_strtmp.Append(_Annotation[_index].c_str());							
							_strComplete.Append(_strtmp);
						}
					}
					_strtmp.Format("\n");
					_strComplete.Append(_strtmp);
					break;
				}

			case EMGENITEMTYPE_ITEMSKILLFACT:
				{
					std::tr1::shared_ptr<SGENITEMSPECSKILLFACT> pTempGen = 
						std::tr1::static_pointer_cast<SGENITEMSPECSKILLFACT>(_GenInfo.vecGenItem[_index]);
					_strtmp.Format(_T("ITEMSPECSKILLFACT	%d %d     %d      %g"), 
						(int)pTempGen->sSkillID.wMainID, (int)pTempGen->sSkillID.wSubID, 
						pTempGen->dwSpecID, pTempGen->fRate);
					_strComplete.Append(_strtmp);

					if(_Annotation.size() != 0)
					{
						if(_Annotation[_index] != "")
						{	
							_strtmp.Format("   //");
							_strtmp.Append(_Annotation[_index].c_str());							
							_strComplete.Append(_strtmp);
						}
					}
					_strtmp.Format("\n");
					_strComplete.Append(_strtmp);
					break;
				}
		}
		
	}

	_Str = _strComplete;
}