#include "pch.h"

#include "GLItem.h"
#include "GLItemDef.h"

#include "../../SigmaCore/Lua/MinLua.h"

#include "../../EngineLib/G-Logic/GLogic.h"
#include "../GLogicDataMan.h"

#include "GLItemCostumeAttach.h"

namespace COSTUMESTATSDATA
{

CBasePartsDt::CBasePartsDt( EMSUIT eParts ) : m_ePart( eParts )
{

}

/* ====================================================================================================== */

CPartsDt::CPartsDt( EMSUIT eParts ) : CBasePartsDt( eParts )
{
	m_vLvPt.clear();	
	m_mData.clear();
}

const sPsData* CPartsDt::GetPsData( COSTUME_STATS eState )
{
	mPsData::const_iterator _iter = m_mData.find( eState );

	if( _iter != m_mData.end() )
	{
		return &_iter->second;
	}

	return NULL;
}

const CPartsDt::mPsData* const CPartsDt::GetPsData()
{
	return &m_mData;
}


WORD CPartsDt::GetLvValue( WORD wLv )
{
	int isize = m_vLvPt.size();
	int iIdx  = (int)(wLv - 1);

	if( isize == 0 || isize <= iIdx || iIdx < 0 ) return 0;

	return m_vLvPt[iIdx];
}

void CPartsDt::SetPartstat( int iStat, const sPsData& sData )
{
	if( iStat == 0 || iStat > (int)E_R_EMPTY ) return;

	COSTUME_STATS eStatNum = static_cast<COSTUME_STATS>(iStat);

	//m_mData[eStatNum].push_back(sData);
	m_mData[eStatNum] = sData;
}

void CPartsDt::InsertLvPoint( WORD wPt )
{
	m_vLvPt.push_back(wPt);
}

bool CPartsDt::ParseData( int iPart, std::string strName )
{
	if( strName.empty() ) return false;

	try
	{
		LuaPlus::LuaObject ob = sc::lua_getGlobalFromName("Use_Point");

		int iIndex = 0;

		for( LuaPlus::LuaTableIterator it(ob); it.IsValid(); it.Next() ) 
		{
			LuaPlus::LuaTableIterator it_node(it.GetValue());

			if( !it_node.IsValid() ) continue;

			int iKey = it_node.GetValue().GetInteger();

			if( iKey != iPart ) continue;

			it_node.Next();

			for( ; it_node.IsValid() ; it_node.Next() )
			{
				InsertLvPoint( it_node.GetValue().GetInteger() );
			}			
		}

		try
		{
			m_ePart = static_cast<EMSUIT>(iPart);

			LuaPlus::LuaObject obParts = sc::lua_getGlobalFromName( strName.c_str() );
			
			for( LuaPlus::LuaTableIterator it(obParts); it.IsValid(); it.Next() ) 
			{
				LuaPlus::LuaTableIterator it_node(it.GetValue());

				if( !it_node.IsValid() ) continue;
				
				int  iStatNum = it_node.GetValue().GetInteger();					it_node.Next();
				WORD wMaxPt   = static_cast<WORD>(it_node.GetValue().GetInteger()); it_node.Next();
				double fInc    = it_node.GetValue().GetDouble();					it_node.Next();
				
				SetPartstat(iStatNum, sPsData(wMaxPt, fInc) );

			}
		}
		catch( const LuaPlus::LuaException& e )
		{
			//MessageBox(NULL, e.GetErrorMessage(), strName.c_str(), MB_OK);
		}
	}
	catch( const LuaPlus::LuaException& e)
	{
		MessageBox(NULL, e.GetErrorMessage(), "Use_Point", MB_OK);

		return false;
	}
	
	return true;
}
/* ====================================================================================================== */

CNDuplication::CNDuplication()
{
	m_List.clear();
}

CNDuplication::~CNDuplication()
{

}

bool CNDuplication::ParseData(std::string strName)
{
	if( strName.empty() ) return false;

	try
	{
		LuaPlus::LuaObject ob = sc::lua_getGlobalFromName(strName.c_str());

		int iIndex = 0;

		for( LuaPlus::LuaTableIterator it(ob); it.IsValid(); it.Next(), ++iIndex ) 
		{
			for( LuaPlus::LuaTableIterator it_node(it.GetValue()); it_node.IsValid(); it_node.Next())
			{
				m_List[iIndex].push_back( it_node.GetValue().GetInteger() );
			}			
		}
	}
	catch( const LuaPlus::LuaException& e)
	{
		MessageBox(NULL, e.GetErrorMessage(), strName.c_str(), MB_OK);

		return false;
	}

	return true;
}

void CNDuplication::GetNDupleList(COSTUME_STATS eSt, mList& mList)
{
	WORD wStat = static_cast<WORD>(eSt);

	mIter _iter = m_List.begin();

	while( _iter != m_List.end())
	{
		vIter _viter = _iter->second.begin();

		while( _viter != _iter->second.end())
		{
			if( wStat == (*_viter) ) {
				mList[_iter->first] = _iter->second;
			}

			++_viter;
		}

		++_iter;
	}
}

bool CNDuplication::IsNDupleExist(COSTUME_STATS eSt, vData& vList)
{
	WORD wStat = static_cast<WORD>(eSt);

	mIter _iter = m_List.begin();

	int iMapKey = -1;

	while( _iter != m_List.end())
	{
		vIter _viter = _iter->second.begin();

		while( _viter != _iter->second.end())
		{
			if( wStat == (*_viter) ) {
				iMapKey = _iter->first;
				break;
			}

			++_viter;
		}

		++_iter;
	}

	if( iMapKey >= 0 ) {
		vList = m_List[iMapKey]; 
		return true;
	}

	return false;
}

/* ====================================================================================================== */

CCostumeStatData& CCostumeStatData::GetInstance()
{
	static CCostumeStatData Inst;

	return Inst;
}

CCostumeStatData::CCostumeStatData()
{
	m_Parts.clear();
	m_ShareMax.clear();
}

CCostumeStatData::~CCostumeStatData()
{

}

bool CCostumeStatData::LoadData( std::string strFileName )
{
	std::string strPath(GLOGIC::GetPath());

	std::string strCostumeFile = strFileName;

	if(strPath.empty()) return false;

	strPath.append(strCostumeFile);

	char szName[_MAX_FNAME] = _T("");
	char szExt[_MAX_EXT] = _T("");
	_splitpath(strPath.c_str(), NULL, NULL, szName, szExt);//경로를 통해서 이름과 확장자를 알아냄
	StringCchCat(szName, _MAX_FNAME, szExt); //이름뒤에 확장자 붙여줌

	std::auto_ptr< sc::BaseStream > pStream(
		GLOGIC::openfile_basestream(
		GLOGIC::UseLogicZipFile(),
		GLogicData::GetInstance().GetGlogicZipFile(),
		strPath.c_str(), 
		szName, 
		false,
		GLOGIC::UseLogicPackFile() ) );

	//파일 경로를 따라가서 파일을 열음(zip파일인지 아닌지로 구분해서, 초기화)
	if(!pStream.get())
	{
		sc::writeLogError(sc::string::format("CCostumeStatData::LoadData, File Open %1%", strPath));
		return false;
	}

	size_t nSize = static_cast<size_t> (pStream->GetFileSize());
	void* pBuffer = malloc(nSize);
	pStream->ReadBuffer(pBuffer, 0);	
	CTypeSmartPtr cPtrData(pBuffer); //smartptr로 데이터 담음

	try
	{
		sc::lua_init();
		if(!sc::lua_doBuffer(static_cast<char*>(pBuffer), nSize, NULL))
		{
			MessageBox(NULL, sc::lua_getError().c_str(), strPath.c_str(), MB_OK);
			return false;
		}		
		return ParseData();
	}
	catch(const LuaPlus::LuaException& e)
	{
		MessageBox(NULL, e.GetErrorMessage(), strPath.c_str(), MB_OK);
		return false;
	}
	return false;
}

bool CCostumeStatData::ParseData()
{
	bool bLoadSuccess = false;

	m_NDuplication.ParseData("Duplicate_type_Check");

	try
	{
		LuaPlus::LuaObject ob = sc::lua_getGlobalFromName("Parts_Naming");

		int iIndex = 0;

		std::wstring ws;

		for( LuaPlus::LuaTableIterator it(ob); it.IsValid(); it.Next(), ++iIndex ) 
		{
			for( LuaPlus::LuaTableIterator it_node(it.GetValue()); it_node.IsValid(); )
			{												
				int iParts = it_node.GetValue().GetInteger();							it_node.Next();

				ws = reinterpret_cast<const wchar_t*>(it_node.GetValue().GetWString()); it_node.Next();

				std::string strTemp = sc::string::unicodeToAnsi(ws);

				LuaPlus::LuaObject objSerch = sc::lua_getGlobalFromName(strTemp.c_str());

				if( !objSerch.IsTable() ) 
				{

					m_Parts[static_cast<EMSUIT>(iParts)] = std::tr1::shared_ptr<CBasePartsDt> ((CBasePartsDt*)0);

					continue;				
				}

				std::tr1::shared_ptr<CBasePartsDt> pTemp( new CPartsDt() );

				pTemp->ParseData( iParts,  strTemp);

				m_Parts[static_cast<EMSUIT>(iParts)] = pTemp;
			}			
		}

		bLoadSuccess = true;
	}
	catch( const LuaPlus::LuaException& e)
	{
		MessageBox(NULL, e.GetErrorMessage(), "Parts_Naming", MB_OK);
	}

	int iMax = 0;
	
	try
	{
		std::wstring ws;

		LuaPlus::LuaObject ob = sc::lua_getGlobalFromName("Share_Max");
		
		for( LuaPlus::LuaTableIterator it(ob); it.IsValid(); it.Next() ) 
		{
			for( LuaPlus::LuaTableIterator it_node(it.GetValue()); it_node.IsValid(); )
			{
				ws = reinterpret_cast<const wchar_t*>(it_node.GetValue().GetWString()); it_node.Next();

				std::string strTemp = sc::string::unicodeToAnsi(ws);

				int iMaxCnt = it_node.GetValue().GetInteger();	it_node.Next();

				if( strTemp.compare("GN_MAX") == 0 )
				{
					m_ShareMax[E_SHARE_GN_TYPE] = iMaxCnt;					
				}
				else
				if( strTemp.compare("VAR_MAX") == 0 )
				{
					m_ShareMax[E_SHARE_VAR_TYPE] = iMaxCnt;
				}
				else
				if( strTemp.compare("ADDED_MAX") == 0 )
				{
					m_ShareMax[E_SHARE_ADDED_TYPE] = iMaxCnt;
				}

				iMax += iMaxCnt;

			}
		}
	}
	catch( const LuaPlus::LuaException& e)
	{
		MessageBox(NULL, e.GetErrorMessage(), "Share_Max", MB_OK);
	}

	if( iMax != COSTUMEUSERVALUE::sCostumeUser::MAX_COS_SIZE )
	{
		sc::writeLogError(std::string("Share Total != Script Cnt"));
	}
	
	return bLoadSuccess;
}

const CPartsDt::mPsData* const CCostumeStatData::GetStatInfo( EMSUIT eSuit )
{
	mPIter _iter = m_Parts.find( eSuit );

	if( _iter != m_Parts.end() && _iter->second != NULL )
	{
		std::tr1::shared_ptr<CPartsDt> pPartsData = std::tr1::dynamic_pointer_cast<CPartsDt>(_iter->second);

		return pPartsData->GetPsData();
	}	

	return NULL;
}

const sPsData* CCostumeStatData::GetStatInfo( EMSUIT eSuit, COSTUME_STATS eStat )
{
	mPIter _iter = m_Parts.find( eSuit );

	if( _iter != m_Parts.end() && _iter->second != NULL )
	{
		std::tr1::shared_ptr<CPartsDt> pPartsData = std::tr1::dynamic_pointer_cast<CPartsDt>(_iter->second);

		return pPartsData->GetPsData(eStat);
	}	

	return NULL;
}

double	CCostumeStatData::GetStatInc( EMSUIT eSuit, COSTUME_STATS eStat )
{
	mPIter _iter = m_Parts.find( eSuit );

	if( _iter != m_Parts.end() && _iter->second != NULL )
	{
		std::tr1::shared_ptr<CPartsDt> pPartsData = std::tr1::dynamic_pointer_cast<CPartsDt>(_iter->second);

		const sPsData* pTemp = pPartsData->GetPsData(eStat);

		if( pTemp ) return pTemp->fInc;
	}	

	return 0;
}

WORD CCostumeStatData::GetStatMaxPt( EMSUIT eSuit, COSTUME_STATS eStat )
{
	mPIter _iter = m_Parts.find( eSuit );

	if( _iter != m_Parts.end() && _iter->second != NULL )
	{
		std::tr1::shared_ptr<CPartsDt> pPartsData = std::tr1::dynamic_pointer_cast<CPartsDt>(_iter->second);

		const sPsData* pTemp = pPartsData->GetPsData(eStat);

		if( pTemp ) return pTemp->wMaxPt;
	}	

	return 0;
}

WORD CCostumeStatData::GetPartsEnableMaxPt( EMSUIT eSuit, WORD wLv )
{
	mPIter _iter = m_Parts.find( eSuit );

	if( _iter != m_Parts.end() && _iter->second != NULL )
	{
		std::tr1::shared_ptr<CPartsDt> pPartsData = std::tr1::dynamic_pointer_cast<CPartsDt>(_iter->second);

		return pPartsData->GetLvValue( wLv );
	}

	return 0;
}

WORD CCostumeStatData::GetShareMax( SHARE_TYPE eType )
{
	mSharecIter _iter = m_ShareMax.find( eType );

	if( _iter != m_ShareMax.end() )
	{
		return _iter->second;
	}

	return 0;
}

void CCostumeStatData::GetNDupleList(COSTUME_STATS eStat, CNDuplication::mList& mList)
{	
	m_NDuplication.GetNDupleList(eStat, mList);
}

bool CCostumeStatData::IsNDupleExist(COSTUME_STATS eStat, CNDuplication::vData& vList)
{	
	return m_NDuplication.IsNDupleExist(eStat, vList);
}

bool CCostumeStatData::IsPartsData(EMSUIT eSuit)
{
	mPIter _iter = m_Parts.find( eSuit );

	if( _iter != m_Parts.end() )
	{
		return true;
	}

	return false;
}


};

