#include "pch.h"

#include "../../RanLogic/Item/ItemSet/GLItemSet.h"
#include "../../RanLogic/GLogicEx.h"

#include "../../../SigmaCore/String/StringFormat.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"

#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxEx.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/BasicVarTextBox.h"

#include "../../RanLogic/GLUseFeatures.h"
#include "../../RanLogic/Item/ItemDurability.h"
#include "../../../SigmaCore/Lua/MinLua.h"
#include "../../../EngineLib/G-Logic/TypeDefine.h"
#include "../../../EngineLib/G-Logic/GLogic.h"


//#include "../../../EngineLib/G-Logic/GLDefine.h"

#include "../../GLogicDataMan.h"

GLItemSet::GLItemSet(void)
:m_dwMaxSetList(0), m_CountCallLoad(-1)
{
	
}

GLItemSet::~GLItemSet(void)
{

}

GLItemSet& GLItemSet::GetInstance()
{
	static GLItemSet g_GLItemSet;
	return g_GLItemSet;
}

bool GLItemSet::LoadFile(const std::string& strName, bool bToolMode)
{
 	std::string strPath(GLOGIC::GetPath());  //GLOGIC헤더가 포함되어야함

	/*if(bToolMode)
	{
		strPath.append(_T("SetItemList.lua"));
		m_vecSetList.clear();
		m_vecSetItem.clear();
		m_CountCallLoad = -1;
		m_dwMaxSetList = NULL;
	}
	else*/
	{
		std::string tempName =  "SetItemList.luc";
		if(tempName.c_str() == strName.c_str())
		{
			m_vecSetList.clear();
			m_vecSetItem.clear();
			m_CountCallLoad = -1;
			m_dwMaxSetList = NULL;
		}

		strPath.append(strName);
	}
	//Tool에서는 lua파일을, 게임에서는 luc파일을 읽음
	return LoadFile_Lua(strPath);
}

void GLItemSet::LoadSetName(const std::string& strTitle, std::string* pName)
{
	std::wstring ws;
	try
	{	
		LuaPlus::LuaObject ob = sc::lua_getGlobalFromName(strTitle.c_str());
		LuaPlus::LuaTableIterator it(ob);	//테이블에 넣으려면 스크립트에 배열로 설정되어 있어야한다
		ws = reinterpret_cast<const wchar_t*>(it.GetValue().GetWString());
		*pName = sc::string::unicodeToAnsi(ws);		
	}//스크립트에서 타이틀과 일치하는 부분을 찾아서 1개씩 넣어줌(한 줄에 2개이상일 때) 
	//숫자열 같은 경우는 사용 불가능한 함수 재정의 필요
	catch(const LuaPlus::LuaException& e)
	{
		MessageBox(NULL, e.GetErrorMessage(), strTitle.c_str(), MB_OK);
	}
}

template <typename T>
void GLItemSet::LoadNum(const std::string& strTitle, T &pNum)
{
	try
	{
		LuaPlus::LuaObject ob = sc::lua_getGlobalFromName(strTitle.c_str());
		pNum = sc::GetIntegerByObject(ob);	
	}
	catch(const LuaPlus::LuaException& e)
	{
		MessageBox(NULL, e.GetErrorMessage(), strTitle.c_str(), MB_OK);
	}
}

template <typename T>
void GLItemSet::LoadSetList(const std::string& strTitle, T Count)
{
	std::wstring ws;
	SITLIST temp;
	try
	{
		int i = 0;
		LuaPlus::LuaObject ob = sc::lua_getGlobalFromName(strTitle.c_str());
		for(LuaPlus::LuaTableIterator it(ob); it.IsValid(); it.Next(), ++i)
		{
			if(i >= (int)Count)
				break;

			DWORD temp_Num = static_cast<DWORD>(it.GetValue().GetFloat());
			temp.dwSetID = temp_Num;
			it.Next();
			ws = reinterpret_cast<const wchar_t*>(it.GetValue().GetWString());
			temp.strSetName = sc::string::unicodeToAnsi(ws);

			m_vecSetList.push_back(temp);
		}
	}
	catch(const LuaPlus::LuaException& e)
	{
		MessageBox(NULL, e.GetErrorMessage(), strTitle.c_str(), MB_OK);
	}
}//세트리스트 스크립트 로드

