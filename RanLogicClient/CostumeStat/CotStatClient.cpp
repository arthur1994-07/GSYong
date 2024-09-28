#include "pch.h"

#include "../RanLogic/GLogicDataMan.h"
#include "../RanLogic/Item/GLItem.h"
#include "../RanLogic/Item/GLItemDef.h"
#include "../RanLogic/Item/GLItemCostumeAttach.h"
#include "../RanLogic/Msg/GLContrlInvenMsg.h"

#include "../GLGaeaClient.h"
#include "../Char/GLCharacter.h"

#include "./Widget/GLWidgetScript.h"

#include "CotStatClient.h"

CCostumeStatClient::CCostumeStatClient(GLGaeaClient* pGaeaClient) 
: m_pGaeaClient(pGaeaClient)
, m_iHavePoint(0), m_iSubPoint(0), m_pSrcItem(NULL), m_pDstItem(NULL), m_iCurSuit(-1)
{
	m_InvestPoint.clear();
}

CCostumeStatClient::~CCostumeStatClient()
{

}

void CCostumeStatClient::SetHavePoint( __int64 iPt )
{
	m_iHavePoint = iPt;

	m_iSubPoint = 0;
}
void CCostumeStatClient::SetSubPoint( __int64 iPt )
{
	m_iSubPoint = iPt;
}
void CCostumeStatClient::SetSuit(EMSUIT eSuit )
{
	m_iCurSuit = static_cast<int>(eSuit);
}
void CCostumeStatClient::SetItem(const SINVENITEM* pSrc, const SINVENITEM* pDst)
{
	m_pSrcItem = pSrc;
	m_pDstItem = pDst;
}

__int64 CCostumeStatClient::GetRemainPoint()
{
	return m_iHavePoint - m_iSubPoint;
}

void CCostumeStatClient::RestMember()
{
	m_iCurSuit   = -1;
	m_iHavePoint = 0;
	m_iSubPoint  = 0;
	m_pSrcItem   = NULL;
	m_pDstItem   = NULL;
	
	m_InvestPoint.clear();

	m_pGaeaClient->GetCharacter()->InitCostumStatData();
}

void CCostumeStatClient::SetNumeric( COSTUME_STATS eSt, WORD wValue )
{
	m_InvestPoint[eSt] = wValue;
}

bool CCostumeStatClient::IsNumeric(COSTUME_STATS eSt)
{
	MIter _mIter = m_InvestPoint.find( eSt );

	bool bResult = false;

	if( _mIter != m_InvestPoint.end() ) 
	{
		bResult =  ((_mIter->second) > 0 ) ? true : false;
	}

	return bResult;
}

WORD CCostumeStatClient::GetNumeric(COSTUME_STATS eSt)
{
	MIter _mIter = m_InvestPoint.find( eSt );

	return ( _mIter != m_InvestPoint.end() ) ? _mIter->second : 0;
}

void CCostumeStatClient::ResetStatPos(COSTUME_STATS eSt)
{
	MIter _mIter = m_InvestPoint.find( eSt );

	if( _mIter != m_InvestPoint.end() )
	{
		_mIter->second = 0;
	}
}

bool CCostumeStatClient::IsInvestEnable(COSTUME_STATS eSt)
{
	/*
	COSTUMESTATSDATA::CNDuplication::vData vDupleList;
	COSTUMESTATSDATA::CNDuplication::vIter _iter;

	if( COSSDATA.IsNDupleExist(eSt, vDupleList) )
	{
		_iter = vDupleList.begin();

		while( _iter != vDupleList.end())
		{			
			if( GetNumeric(static_cast<COSTUME_STATS>(*_iter)) > 0 ) return false;
			
			++_iter;
		}
	}
	*/

	COSTUMESTATSDATA::CNDuplication::mList mDupleList;
	COSTUMESTATSDATA::CNDuplication::mIter _miter;

	COSTUMESTATSDATA::CNDuplication::vIter _iter;

	COSSDATA.GetNDupleList(eSt, mDupleList );

	_miter = mDupleList.begin();

	while( _miter != mDupleList.end() )
	{
		_iter = _miter->second.begin();

		while( _iter != _miter->second.end() )
		{
			COSTUME_STATS eCsNum = static_cast<COSTUME_STATS>(*_iter);

			if( eCsNum != eSt )
			{
				if( GetNumeric(eCsNum) > 0 ) return false;
			}

			++_iter;
		}

		++_miter;
	}

	return true;
}

bool  CCostumeStatClient::GetData( GLMSG::SNET_INVEN_COSTUME_STATS::SINVESTINFO* pData )
{
	int iRefNum = 0;

	MIter _iter = m_InvestPoint.begin();

	while( _iter != m_InvestPoint.end() )
	{		
		if( _iter->first != E_NONE && _iter->second > 0 && iRefNum < COSTUMEUSERVALUE::sCostumeUser::MAX_COS_SIZE )
		{
			pData[iRefNum].eStatsNum = _iter->first;
			pData[iRefNum].wInvestPt = _iter->second;

			++iRefNum;
		}

		++_iter;
	}

	return (iRefNum > 0) ? true : false ;
}

bool CCostumeStatClient::SetItemInfo( const SINVENITEM* pSrc, const SINVENITEM* pDst )
{
	if( NULL == pSrc || NULL == pDst )
		return false;

	const SITEM* pSrcItem = GLogicData::GetInstance().GetItem( pSrc->sItemCustom.GetNativeID() );
	const SITEM* pDstItem = GLogicData::GetInstance().GetItem( pDst->sItemCustom.GetNativeID() );
	if( NULL == pSrcItem || NULL == pDstItem )
		return false;

	WORD wLvPoint = COSSDATA.GetPartsEnableMaxPt( pDstItem->sSuitOp.emSuit, (WORD) pSrcItem->sDrugOp.dwCureVolume );
	if( 0 == wLvPoint )
	{
		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_CostumeStat_Error,
			"-n", (int) NSCostumeStat::EMError_NoPoint );

		return false;
	}

	RestMember();

	const COSTUMESTATSDATA::CPartsDt::mPsData* const pData = COSSDATA.GetStatInfo( pDstItem->sSuitOp.emSuit );
	if( NULL == pData )
		return false;

	SetSuit( pDstItem->sSuitOp.emSuit );
	SetHavePoint( wLvPoint );
	SetItem( pSrc, pDst );

	return true;
}

WORD CCostumeStatClient::GetMaxStatPoint( COSTUME_STATS emStat )
{
	return COSSDATA.GetStatMaxPt( (EMSUIT) m_iCurSuit, emStat );
}

double CCostumeStatClient::GetStatInc( COSTUME_STATS emStat )
{
	return COSSDATA.GetStatInc( (EMSUIT) m_iCurSuit, emStat );
}