namespace COSTUMEUSERVALUE
{
	const bool sCostumeUser::sInvest::operator ==(const sCostumeUser::sInvest& _rhs) const
	{
		if ( cStatType == _rhs.cStatType && wInvestPt == _rhs.wInvestPt)
			return true;

		return false;
	}

	const bool sCostumeUser::sInvest::operator !=(const sCostumeUser::sInvest& _rhs) const
	{
		if ( cStatType != _rhs.cStatType )
			return true;

		return false;
	}
	const sCostumeUser::sInvest& sCostumeUser::sInvest::operator =(const sCostumeUser::sInvest& rhs)
	{
		cStatType  = rhs.cStatType;
		//nStatValue = rhs.nStatValue;
		wInvestPt  = rhs.wInvestPt;

		return *this;
	}
	sCostumeUser::sInvest::sInvest(const BYTE _cStatType, const WORD _wInvestPt)
		: cStatType(_cStatType)
		, wInvestPt(_wInvestPt)
	{
	}
	sCostumeUser::sInvest::sInvest(void)
		: cStatType(0)
		, wInvestPt(0)
	{
	}
	void sCostumeUser::sInvest::reset(void)
	{
		cStatType = 0;
		wInvestPt = 0;
	}

	const sCostumeUser& sCostumeUser::operator = (const sCostumeUser& rhs)
	{
		for ( DWORD _i(sCostumeUser::MAX_COS_SIZE); _i--; )
			Invest[_i] = rhs.Invest[_i];

		tEndTime = rhs.tEndTime;

		return *this;
	}

