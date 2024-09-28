#include "pch.h"

//#include "../../EngineLib/G-Logic/GLPeriod.h"
#include "../ClientActor.h"
#include "../GLGaeaClient.h"
#include "DamageRecord.h"


namespace DamageRecord
{
	Manager::Manager(void)
		: m_bUse(false)
	{
		m_vectorViewSlot.reserve(VIEW_SLOT_NSIZE);
	}

	Manager::~Manager(void)
	{
	}

	const bool Manager::isOn(void) const
	{
		return m_bUse;
	}
	void Manager::turnOn(void)
	{
		reset();
		m_bUse = true;
	}

	void Manager::resetActorByIndex(const DWORD nIndex)
	{
		if ( nIndex > (m_vectorViewSlot.size() - 1) )
			return;

		const LogID& ActorID = m_vectorViewSlot[nIndex];
		ViewMapIter iterView = m_mapView.find(ActorID);
		if ( iterView == m_mapView.end() )
			return;

		ViewElement& Element = iterView->second;
		Element.reset();
	}

	void Manager::addActor(const DWORD dwDbNum, const char* const szName)
	{
		_addActor(LogID(dwDbNum), szName);
	}
	void Manager::addActor(const WORD wMainID, const WORD wSubID, const char* const szName)
	{
		_addActor(LogID(wMainID, wSubID), szName);
	}
	BOOL Manager::_addActor(const LogID& ActorID, const char* const szName)
	{
		if ( m_bUse == false )
			return FALSE;

		if ( szName == NULL )
			return FALSE; // ActorID에 대한 유효성 검사;

		{ // Use By UI;
			if ( m_vectorViewSlot.size() < VIEW_SLOT_NSIZE )
			{ // 등록 될 공간이 있다면 등록;
				const ViewSlotVectorCIter iterSlot = std::find(m_vectorViewSlot.begin(), m_vectorViewSlot.end(), ActorID);
				if ( iterSlot == m_vectorViewSlot.end() )
					m_vectorViewSlot.push_back(ActorID);
			}
		}

		{ // Use By Log;
			ViewMapCIter iterView = m_mapView.find(ActorID);
			if ( iterView != m_mapView.end() )
				return TRUE; // 이미 등록 되어 있다면 리턴;		

			m_mapView.insert(ViewMapValue(ActorID, ViewElement(ActorID, szName)));
		}

		return TRUE;
	}

	void Manager::dropActor(const DWORD dwDbNum)
	{
		_dropActor(LogID(dwDbNum));
	}
	void Manager::dropActor(const WORD wMainID, const WORD wSubID)
	{
		_dropActor(LogID(wMainID, wSubID));
	}
	void Manager::dropActorByIndex(const DWORD nIndex)
	{ // Use By UI;
		if ( nIndex > (m_vectorViewSlot.size() - 1) )
			return;

		const LogID& ActorId = m_vectorViewSlot[nIndex];
		_dropActor(ActorId);		
	}
	void Manager::_dropActor(const LogID& ActorID)
	{ 
		const ViewSlotVectorIter endSlot = m_vectorViewSlot.end();
		const ViewSlotVectorIter iterSlot = std::find(m_vectorViewSlot.begin(), endSlot, ActorID);
		if ( iterSlot == endSlot )
			return;

		m_vectorViewSlot.erase(iterSlot);

		// 파일로 남겨야 하기 때문에 reset()만 해주고 삭제 하지는 않음;
		ViewMapIter iterView = m_mapView.find(ActorID);
		if ( iterView == m_mapView.end() )
			return;

		ViewElement& Element = iterView->second;
		Element.reset();		
	}

	bool Manager::doDamage(const DWORD dwDbNum, const char* const szName, const DWORD nDamage, const SNATIVEID& SkillID, const WORD wSkillLevel, const DWORD dwDamageFlag)
	{
		return _doDamage(LogID(dwDbNum), szName, nDamage, SkillID, wSkillLevel, dwDamageFlag);
	}
	bool Manager::doDamage(const WORD wMainID, const WORD wSubID, const char* const szName, const DWORD nDamage, const SNATIVEID& SkillID, const WORD wSkillLevel, const DWORD dwDamageFlag)
	{
		return _doDamage(LogID(wMainID, wSubID), szName, nDamage, SkillID, wSkillLevel, dwDamageFlag);
	}
	bool Manager::_doDamage(const LogID& ActorID, const char* const szName, const DWORD nDamage, const SNATIVEID& SkillID, const WORD wSkillLevel, const DWORD dwDamageFlag)
	{
		if ( m_bUse == false )
			return false;

		const ViewSlotVectorIter endSlot = m_vectorViewSlot.end();
		const ViewSlotVectorCIter iterSlot = std::find(m_vectorViewSlot.begin(), endSlot, ActorID);
		if ( iterSlot == endSlot )
			return false; // ViewSlot에 등록 되어 있지 않다면 갱신 할 필요 없음;

		const float fTime = float(clock()) * 0.001f;

		ViewMapIter iterView = m_mapView.find(ActorID);
		if ( iterView == m_mapView.end() )
		{
			if ( !_addActor(ActorID, szName) )
				return false;

			iterView = m_mapView.find(ActorID);
			if ( iterView == m_mapView.end() )
			{
				return false;
			}
		}

		ViewElement& Element = iterView->second;
		Element.doDamage(nDamage, fTime, SkillID, wSkillLevel, dwDamageFlag);

		return true;
	}