void GLItemSet::LoadIDList(const std::string& strTitle, std::vector<SNATIVEID> &_Setitem, BYTE MaxIDNum)
{
	BYTE i = 0;
	LuaPlus::LuaObject ob = sc::lua_getGlobalFromName(strTitle.c_str());	
	try
	{
		SNATIVEID Temp;
		for(LuaPlus::LuaTableIterator it(ob); it.IsValid(); it.Next(), ++i)
		{
			if(i >= MaxIDNum)
				break;

			Temp.wMainID = static_cast<WORD>(it.GetValue().GetInteger());			
			it.Next();
			Temp.wSubID = static_cast<WORD>(it.GetValue().GetInteger());	
			_Setitem.push_back(Temp);
		}		
	}
	catch(const LuaPlus::LuaException& e)
	{
		MessageBox(NULL, e.GetErrorMessage(), strTitle.c_str(), MB_OK);
	}
}//세트 구성 아이템 ID로드

void GLItemSet::LoadBonusList(const std::string& strTitle, std::vector<SSIB> &_vecBonus, BYTE MaxBonusNum)
{
	BYTE i = 0;
	LuaPlus::LuaObject ob = sc::lua_getGlobalFromName(strTitle.c_str());
	try
	{	
		SSIB Temp;
		for(LuaPlus::LuaTableIterator it(ob); it.IsValid(); it.Next(), ++i)
		{
			if(i >= MaxBonusNum)
				break;

			Temp.byTerms = static_cast<BYTE>(it.GetValue().GetInteger());
			it.Next();

			Temp.sSpec.emSPEC		= static_cast<EMSPEC_ADDON>( it.GetValue().GetInteger() );
			it.Next();

			Temp.sSpec.sSPEC.fVAR1	= it.GetValue().GetFloat();

			_vecBonus.push_back(Temp);
		}
	}
	catch(const LuaPlus::LuaException& e)
	{
		MessageBox(NULL, e.GetErrorMessage(), strTitle.c_str(), MB_OK);
	}
}//효과 설정부 로드

void GLItemSet::LoadSlotIndicationIcon(const std::string& strTitle, std::string& strSlotIndication)
{
	std::wstring ws;
	try
	{	
		LuaPlus::LuaObject ob = sc::lua_getGlobalFromName(strTitle.c_str());
		LuaPlus::LuaTableIterator it(ob);	//테이블에 넣으려면 스크립트에 배열로 설정되어 있어야한다
		ws = reinterpret_cast<const wchar_t*>(it.GetValue().GetWString());
		strSlotIndication = sc::string::unicodeToAnsi(ws);		
	}//스크립트에서 타이틀과 일치하는 부분을 찾아서 1개씩 넣어줌(한 줄에 2개이상일 때) 
	//숫자열 같은 경우는 사용 불가능한 함수 재정의 필요
	catch(const LuaPlus::LuaException& e)
	{
		MessageBox(NULL, e.GetErrorMessage(), strTitle.c_str(), MB_OK);
	}
}

void GLItemSet::LoadSetItemInfo(DWORD Count)
{		
		SSETITEM Temp;

		LoadNum(_T("Set_ID"),Temp.dwSetID);
		LoadSetName(_T("Set_Name"), &Temp.strName);
		LoadNum(_T("MaxItemList"), Temp.byMaxNumItem);
		LoadNum(_T("MaxToolTipItemList"), Temp.byMaxToolTipItemList );
		//아이템 리스트 보다 튤팁 라인이 클수 없음 dyha
		//Temp.byMaxToolTipItemList > Temp.byMaxNumItem? Temp.byMaxNumItem:Temp.byMaxToolTipItemList;
		LoadNum(_T("MaxPieceState"), Temp.byMaxNumBonus);
		LoadIDList(_T("Set_Item_List"), Temp.vecItemList, Temp.byMaxNumItem);
		LoadBonusList(_T("Set_Piece_State"), Temp.vecBonus, Temp.byMaxNumBonus);
		LoadSlotIndicationIcon(_T("SetSlot_IndicationIcon"), Temp.strSlotIndicationIcon );
		
		//최종 로딩상 예외처리 할 부분
		m_vecSetItem.push_back(Temp);
}//아이템 정보 스크립트 전체 로드