	const bool sCostumeUser::operator ==(const sCostumeUser& rhs)
	{
		// 개조 횟수에 대한 비교는 하지 않는다;
		for ( DWORD _i(sCostumeUser::MAX_COS_SIZE); _i--; )
		{
			if ( Invest[_i] != rhs.Invest[_i] || tEndTime != rhs.tEndTime )
				return false;
		}

		return true;
	}

	sCostumeUser::sCostumeUser(void) : tEndTime(0)
	{
		
	}
	sCostumeUser::sCostumeUser(const sCostumeUser& rhs)
	{
		for ( DWORD _i(sCostumeUser::MAX_COS_SIZE); _i--; )
			Invest[_i] = rhs.Invest[_i];

		tEndTime = rhs.tEndTime;
	}

	void sCostumeUser::remove(const DWORD nIndex)
	{
		if ( nIndex >= sCostumeUser::MAX_COS_SIZE )
			return;	

		for ( DWORD _i(nIndex); _i < sCostumeUser::MAX_COS_SIZE - 1; ++_i )
			Invest[_i] = Invest[_i + 1];

		Invest[sCostumeUser::MAX_COS_SIZE - 1].reset();
	}

	const bool sCostumeUser::setInvest(const BYTE nType, const WORD wPt)
	{
		const DWORD nIndex(sCostumeUser::_findEmptySlot());

		if ( nIndex == sCostumeUser::COSTUME_TYPE_NULL )
			return false;

		return sCostumeUser::setInvest(nIndex, nType, wPt );
	}