	void Manager::reset(void)
	{
		m_mapView.clear();
		m_vectorViewSlot.clear();
	}

	const DWORD Manager::size(void) const
	{
		return m_vectorViewSlot.size();
	}

	const ViewElement* const Manager::GetViewElement(const DWORD nSlotIndex) const
	{
		if ( nSlotIndex >= m_vectorViewSlot.size() )
			return NULL;

		const LogID& ActorID = m_vectorViewSlot[nSlotIndex];
		const ViewMapCIter iter = m_mapView.find(ActorID);
		if ( iter == m_mapView.end() )
			return NULL;

		return &iter->second;
	}

	const bool Manager::saveCSV(void)
	{
		// 현재시각
		CTime Time(CTime::GetCurrentTime());
		const int nYear = Time.GetYear();
		const int nMonth = Time.GetMonth();
		const int nDay = Time.GetDay();
		const int nHour = Time.GetHour();
		const int nMinute = Time.GetMinute();

		CString strFileName;
		strFileName.Format("./log/DPS_%02d%02d%02d_%02d%02d.csv", nYear, nMonth, nDay, nHour, nMinute);			

		std::fstream streamFile;
		streamFile.open(strFileName, std::ios_base::out);

		{ // Element Header;
			for ( DWORD _i = LOG_ATTRIBUTE_NSIZE; _i; --_i )
				streamFile<<strAttribute[LOG_ATTRIBUTE_NSIZE - _i]<<",";
			streamFile<<std::endl;
		}

		{ // Elements Log;
			for ( ViewMapIter iter = m_mapView.begin(); iter != m_mapView.end(); ++iter )
			{
				ViewElement& Element = iter->second;
				Element.saveCSV(streamFile);
			}
		}
		

		streamFile.close();
		return true;
	}

	LuaTable Manager::GetRecord()
 	{
		LuaTable tbRecordList( GLWidgetScript::GetInstance().GetLuaState() );

		int nTableIndex = 1;
		for( int i = 0; i < VIEW_SLOT_NSIZE; ++i )
		{
			const ViewElement* const pElement = GetViewElement( i );
			if( NULL == pElement )
				continue;

			const float fTime = pElement->m_Calculator.m_fElpasedTime;
			const int nMinute = int( fTime * ( 1.0f / 60.0f ) );
			const int nSecond = int( fTime - float( nMinute * 60 ) );

			LuaTable tbRecord( GLWidgetScript::GetInstance().GetLuaState() );
			tbRecord.set( "strName", pElement->m_szName );
			tbRecord.set( "nDamage", pElement->m_nAccumDamage );
			tbRecord.set( "nMinute", nMinute );
			tbRecord.set( "nSecond", nSecond );
			tbRecord.set( "fDPS", pElement->m_Calculator.m_fDPS );

			if( pElement->m_SkillID == SNATIVEID( false ) )
			{
				tbRecord.set( "MID", -1 );
				tbRecord.set( "SID", -1 );
				tbRecord.set( "nSkillLevel", -1 );
			}
			else
			{
				tbRecord.set( "MID", pElement->m_SkillID.Mid() );
				tbRecord.set( "SID", pElement->m_SkillID.Sid() );
				tbRecord.set( "nSkillLevel", pElement->m_wSkillLevel + 1);
			}

			tbRecordList.set( nTableIndex++, tbRecord );
		}

		return tbRecordList;
	}

	LuaTable Manager::GetDetail( int nIndex )
	{
		LuaTable tbDetailList( GLWidgetScript::GetInstance().GetLuaState() );

		const ViewElement* const pElement = GetViewElement( nIndex );
		if( NULL == pElement )
			return tbDetailList;

		int nTableIndex = 1;
		ViewElement::DetailMapCIter iter = pElement->m_mapDetail.begin();
		for( ; iter != pElement->m_mapDetail.end(); ++iter )
		{
			const DetailElement& pNode = iter->second;

			LuaTable tbDetail( GLWidgetScript::GetInstance().GetLuaState() );

			if( pNode.m_SkillID == SNATIVEID( false ) )
				tbDetail.set( "strName", "Normal" );
			else
				tbDetail.set( "strName", GLSkillMan::GetInstance().GetSkillName( pNode.m_SkillID ).c_str() );

			tbDetail.set( "nDamage", pNode.m_nDamage );
			tbDetail.set( "nUseCount", pNode.m_nUseCounter );
			tbDetail.set( "nDPS", pNode.m_nDamage / pNode.m_nUseCounter );
			tbDetail.set( "nRate", ( pNode.m_nDamage * 100 ) / 
				( 0 != pElement->m_nAccumDamage ? pElement->m_nAccumDamage : 1 ) );

			tbDetailList.set( nTableIndex++, tbDetail );
		}

		return tbDetailList;
	}
}