#ifndef _GL_ITEM_SET_H_
#define _GL_ITEM_SET_H_

//#include "../RanLogic/Item/GLItem.h"
#include "../RanLogic/Skill/GLSkill.h"

struct SNATIVEID;
struct SSUM_ITEM;
struct SITEMCUSTOM;
struct SITEM;


//셋트아이템 리스트
typedef struct SETITEMLIST
{
	DWORD		dwSetID;
	std::string strSetName;

	SETITEMLIST(){}
	SETITEMLIST( const SETITEMLIST &sSetItemList)
	{
		dwSetID		= sSetItemList.dwSetID;
		strSetName	= sSetItemList.strSetName; 
	}

}SITLIST;

//셋트 아이템의 효과가 저장된 구조체 
typedef struct SSETITEMBONUS
{
	BYTE						byTerms;		//셋트 효과가 발동될 조건(ex. 아이템 갯수)	
	SKILL::SSPEC_ADDON_LEVEL	sSpec;			//셋트 효과의 기능, 발동, 수치, 확률

	SSETITEMBONUS(){}
	SSETITEMBONUS( const SSETITEMBONUS& sSetItemBonus)
	{
		byTerms = sSetItemBonus.byTerms;
		sSpec = sSetItemBonus.sSpec;
	}
	SSETITEMBONUS( const BYTE _byTerms, const SKILL::SSPEC_ADDON_LEVEL &_sSPEC )
	{
		byTerms = _byTerms;
		sSpec = _sSPEC;
	}

}SSIB;
typedef std::vector<SNATIVEID>  VNATIVEID;
typedef std::vector<SSIB>		VECITEMBONUSE;

//셋트 아이템 ID당 생성될 구조체 (ex. 머라카노의 분노)
struct SSETITEM
{
	WORD					dwSetID;		//셋트 ID			ex)머라카노의 분노 (주의! 아이템 개별 명칭이 아님 ) 
	std::string				strName;		//셋트 아이템 명칭  ex)머라라노의 분노

	BYTE					byMaxNumBonus;	//최대 보너스 갯수 
	BYTE					byMaxNumItem;	//최대 아이템 갯수
	BYTE					byMaxToolTipItemList;  //세트아이템 구성의 최대치

	VNATIVEID				vecItemList;	//현재 셋트에 속해있는 아이템 ID 리스트
	VECITEMBONUSE			vecBonus;		//셋트 아이템의 효과 갯수 만큼 1개 이상의 구조체를 가지게 된다 ex)2셋트 효과, 3셋트 효과	   

	std::string				strSlotIndicationIcon;	// 셋트별 슬롯 표시;

	SSETITEM(){}
	SSETITEM( const SSETITEM &sSetItem )
	{
		dwSetID					= sSetItem.dwSetID;
		strName					= sSetItem.strName;

		byMaxNumBonus			= sSetItem.byMaxNumBonus;
		byMaxNumItem			= sSetItem.byMaxNumItem;
		byMaxToolTipItemList	= sSetItem.byMaxToolTipItemList;

		vecItemList				= sSetItem.vecItemList;
		vecBonus				= sSetItem.vecBonus;
		strSlotIndicationIcon		= sSetItem.strSlotIndicationIcon;
	}
	SSETITEM( const WORD _dwSetID, const std::string  _strName, const BYTE _byMaxNumBonus, const BYTE _byMaxNumItem, const BYTE _byMaxToolTipItemList,
		const VNATIVEID &_vecItemList, const VECITEMBONUSE &_vecBonus, const std::string _strSlotIndication = "")
	{
		dwSetID					= _dwSetID;
		strName					= _strName;

		byMaxNumBonus			= _byMaxNumBonus;
		byMaxNumItem			= _byMaxNumItem;
		byMaxToolTipItemList	= _byMaxToolTipItemList;

		vecItemList				= _vecItemList;
		vecBonus				= _vecBonus;
		strSlotIndicationIcon		= _strSlotIndication;
	}
};