	// 지정된 곳에 값을 설정 함;
	const bool sCostumeUser::setInvest(const DWORD nIndex, const BYTE nType, const WORD wPt)
	{
		/*
		short nVALUE(0);
		switch ( nType )
		{
		default:
		case sCostumeUser::COSTUME_TYPE_NULL:
			return false;

		case E_EXPMULTIPLE:
		case E_HITRATE:
		case E_AVOIDRATE:
			nVALUE = short(fValue*100.0f);
			break;

		case E_DAMAGE:
		case E_DEFENCE:
		case E_RESIST_FIRE:
		case E_RESIST_ICE:
		case E_RESIST_ELECTRIC:
		case E_RESIST_POISON:
		case E_RESIST_SPIRIT:
			nVALUE = short(fValue);
			break;

		case E_R_HP_INC:
		case E_R_MP_INC:
		case E_R_SP_INC:
		case E_R_HMSP_INC:
			nVALUE = short(fValue * 10000);
			break;

		case E_R_MOVESPEED:
		case E_R_ATTACKSPEED:
		case E_R_CRITICALHIT:
		case E_R_STRIKEHIT:
			nVALUE = short(fValue * 100);
			break;

		case E_A_HP_INC:
		case E_A_MP_INC:
		case E_A_SP_INC:
		case E_A_HMSP_INC:
		case E_A_MOVESPEED:
		case E_A_ATTACKSPEED:
		case E_A_CRITICALHIT:
			nVALUE = short(fValue);
			break;		

		case E_ADD_HITRATE:
		case E_ADD_AVOIDRATE:
			nVALUE = short(fValue * 100);
			break;

		case E_ADD_DAMAGE:
		case E_ADD_DEFENSE:
		case E_ADD_HP:
		case E_ADD_MANA:
		case E_ADD_STEMINA:
		case E_ADD_STATS_POW:
		case E_ADD_STATS_STA:
		case E_ADD_STATS_SPI:
		case E_ADD_STATS_DEX:
		case E_ADD_STATS_INT:
		case E_ADD_STATS_STR:		
		case E_ADD_ENERGY:
		case E_ADD_SHOOTING:
		case E_ADD_MELEE:
			nVALUE = short(fValue);
			break;

		};*/

		//		return sCostumeUser::assign(nIndex, nType, nVALUE, wPt);
		return false;
	}

