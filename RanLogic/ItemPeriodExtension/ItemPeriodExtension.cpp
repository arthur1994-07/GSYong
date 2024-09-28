
#include "pch.h"

#include "../../EngineLib/G-Logic/TypeDefine.h"
#include "../../EngineLib/G-Logic/GLogic.h"

#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/String/StringUtil.h"
#include "../../SigmaCore/File/BaseStream.h"
#include "../../SigmaCore/Memory/SmartPtr.h"
#include "../../SigmaCore/Lua/MinLua.h"

#include "../GLogicFile.h"
#include "../GLogicDataMan.h"
#include "../Item/GLItem.h"

#include "./ItemPeriodExtension.h"

namespace ItemPeriodExtension
{
	extern HRESULT LoadFile(GLogicFile* const pFileLogic, bool bTool)
	{
		const std::string strPathLogic(GLOGIC::GetPath())
			, strListFileName(pFileLogic->GetItemPeriodExtensionFileName())
			, strListFileFullPath(strPathLogic + strListFileName);

		std::auto_ptr<sc::BaseStream> pStreamBase(
			GLOGIC::openfile_basestream(
			true,
			GLogicData::GetInstance().GetGlogicZipFile(),
			strListFileFullPath.c_str(),
			strListFileName,
			false,
			GLOGIC::UseLogicPackFile()));

		if ( pStreamBase.get() == 0 )
		{
			const std::string strError(sc::string::format("ItemPeriodExtension::LoadFile can't find file : %1%", strListFileFullPath));
			::MessageBox(0, strError.c_str(), "error", MB_OK);
			return E_FAIL;
		}

		const size_t nSize = static_cast< size_t >( pStreamBase->GetFileSize() );
		void* pBuffer = ::malloc(nSize);
		pStreamBase->ReadBuffer(pBuffer, 0);
		CTypeSmartPtr _pSmartPtr(pBuffer);

		sc::lua_init();
		if ( !sc::lua_doBuffer(static_cast<char*>(pBuffer), nSize, NULL) )
			return E_FAIL;

		LuaPlus::LuaObject loItemList = sc::lua_getGlobalFromName("FileList");

		std::vector<std::string> vecFileList;
		vecFileList.reserve(loItemList.GetTableCount());
		for ( LuaPlus::LuaTableIterator itrFile(loItemList); itrFile.IsValid(); itrFile.Next() )
		{
			const std::string strFile(sc::string::unicodeToAnsi((const wchar_t*)itrFile.GetValue().GetWString()));
			vecFileList.push_back(strFile);
		}

		// for entity load...
		const INT nFileSize(static_cast<INT>(vecFileList.size()));
		for ( INT i = 0; i < nFileSize; ++i )
		{
			Manager::GetInstance().Insert(vecFileList[i], bTool);
		}
		return S_OK;
	}

	Entity::Entity(void)
	{
	}

	Entity::~Entity(void)
	{
	}

	HRESULT Entity::LoadFile(std::string& strFileName, bool bTool)
	{
		const std::string strPathLogic(GLOGIC::GetPath())
			, strFullPath(strPathLogic + strFileName);

		std::auto_ptr<sc::BaseStream> pStreamBase(
			GLOGIC::openfile_basestream(
			true,
			GLogicData::GetInstance().GetGlogicZipFile(),
			strFullPath.c_str(),
			strFileName,
			false,
			GLOGIC::UseLogicPackFile()));

		if ( pStreamBase.get() == 0 )
		{
			const std::string strError(sc::string::format("ItemPeriodExtension::Entity::LoadFile can't find file : %1%", strFullPath));
			::MessageBox(0, strError.c_str(), "error", MB_OK);
			return E_FAIL;
		}

		const size_t nSize = static_cast< size_t >( pStreamBase->GetFileSize() );
		void* pBuffer = ::malloc(nSize);
		pStreamBase->ReadBuffer(pBuffer, 0);
		CTypeSmartPtr _pSmartPtr(pBuffer);

		sc::lua_init();
		if ( !sc::lua_doBuffer(static_cast<char*>(pBuffer), nSize, NULL) )
			return E_FAIL;

		LuaPlus::LuaObject loExtensionCnt = sc::lua_getGlobalFromName("ExtensionCount");

		strcpy_s( m_szName, FILENAME_LENGTH, strFileName.c_str() );
		m_nPeriodExtensionCount = sc::GetIntegerByObject(loExtensionCnt);

		// load
		LuaPlus::LuaObject loItemList = sc::lua_getGlobalFromName("ItemList");
		for ( LuaPlus::LuaTableIterator itrItem( loItemList ); itrItem.IsValid(); itrItem.Next() )
		{
			for ( LuaPlus::LuaTableIterator itrID( itrItem.GetValue() ); itrID.IsValid(); itrID.Next() )
			{
				INT nMID(itrID.GetValue().GetInteger()), nSID;	itrID.Next();
				nSID = itrID.GetValue().GetInteger();

				m_vecItemList.push_back(SNATIVEID(nMID, nSID));

				// 툴을 제외하고 클라, 서버일 경우 검색 속도를 위해 다음과 같은 추가작업을 함;
				// 기간 연장 아이템인 경우를 제외하고;
				// SITEM->sBasicOp.strItemPeriodExtension에 기간 연장 리스트를 등록해둔다;
				// SITEM->sBasicOp.strItemPeriodExtension는 기간 연장 아이템만 사용한다;
				if ( bTool == false )
				{
					SITEM* pItem = GLogicData::GetInstance().GetItem( nMID, nSID );
					if ( pItem && (pItem->sBasicOp.emItemType != ITEM_PERIODEXTANSION &&
						pItem->sDrugOp.emDrug != ITEM_DRUG_PERIODEXTENTION) )
					{
						pItem->sBasicOp.strItemPeriodExtension = strFileName;
					}
				}
			}
		}

		return S_OK;
	}