bool GLItemSet::ParseSetItem()
{
	if(m_dwMaxSetList == NULL)
	{
		LoadNum(_T("m_MaxSetList"), m_dwMaxSetList);
		LoadSetList(_T("Set_List"), m_dwMaxSetList);
	}
	else
	{
		LoadSetItemInfo(m_CountCallLoad);	
	}

	if(m_CountCallLoad >= (char)m_dwMaxSetList-1) return true;
	
	m_CountCallLoad++;
	return LoadFile(m_vecSetList[m_CountCallLoad].strSetName, false);
}//리스트 스크립트, 세트아이템 설정정보 스크립트 모두 로드

bool GLItemSet::LoadFile_Lua(const std::string& strPath)
{
	if(strPath.empty()) return false;

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
		sc::writeLogError(sc::string::format("GLSetItem::LoadFile_Lua, File Open %1%", strPath));
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
		return ParseSetItem();
	}
	catch(const LuaPlus::LuaException& e)
	{
		MessageBox(NULL, e.GetErrorMessage(), strPath.c_str(), MB_OK);
		return false;
	}
	return false;
}

const bool GLItemSet::IsSetItem(const SNATIVEID &sNativeID) const
{
	//검색 테스트 1
	for(DWORD i = 0; i < m_vecSetItem.size(); ++i)
	{
		for(DWORD j = 0; j < m_vecSetItem[i].vecItemList.size(); ++j)
		{
			if(sNativeID == m_vecSetItem[i].vecItemList[j]) return true;
		}
	}

	return false;
}

const SSETITEM*	GLItemSet::GetInfoSetItem(const SNATIVEID &sNativeID) const
{
	
	for(DWORD i = 0; i < m_vecSetItem.size(); ++i)
	{
		for(DWORD j = 0; j < m_vecSetItem[i].vecItemList.size(); ++j)
		{
			if(sNativeID == m_vecSetItem[i].vecItemList[j]) 
				return &m_vecSetItem[i];
		}
	}

	return NULL;
}

const SSETITEM*	GLItemSet::GetInfoSetItem(const DWORD &dwSetID) const
{

	for(DWORD i = 0; i < m_vecSetItem.size(); ++i)
	{
		if(dwSetID == m_vecSetItem[i].dwSetID) 
			return &m_vecSetItem[i];
	}

	return NULL;
}

const SETITEMLIST* GLItemSet::GetInfoSetItemList(const DWORD &dwSetID) const
{
	for(DWORD i = 0; i < m_vecSetItem.size(); ++i)
	{
		if(dwSetID == m_vecSetItem[i].dwSetID) 
		{
			for(DWORD j = 0; j < m_vecSetList.size(); ++j)
			{
				if( dwSetID == m_vecSetList[j].dwSetID )
					return &m_vecSetList[j];
			}
		}
			
	}

	return NULL;

}

void GLItemSet::GetItemBonusStr(std::string *strResult, const DWORD &dwTargetSetItemID, const UINT nTargetBonus)
{
	if(NULL == strResult) return;

	const SSETITEM* sSetItem = GetInfoSetItem(dwTargetSetItemID);

	if(NULL == sSetItem) return;

	GetItemBonusStr(strResult, sSetItem, nTargetBonus);
}