	const BYTE sCostumeUser::getType(const DWORD nIndex) const
	{
		return nIndex < sCostumeUser::MAX_COS_SIZE ? Invest[nIndex].cStatType : E_NONE;
	}
	const __time64_t sCostumeUser::getEndTime() const
	{
		return tEndTime;
	}
	const WORD sCostumeUser::getInvestPt(const DWORD nIndex) const
	{
		return nIndex < sCostumeUser::MAX_COS_SIZE ? Invest[nIndex].wInvestPt : E_NONE;
		
	}

	bool  sCostumeUser::getTypeInfo( BYTE  nType, sInvest& sinfo ) const
	{
		for ( DWORD _i(0); _i < sCostumeUser::MAX_COS_SIZE; ++_i )
		{
			if ( Invest[_i].cStatType == nType )
			{
				sinfo = Invest[_i];

				return true;
			}
		}

		return false;
	}
	
	const DWORD sCostumeUser::_findEmptySlot(void) const
	{
		for ( DWORD _i(0); _i < sCostumeUser::MAX_COS_SIZE; ++_i )
		{
			if ( Invest[_i].cStatType == E_NONE )
				return _i;
		}

		return sCostumeUser::COSTUME_TYPE_NULL;
	}

	const DWORD sCostumeUser::_size(void) const
	{
		DWORD nSize(0);
		for ( DWORD _i(sCostumeUser::MAX_COS_SIZE); _i--; )
			nSize += (Invest[_i].cStatType == E_NONE ? 1 : 0);

		return nSize;
	}