//셋트아이템 검색에 사용될 변수 구조체
struct SSETITEMSEARCHINFO
{
	DWORD		dwSetID;	//검색된 셋트 ID
	VNATIVEID	vItemID;	//검색된 셋트 아이템 ID

	SSETITEMSEARCHINFO()
	:dwSetID(LONG_MAX)
	{}
	SSETITEMSEARCHINFO(const DWORD _dwSetID, const SNATIVEID &_vItemID)
	{
		dwSetID	  = _dwSetID;
		vItemID.push_back(_vItemID);
	}
};
typedef std::vector<SSETITEMSEARCHINFO> VSSETITEMINFO;

//셋트 아이템 리스트를 관리하는 클래스
class GLItemSet
{
public:
	static GLItemSet& GetInstance();

public:
	bool LoadFile(const std::string& strName, bool bToolMode);
	template <typename T>
	void LoadSetList(const std::string& strTitle, T Count); //리스트 내용 전체를 읽음
	template <typename T>
	void LoadNum(const std::string& strTitle, T &pNum);//최대갯수, 세트ID등 읽을때 사용
	void LoadSetName(const std::string& strTitle, std::string* pName); //리스트에서 읽을때는 안씀

	void LoadIDList(const std::string& strTitle, std::vector<SNATIVEID> &_Setitem, BYTE MaxIDNum); //세트에 속한 아이템ID읽기
	void LoadBonusList(const std::string& strTitle, std::vector<SSIB> &_vecBonus, BYTE MaxBonusNum);//효과 리스트 읽기
	void LoadSlotIndicationIcon(const std::string& strTitle, std::string& strSlotIndication);

	void LoadSetItemInfo(DWORD Count);
	bool ParseSetItem();	
	bool LoadFile_Lua(const std::string& strPath);//셋트아이템 여부 검사

private:
	DWORD						m_dwMaxSetList;	//리스트 목록에 적힌 리스트상 최대 셋트 목록 리스트
	char						m_CountCallLoad;

//by dyha
public:	
	//셋트 효과 적용 로직
	void				SearchSetItem(VSSETITEMINFO &vResult, const SITEMCUSTOM *SlotItem, const UINT iArraySize, bool bUseSubWeapon = false );
	
	//Tool
	void				CheckErrorInfoSetItem(bool bSetID, bool bItemID);
	
	//ToolTip, 예외
	bool				IsUsableOpt(EMSPEC_ADDON INDEX);	//사용 가능 옵션 확인
	bool				IsSearchableItem(EMSUIT INDEX);		//셋트아이템 사용 가능 아이템 타입
	bool				IsPerentOpt(EMSPEC_ADDON INDEX);	//표기 숫자 타입 확인
	bool				CheckPairItem( const SITEM &sItem );	//1개 이상 착용 가능한 아이템 확인(반지, 악세서리 슬롯2개)

	//Get Set
	const bool			IsSetItem(const SNATIVEID &sNativeID) const;		//셋트 아이템 판단
	const SETITEMLIST*	GetInfoSetItemList(const DWORD	   &dwSetID) const;	//셋트아이템 리스트 파일 구조제 반환
	const SSETITEM*		GetInfoSetItem(const SNATIVEID &sNativeID) const;	//해당 셋트아이템이 포함 된 셋트 구조체 반환
	const SSETITEM*		GetInfoSetItem(const DWORD	   &dwSetID) const;		//해당 셋트아이템이 포함 된 셋트 구조체 반환

	const std::vector<SITLIST>   GetSetItemList() const {return m_vecSetList;} 
	const std::vector<SSETITEM>  GetSetItemInfoList() const {return m_vecSetItem;} 

	void				GetItemBonusStr(std::string *strResult, const SSETITEM* sTargetSetItem, const UINT nTargetBonus);
	void				GetItemBonusStr(std::string *strResult, const DWORD &dwTargetSetItemID, const UINT nTargetBonus);

private:
	std::vector<SITLIST>		m_vecSetList;	//셋트 목록 정보
	std::vector<SSETITEM>		m_vecSetItem;	//셋트 정보

protected:
	GLItemSet(void);

public:
	~GLItemSet(void);

};

#endif