void GLItemSet::GetItemBonusStr(std::string *strResult, const SSETITEM* sTargetSetItem, const UINT nTargetBonus)
{
	if(NULL == strResult) return;

	bool bStartStr = true;
	for(UINT i = 0; i < sTargetSetItem->vecBonus.size(); ++i)
	{
		if( !IsUsableOpt(sTargetSetItem->vecBonus[i].sSpec.emSPEC) ) continue;

		if(nTargetBonus == sTargetSetItem->vecBonus[i].byTerms)
		{			
			//간격 문자
			if(!bStartStr) 
			{
				*strResult += ", ";
				
			}

			bStartStr = false;

			//셋트효과 보너스
			if ( IsPerentOpt( sTargetSetItem->vecBonus[i].sSpec.emSPEC ) )
			{
				if( sTargetSetItem->vecBonus[i].sSpec.sSPEC.fVAR1 >= 0)//음수 양수 구분
					*strResult += sc::string::format( "%s +%.2f", ID2GAMEWORD("ITEM_INFO_SET_BONUS", sTargetSetItem->vecBonus[i].sSpec.emSPEC), sTargetSetItem->vecBonus[i].sSpec.sSPEC.fVAR1 );
				else
					*strResult += sc::string::format( "%s %.2f", ID2GAMEWORD("ITEM_INFO_SET_BONUS", sTargetSetItem->vecBonus[i].sSpec.emSPEC), sTargetSetItem->vecBonus[i].sSpec.sSPEC.fVAR1 );
				
				*strResult += _T("%");
			}
			else
			{
				if( sTargetSetItem->vecBonus[i].sSpec.sSPEC.fVAR1 >= 0)//음수 양수 구분
					*strResult += sc::string::format( "%s +%d", ID2GAMEWORD("ITEM_INFO_SET_BONUS", sTargetSetItem->vecBonus[i].sSpec.emSPEC), sTargetSetItem->vecBonus[i].sSpec.sSPEC.fVAR1 );
				else
					*strResult += sc::string::format( "%s %d", ID2GAMEWORD("ITEM_INFO_SET_BONUS", sTargetSetItem->vecBonus[i].sSpec.emSPEC), sTargetSetItem->vecBonus[i].sSpec.sSPEC.fVAR1 );
			}
		}
	}
}