	bool sCostumeUser::IsCSApplied(void) const
	{
		return (_size() == sCostumeUser::MAX_COS_SIZE ) ? false : true;
	}

	void sCostumeUser::ResetInvest()
	{
		for ( int i = 0; i < sCostumeUser::MAX_COS_SIZE; ++i )
			Invest[i].reset();

		tEndTime = 0;
	}

	float sCostumeUser::getValue( int iNum, int iPart )
	{
		if( iNum < 0 || iNum >= MAX_COS_SIZE ) return 0.0;

		COSTUME_STATS sType = (COSTUME_STATS)Invest[iNum].cStatType;

		double dValue = COSSDATA.GetStatInc((EMSUIT)iPart, sType );
				
		float fValue = 0.f;

		switch( sType )
		{
		case E_EXPMULTIPLE   :
		case E_R_HP_INC	     :
		case E_R_MP_INC	     :
		case E_R_SP_INC	     :
		case E_R_HMSP_INC    :
		case E_R_MOVESPEED   :
		case E_R_ATTACKSPEED :
		case E_R_CRITICALHIT :
		case E_R_STRIKEHIT   :
		case E_R_EMPTY       :
			fValue = static_cast<float>(dValue*Invest[iNum].wInvestPt);
			fValue = fValue*100;
			break;
		default :
			{
				int iValue = static_cast<int>(dValue*Invest[iNum].wInvestPt);
				fValue = static_cast<float>(iValue);
			}			
			break;
		}

		return fValue;
	}

};
