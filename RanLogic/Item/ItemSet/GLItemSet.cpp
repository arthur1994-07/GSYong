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
 	std::string strPath(GLOGIC::GetPath());  //GLOGIC����� ���ԵǾ����

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
	//Tool������ lua������, ���ӿ����� luc������ ����
	return LoadFile_Lua(strPath);
}

void GLItemSet::LoadSetName(const std::string& strTitle, std::string* pName)
{
	std::wstring ws;
	try
	{	
		LuaPlus::LuaObject ob = sc::lua_getGlobalFromName(strTitle.c_str());
		LuaPlus::LuaTableIterator it(ob);	//���̺� �������� ��ũ��Ʈ�� �迭�� �����Ǿ� �־���Ѵ�
		ws = reinterpret_cast<const wchar_t*>(it.GetValue().GetWString());
		*pName = sc::string::unicodeToAnsi(ws);		
	}//��ũ��Ʈ���� Ÿ��Ʋ�� ��ġ�ϴ� �κ��� ã�Ƽ� 1���� �־���(�� �ٿ� 2���̻��� ��) 
	//���ڿ� ���� ���� ��� �Ұ����� �Լ� ������ �ʿ�
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
}//��Ʈ����Ʈ ��ũ��Ʈ �ε�

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
}//��Ʈ ���� ������ ID�ε�

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
}//ȿ�� ������ �ε�

void GLItemSet::LoadSlotIndicationIcon(const std::string& strTitle, std::string& strSlotIndication)
{
	std::wstring ws;
	try
	{	
		LuaPlus::LuaObject ob = sc::lua_getGlobalFromName(strTitle.c_str());
		LuaPlus::LuaTableIterator it(ob);	//���̺� �������� ��ũ��Ʈ�� �迭�� �����Ǿ� �־���Ѵ�
		ws = reinterpret_cast<const wchar_t*>(it.GetValue().GetWString());
		strSlotIndication = sc::string::unicodeToAnsi(ws);		
	}//��ũ��Ʈ���� Ÿ��Ʋ�� ��ġ�ϴ� �κ��� ã�Ƽ� 1���� �־���(�� �ٿ� 2���̻��� ��) 
	//���ڿ� ���� ���� ��� �Ұ����� �Լ� ������ �ʿ�
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
		//������ ����Ʈ ���� ƫ�� ������ Ŭ�� ���� dyha
		//Temp.byMaxToolTipItemList > Temp.byMaxNumItem? Temp.byMaxNumItem:Temp.byMaxToolTipItemList;
		LoadNum(_T("MaxPieceState"), Temp.byMaxNumBonus);
		LoadIDList(_T("Set_Item_List"), Temp.vecItemList, Temp.byMaxNumItem);
		LoadBonusList(_T("Set_Piece_State"), Temp.vecBonus, Temp.byMaxNumBonus);
		LoadSlotIndicationIcon(_T("SetSlot_IndicationIcon"), Temp.strSlotIndicationIcon );
		
		//���� �ε��� ����ó�� �� �κ�
		m_vecSetItem.push_back(Temp);
}//������ ���� ��ũ��Ʈ ��ü �ε�

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
}//����Ʈ ��ũ��Ʈ, ��Ʈ������ �������� ��ũ��Ʈ ��� �ε�

bool GLItemSet::LoadFile_Lua(const std::string& strPath)
{
	if(strPath.empty()) return false;

	char szName[_MAX_FNAME] = _T("");
	char szExt[_MAX_EXT] = _T("");
	_splitpath(strPath.c_str(), NULL, NULL, szName, szExt);//��θ� ���ؼ� �̸��� Ȯ���ڸ� �˾Ƴ�
	StringCchCat(szName, _MAX_FNAME, szExt); //�̸��ڿ� Ȯ���� �ٿ���

	std::auto_ptr< sc::BaseStream > pStream(
		GLOGIC::openfile_basestream(
		GLOGIC::UseLogicZipFile(),
		GLogicData::GetInstance().GetGlogicZipFile(),
		strPath.c_str(), 
		szName, 
		false,
		GLOGIC::UseLogicPackFile() ) );

	//���� ��θ� ���󰡼� ������ ����(zip�������� �ƴ����� �����ؼ�, �ʱ�ȭ)
	if(!pStream.get())
	{
		sc::writeLogError(sc::string::format("GLSetItem::LoadFile_Lua, File Open %1%", strPath));
			return false;
	}

	size_t nSize = static_cast<size_t> (pStream->GetFileSize());
	void* pBuffer = malloc(nSize);
	pStream->ReadBuffer(pBuffer, 0);	
	CTypeSmartPtr cPtrData(pBuffer); //smartptr�� ������ ����

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
	//�˻� �׽�Ʈ 1
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
			//���� ����
			if(!bStartStr) 
			{
				*strResult += ", ";
				
			}

			bStartStr = false;

			//��Ʈȿ�� ���ʽ�
			if ( IsPerentOpt( sTargetSetItem->vecBonus[i].sSpec.emSPEC ) )
			{
				if( sTargetSetItem->vecBonus[i].sSpec.sSPEC.fVAR1 >= 0)//���� ��� ����
					*strResult += sc::string::format( "%s +%.2f", ID2GAMEWORD("ITEM_INFO_SET_BONUS", sTargetSetItem->vecBonus[i].sSpec.emSPEC), sTargetSetItem->vecBonus[i].sSpec.sSPEC.fVAR1 );
				else
					*strResult += sc::string::format( "%s %.2f", ID2GAMEWORD("ITEM_INFO_SET_BONUS", sTargetSetItem->vecBonus[i].sSpec.emSPEC), sTargetSetItem->vecBonus[i].sSpec.sSPEC.fVAR1 );
				
				*strResult += _T("%");
			}
			else
			{
				if( sTargetSetItem->vecBonus[i].sSpec.sSPEC.fVAR1 >= 0)//���� ��� ����
					*strResult += sc::string::format( "%s +%d", ID2GAMEWORD("ITEM_INFO_SET_BONUS", sTargetSetItem->vecBonus[i].sSpec.emSPEC), sTargetSetItem->vecBonus[i].sSpec.sSPEC.fVAR1 );
				else
					*strResult += sc::string::format( "%s %d", ID2GAMEWORD("ITEM_INFO_SET_BONUS", sTargetSetItem->vecBonus[i].sSpec.emSPEC), sTargetSetItem->vecBonus[i].sSpec.sSPEC.fVAR1 );
			}
		}
	}
}