	bool Entity::IsRegItem(const SNATIVEID& nidCheck) const
	{
		const INT nSize(static_cast<INT>(m_vecItemList.size()));
		for ( INT i = 0; i < nSize; ++i )
		{
			if ( m_vecItemList[i].dwID == nidCheck.dwID )
				return true;
		}
		return false;
	}

	Manager& Manager::GetInstance(void)
	{
		static Manager s_manager;
		return s_manager;
	}

	Manager::Manager(void)
	{
	}

	Manager::~Manager(void)
	{
	}

	HRESULT Manager::Insert(std::string strFileName, bool bTool)
	{
		Entity entity;
		if ( entity.LoadFile(strFileName, bTool) != S_OK )
			return E_FAIL;

		m_mapEntityList.insert( std::make_pair(strFileName, entity) );
		return S_OK;
	}

	/*
	HRESULT Manager::IsExtendableItem(const SINVENITEM* pExtensionInvenItem, const SINVENITEM* pCheckInvenItem)
	{
		if ( !pExtensionInvenItem || !pCheckInvenItem )
			return E_FAIL;

		const SITEMCUSTOM& sCustomItem = pCheckInvenItem->sItemCustom;

		// 코스튬, 일반 확인;
		{
			bool bBase(false), bCostume(false);
			const SITEM* pNormalItem = GLogicData::GetInstance().GetItem ( sCustomItem.GetNativeID() );
			if ( pNormalItem )
				bBase = pNormalItem->sBasicOp.dwFlags & ITEM_TIMELMT;

			const SITEM* pItem = GLogicData::GetInstance().GetItem ( sCustomItem.nidDISGUISE );
			if ( pItem )
				bCostume = pItem->sBasicOp.dwFlags & ITEM_TIMELMT;
		}

		SITEM* pExtensionItem = GLogicData::GetInstance().GetItem( pExtensionInvenItem->GetNativeID() )
			,* pCheckItem = GLogicData::GetInstance().GetItem( pCheckInvenItem->GetNativeID() );
		if ( pExtensionItem->sBasicOp.emItemType != ITEM_PERIODEXTANSION )
			return S_FALSE;

		if ( pCheckItem->ISINSTANCE() || pCheckItem->ISPILE() )
			return S_FALSE;
	}
	*/

	HRESULT Manager::IsExtendableItem(const SNATIVEID& nidExtension, const SNATIVEID& nidCheckItem, OUT INT& nMaxCount)
	{
		// 코스튬, 일반 확인;
		SITEM* pExtensionItem = GLogicData::GetInstance().GetItem( nidExtension )
			,* pCheckItem = GLogicData::GetInstance().GetItem( nidCheckItem );
		if ( pExtensionItem->sBasicOp.emItemType != ITEM_PERIODEXTANSION ||
			 pExtensionItem->sDrugOp.emDrug != ITEM_DRUG_PERIODEXTENTION )
			return S_FALSE;

		if ( !pCheckItem || pCheckItem->ISINSTANCE() || pCheckItem->ISPILE()
			|| (IsRegItem(nidExtension, nidCheckItem, nMaxCount) == false) )
			return S_FALSE;

		return S_OK;
	}