/*스크립트로 불러둔 정보내에서 입력받은 아이템과 같은 아이템이 있는지 확인 후 셋트아이템인 경우에만 VSSETITEMINFO에 저장
VSSETITEMINFO &vSearchResult : 슬롯 아이템중 셋트 아이템인 항목 만 모아서 반환한다
const SITEMCUSTOM *sSlotItem : 유저 슬롯아이템 (주의! 1개가 아닌 iArraySize 만큼 확인한다)
UINT  iArraySize			 : SITEMCUSTOM 형태로 입력받은 포인터의 크기
*/
void GLItemSet::SearchSetItem(VSSETITEMINFO &vResult, const SITEMCUSTOM *SlotItem, const UINT iArraySize, bool bUseSubWeapon )
{
	if( NULL == SlotItem ) return;
	
	vResult.clear();	//좋지 않다 변수 중복 문제로 초기화 필수임

	for ( UINT i = 0; i< iArraySize; ++i )
	{	
		//1.셋트 목록에 존재여부 확인
		const SSETITEM *sSetItem = GLItemSet::GetInstance().GetInfoSetItem(SlotItem[i].GetNativeID());
		if(NULL == sSetItem) continue;

		//2.예외처리
		//2-1. 코스튬 아이템은 검색 불가능
		//const SITEM* pDisguise = GLogicData::GetInstance().GetItem( SlotItem[i].nidDISGUISE );
		//if( NULL != pDisguise ) continue;
		
		//2-2. 셋트아이템으로 사용 가능한 항목인지 확인 ex)장갑 신발 등등
		const SITEM *sItemTarget = GLogicData::GetInstance().GetItem( SlotItem[i].GetNativeID() );
		if(NULL == sItemTarget) continue;

		if( !IsSearchableItem(sItemTarget->sSuitOp.emSuit) ) continue;	

		EMSLOT eSlot = static_cast<EMSLOT>(i);
		
		//2-3. 내구도 확인 : 0이면 검색에서 제외
		if ( GLUseFeatures::GetInstance().IsUsingItemDurability() )
		{
			float fDurabilityRate( 1.0f );   // 기본적으로 100%

			if(  ItemDurability::GetInstance().IsUse( eSlot )  )
				fDurabilityRate= ItemDurability::GetInstance().GetPercent( SlotItem[i] );

			if( fDurabilityRate <= 0.0f) continue;
		}

		switch( eSlot )
		{
		case SLOT_RHAND :
		case SLOT_LHAND : if( bUseSubWeapon ) continue;
			break;
		case SLOT_RHAND_S :
		case SLOT_LHAND_S : if( !bUseSubWeapon ) continue;
			break;
		}
				
		//3. 셋트 아이템으로 판별나면 검색 목록에 넣어줌
		if(i)
		{
			bool bNewSetID(false); // 셋트 아이디 확인용
			
			for(WORD j = 0; j < vResult.size(); ++j)
			{
				if( sSetItem->dwSetID == vResult[j].dwSetID )// 한번 검색 된 셋트은 참조 횟수만 추가
				{
					//3-1. 동일한 셋트 목록 내 중복 아이디 판단
					bool bSameItem(false);
					for(WORD k = 0; k < vResult[j].vItemID.size(); ++k)
					{
						if( vResult[j].vItemID[k] == SlotItem[i].GetNativeID() ) 
							bSameItem = true;						
					}
										
					if( !bSameItem )
					{
						vResult[j].vItemID.push_back(SlotItem[i].GetNativeID());
						bNewSetID = true;
						break;
					}
				}
			}

			if(!bNewSetID)	//검색 된 적이 없는 셋트 아이템이니 새로운 리스트 추가
			{
				vResult.push_back(SSETITEMSEARCHINFO(sSetItem->dwSetID, SlotItem[i].GetNativeID()));
			}
		}
		else//처음 아무것도 검색이 안된 상태 i = 0
		{
			vResult.push_back(SSETITEMSEARCHINFO(sSetItem->dwSetID, SlotItem[i].GetNativeID()));
		}
	}
}

//튤팁 출력을 위한 정수와 실수 옵션 구분용 스크립트상 소숫점으로 잘못 표기도 가능하기 때문에 코드상에서 구분을 해준다
bool GLItemSet::IsPerentOpt(EMSPEC_ADDON INDEX)
{
	switch(INDEX)
	{	
		//실수표기가 아닌 부분은 추가 할 필요가 없다
		case EMSPECA_DAMAGE_RATE:
		case EMSPECA_DEFENSE_RATE:
		case EMSPECA_HIT_RATE:
		case EMSPECA_AVOID_RATE:
		case EMSPECA_HP_VARIATION_RATE:
		case EMSPECA_MP_VARIATION_RATE:
		case EMSPECA_SP_VARIATION_RATE:
		case EMSPECA_HMS_VARIATION_RATE:
		case EMSPECA_MOVEVELO:
		case EMSPECA_DAMAGE_RATE_ALL:

		case EMSPECA_ITEMDROP_RATE:
		case EMSPECA_MONEYDROP_RATE:
		case EMSPECA_EXP_RATE:
        case EMSPECA_MOVEVELO_LIMIT:
			return true;
	}

	return false;
}