/*��ũ��Ʈ�� �ҷ��� ���������� �Է¹��� �����۰� ���� �������� �ִ��� Ȯ�� �� ��Ʈ�������� ��쿡�� VSSETITEMINFO�� ����
VSSETITEMINFO &vSearchResult : ���� �������� ��Ʈ �������� �׸� �� ��Ƽ� ��ȯ�Ѵ�
const SITEMCUSTOM *sSlotItem : ���� ���Ծ����� (����! 1���� �ƴ� iArraySize ��ŭ Ȯ���Ѵ�)
UINT  iArraySize			 : SITEMCUSTOM ���·� �Է¹��� �������� ũ��
*/
void GLItemSet::SearchSetItem(VSSETITEMINFO &vResult, const SITEMCUSTOM *SlotItem, const UINT iArraySize, bool bUseSubWeapon )
{
	if( NULL == SlotItem ) return;
	
	vResult.clear();	//���� �ʴ� ���� �ߺ� ������ �ʱ�ȭ �ʼ���

	for ( UINT i = 0; i< iArraySize; ++i )
	{	
		//1.��Ʈ ��Ͽ� ���翩�� Ȯ��
		const SSETITEM *sSetItem = GLItemSet::GetInstance().GetInfoSetItem(SlotItem[i].GetNativeID());
		if(NULL == sSetItem) continue;

		//2.����ó��
		//2-1. �ڽ�Ƭ �������� �˻� �Ұ���
		//const SITEM* pDisguise = GLogicData::GetInstance().GetItem( SlotItem[i].nidDISGUISE );
		//if( NULL != pDisguise ) continue;
		
		//2-2. ��Ʈ���������� ��� ������ �׸����� Ȯ�� ex)�尩 �Ź� ���
		const SITEM *sItemTarget = GLogicData::GetInstance().GetItem( SlotItem[i].GetNativeID() );
		if(NULL == sItemTarget) continue;

		if( !IsSearchableItem(sItemTarget->sSuitOp.emSuit) ) continue;	

		EMSLOT eSlot = static_cast<EMSLOT>(i);
		
		//2-3. ������ Ȯ�� : 0�̸� �˻����� ����
		if ( GLUseFeatures::GetInstance().IsUsingItemDurability() )
		{
			float fDurabilityRate( 1.0f );   // �⺻������ 100%

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
				
		//3. ��Ʈ ���������� �Ǻ����� �˻� ��Ͽ� �־���
		if(i)
		{
			bool bNewSetID(false); // ��Ʈ ���̵� Ȯ�ο�
			
			for(WORD j = 0; j < vResult.size(); ++j)
			{
				if( sSetItem->dwSetID == vResult[j].dwSetID )// �ѹ� �˻� �� ��Ʈ�� ���� Ƚ���� �߰�
				{
					//3-1. ������ ��Ʈ ��� �� �ߺ� ���̵� �Ǵ�
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

			if(!bNewSetID)	//�˻� �� ���� ���� ��Ʈ �������̴� ���ο� ����Ʈ �߰�
			{
				vResult.push_back(SSETITEMSEARCHINFO(sSetItem->dwSetID, SlotItem[i].GetNativeID()));
			}
		}
		else//ó�� �ƹ��͵� �˻��� �ȵ� ���� i = 0
		{
			vResult.push_back(SSETITEMSEARCHINFO(sSetItem->dwSetID, SlotItem[i].GetNativeID()));
		}
	}
}

//ƫ�� ����� ���� ������ �Ǽ� �ɼ� ���п� ��ũ��Ʈ�� �Ҽ������� �߸� ǥ�⵵ �����ϱ� ������ �ڵ�󿡼� ������ ���ش�
bool GLItemSet::IsPerentOpt(EMSPEC_ADDON INDEX)
{
	switch(INDEX)
	{	
		//�Ǽ�ǥ�Ⱑ �ƴ� �κ��� �߰� �� �ʿ䰡 ����
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

//��Ʈ ���������� ��� �Ұ����� �ɼ��� �������� �ʱ� ���� ��� �� �ɼǸ� ���⿡ �߰� ���ش� 
bool GLItemSet::IsUsableOpt(EMSPEC_ADDON INDEX)
{
	switch(INDEX)
	{
		// ���� ���� �ɼ� ���
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
		
		// ���� ��ų ȿ����(�׽�Ʈ �Ϸ�2014.7.8, �ɼ� ���� �ּ����� �ϸ� ����)
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

//��ȹ������ ��� �Ұ����� ��� Ÿ�Ե��� �˻��ؼ� ��� ������ �׸� 
bool GLItemSet::IsSearchableItem(EMSUIT INDEX)
{
	//�ڽ�Ƭ �׸� �˻�

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
		//case SUIT_VEHICLE_PARTS_B:	// Ż�� �� ���� ����
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

//1�� �̻� ���� ������ ������ Ȯ��(����, �Ǽ����� ����2��)
bool GLItemSet::CheckPairItem( const SITEM &sItem )
{
	return false;
}