	// 툴을 제외하고 클라, 서버일 경우 검색 속도를 위해 다음과 같은 추가작업이 되어있음;
	// 기간 연장 아이템인 경우를 제외하고;
	// SITEM->sBasicOp.strItemPeriodExtension에 기간 연장 리스트를 등록해둔다;
	// SITEM->sBasicOp.strItemPeriodExtension는 기간 연장 아이템만 사용한다;
	bool Manager::IsRegItem(const SNATIVEID& nidCheck, OUT INT& nCount)
	{
		SITEM* pItem = GLogicData::GetInstance().GetItem( nidCheck );
		if ( pItem && (pItem->sBasicOp.emItemType != ITEM_PERIODEXTANSION &&
			pItem->sDrugOp.emDrug != ITEM_DRUG_PERIODEXTENTION) )
		{
			MAP_ENTITYLIST_ITR itrEntity = m_mapEntityList.find(pItem->sBasicOp.strItemPeriodExtension);
			if ( itrEntity == m_mapEntityList.end() )
				return false;

			const Entity& entity = (itrEntity->second);
			if ( entity.IsRegItem(nidCheck) )
			{
				nCount = entity.m_nPeriodExtensionCount;
				return true;
			}
		}

		MAP_ENTITYLIST_ITR itrEntity = m_mapEntityList.begin();
		for (; itrEntity != m_mapEntityList.end(); ++itrEntity )
		{
			const Entity& entity = itrEntity->second;
			if ( entity.IsRegItem(nidCheck) )
			{
				nCount = entity.m_nPeriodExtensionCount;
				return true;
			}
		}
		return false;
	}

	bool Manager::IsRegItem(const SNATIVEID& nidExtension, const SNATIVEID& nidCheck)
	{
		SITEM* pExtensionItem = GLogicData::GetInstance().GetItem( nidExtension );
		MAP_ENTITYLIST_ITR itrEntity = m_mapEntityList.find(pExtensionItem->sBasicOp.strItemPeriodExtension);
		if ( itrEntity == m_mapEntityList.end() )
			return false;

		const Entity& entity = (itrEntity->second);
		if ( entity.IsRegItem(nidCheck) )
			return true;

		return false;
	}

	bool Manager::IsRegItem(const SNATIVEID& nidExtension, const SNATIVEID& nidCheck, OUT INT& nCount)
	{
		SITEM* pExtensionItem = GLogicData::GetInstance().GetItem( nidExtension );
		MAP_ENTITYLIST_ITR itrEntity = m_mapEntityList.find(pExtensionItem->sBasicOp.strItemPeriodExtension);
		if ( itrEntity == m_mapEntityList.end() )
			return false;

		const Entity& entity = (itrEntity->second);
		if ( entity.IsRegItem(nidCheck) )
		{
			nCount = entity.m_nPeriodExtensionCount;
			return true;
		}

		return false;
	}

	// client test;
	DWORD Manager::DoTest(const SITEMCUSTOM& sExtensionItem, const SITEMCUSTOM& sCheckItem, __time64_t& tPrevTime, __time64_t& tNextTime)
	{
		INT nBaseMaxCount, nCostMaxCount;
		const bool bBase = IsExtendableItem(sExtensionItem.GetNativeID(), sCheckItem.GetNativeID(), nBaseMaxCount) == S_OK
			, bCostume = IsExtendableItem(sExtensionItem.GetNativeID(), sCheckItem.nidDISGUISE, nCostMaxCount) == S_OK;

		const SITEM* pHold = GLogicData::GetInstance().GetItem( sExtensionItem.GetNativeID() );
		// 둘다 적용가능할 수 없다;   
		if ( bBase && bCostume )
		{
			return EMPERIODEX_FB_FAIL_DUPLICATION;
		}

		// 일반 아이템과 코스튬은 따로 처리해줘야한다;
		if ( bBase )
		{
			// 시효성 정보가 없다;
			if ( sCheckItem.IsTimeLimit(SITEMCUSTOM::CHECKFLAGOP_NORMAL) == false )
			{
				return EMPERIODEX_FB_FAIL;
			}

			const SITEM* pBaseItem = GLogicData::GetInstance().GetItem( sCheckItem.GetNativeID() );

			tPrevTime = sCheckItem.tBORNTIME + sCheckItem.GetPeriodExBasic() + pBaseItem->sDrugOp.tTIME_LMT;
			tNextTime = tPrevTime + pHold->sDrugOp.dwCureVolume;
			if ( nBaseMaxCount != 0 && sCheckItem.cPeriodExtensionCount >= nBaseMaxCount )
			{
				return EMPERIODEX_FB_FAIL_LIMITCOUNT;
			}
			return EMPERIODEX_FB_SUCCESS_BASE;
		}
		else if ( bCostume )
		{
			// 시효성 정보가 없다;
			if ( sCheckItem.IsTimeLimit(SITEMCUSTOM::CHECKFLAGOP_COSTUME) == false )
			{
				return EMPERIODEX_FB_FAIL;
			}

			tPrevTime = sCheckItem.tDISGUISE + sCheckItem.GetPeriodExDisguise();
			tNextTime = tPrevTime + pHold->sDrugOp.dwCureVolume;
			if ( nCostMaxCount != 0 &&  sCheckItem.cCostumePeriodExtensionCount >= nCostMaxCount )
			{
				return EMPERIODEX_FB_FAIL_LIMITCOUNT;
			}
			return EMPERIODEX_FB_SUCCESS_COSTUME;
		}
		else
		{
			return EMPERIODEX_FB_FAIL;
		}
		return EMPERIODEX_FB_SUCCESS;
	}

}