//셋트 아이템으로 사용 불가능한 옵션을 적용하지 않기 위해 사용 할 옵션만 여기에 추가 해준다 
bool GLItemSet::IsUsableOpt(EMSPEC_ADDON INDEX)
{
	switch(INDEX)
	{
		// 기존 랜덤 옵션 기능
		case EMSPECA_DAMAGE_VALUE:
		case EMSPECA_DEFENSE_VALUE:
		case EMSPECA_HIT_VALUE:
		case EMSPECA_AVOID_VALUE:
		case EMSPECA_HP_VALUE:
		case EMSPECA_MP_VALUE:
		case EMSPECA_SP_VALUE:
		//case EMSPECA_CP_ADD:
		case EMSPECA_ATTACK_RANGE_VALUE:
		case EMSPECA_DIS_SP_VALUE:
		case EMSPECA_RESIST_VALUE:
		case EMSPECA_STATS_POW_VALUE:
		case EMSPECA_STATS_STR_VALUE:
		case EMSPECA_STATS_SPI_VALUE:
		case EMSPECA_STATS_DEX_VALUE:
		case EMSPECA_STATS_STA_VALUE:
		case EMSPECA_PA_VALUE:
		case EMSPECA_SA_VALUE:
		case EMSPECA_MA_VALUE:
		case EMSPECA_HP_POTION_INC_VALUE:
		case EMSPECA_MP_POTION_INC_VALUE:
		case EMSPECA_SP_POTION_INC_VALUE:
		//case EMSPECA_DAMAGE_RATE: -> EMSPECA_DAMAGE_RATE_ALL
		case EMSPECA_DEFENSE_RATE:
		case EMSPECA_HIT_RATE:
		case EMSPECA_AVOID_RATE:
		case EMSPECA_HP_VARIATION_RATE:
		case EMSPECA_MP_VARIATION_RATE:
		case EMSPECA_SP_VARIATION_RATE:
		case EMSPECA_HMS_VARIATION_RATE:
	//	case EMSPECA_MOVEVELO:
		case EMSPECA_MOVEVELO_VALUE:
		case EMSPECA_DAMAGE_RATE_ALL:
		case EMSPECA_CP_INC_VALUE:
		
		// 기존 스킬 효과들(테스트 완료2014.7.8, 옵션 사용시 주석해제 하면 적용)
		//case EMSPECA_MOVEVELO:
		//case EMSPECA_INVISIBLE:
		//case EMSPECA_RECVISIBLE:	
		//case EMSPECA_STUN:
		//case EMSPECA_NON_DRUG:
		//case EMSPECA_NON_SKILL:
		//case EMSPECA_TARRANGE_ALL:
		//case EMSPECA_APPLYRANGE_ALL:
		//case EMSPECA_ITEMDROP_RATE:
		//case EMSPECA_MONEYDROP_RATE:
		//case EMSPECA_EXP_RATE:
			return true;
	}
	return false;
}

//기획상으로 사용 불가능한 장비 타입들을 검사해서 사용 가능한 항목만 
bool GLItemSet::IsSearchableItem(EMSUIT INDEX)
{
	//코스튬 항목 검사

	switch( INDEX )
	{
		case SUIT_HEADGEAR:		
		case SUIT_UPPER:
		case SUIT_LOWER:
		case SUIT_HAND:
		case SUIT_FOOT:
		case SUIT_HANDHELD:
		case SUIT_NECK:
		case SUIT_WRIST:
		case SUIT_FINGER:
		//case SUIT_PET_A:
		//case SUIT_PET_B:
		//case SUIT_VEHICLE:
		//case SUIT_VEHICLE_SKIN:		
		//case SUIT_VEHICLE_PARTS_A:
		//case SUIT_VEHICLE_PARTS_B:	// 탈것 중 보드 파츠
		//case SUIT_VEHICLE_PARTS_C:
		//case SUIT_VEHICLE_PARTS_D:
		//case SUIT_VEHICLE_PARTS_E:
		//case SUIT_VEHICLE_PARTS_F:
		//case SUIT_VEHICLE_PARTS_SIZE:
		case SUIT_WAISTBAND:
		case SUIT_EARRING:
		case SUIT_ACCESSORIES:
		case SUIT_DECORATION:
		return true;
	}
	
	return false;
}

//1개 이상 착용 가능한 아이템 확인(반지, 악세서리 슬롯2개)
bool GLItemSet::CheckPairItem( const SITEM &sItem )
{
	return false;
}
