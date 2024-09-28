#ifndef _GL_ITEM_SET_H_
#define _GL_ITEM_SET_H_

//#include "../RanLogic/Item/GLItem.h"
#include "../RanLogic/Skill/GLSkill.h"

struct SNATIVEID;
struct SSUM_ITEM;
struct SITEMCUSTOM;
struct SITEM;


//��Ʈ������ ����Ʈ
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

//��Ʈ �������� ȿ���� ����� ����ü 
typedef struct SSETITEMBONUS
{
	BYTE						byTerms;		//��Ʈ ȿ���� �ߵ��� ����(ex. ������ ����)	
	SKILL::SSPEC_ADDON_LEVEL	sSpec;			//��Ʈ ȿ���� ���, �ߵ�, ��ġ, Ȯ��

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

//��Ʈ ������ ID�� ������ ����ü (ex. �Ӷ�ī���� �г�)
struct SSETITEM
{
	WORD					dwSetID;		//��Ʈ ID			ex)�Ӷ�ī���� �г� (����! ������ ���� ��Ī�� �ƴ� ) 
	std::string				strName;		//��Ʈ ������ ��Ī  ex)�Ӷ����� �г�

	BYTE					byMaxNumBonus;	//�ִ� ���ʽ� ���� 
	BYTE					byMaxNumItem;	//�ִ� ������ ����
	BYTE					byMaxToolTipItemList;  //��Ʈ������ ������ �ִ�ġ

	VNATIVEID				vecItemList;	//���� ��Ʈ�� �����ִ� ������ ID ����Ʈ
	VECITEMBONUSE			vecBonus;		//��Ʈ �������� ȿ�� ���� ��ŭ 1�� �̻��� ����ü�� ������ �ȴ� ex)2��Ʈ ȿ��, 3��Ʈ ȿ��	   

	std::string				strSlotIndicationIcon;	// ��Ʈ�� ���� ǥ��;

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

//��Ʈ������ �˻��� ���� ���� ����ü
struct SSETITEMSEARCHINFO
{
	DWORD		dwSetID;	//�˻��� ��Ʈ ID
	VNATIVEID	vItemID;	//�˻��� ��Ʈ ������ ID

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

//��Ʈ ������ ����Ʈ�� �����ϴ� Ŭ����
class GLItemSet
{
public:
	static GLItemSet& GetInstance();

public:
	bool LoadFile(const std::string& strName, bool bToolMode);
	template <typename T>
	void LoadSetList(const std::string& strTitle, T Count); //����Ʈ ���� ��ü�� ����
	template <typename T>
	void LoadNum(const std::string& strTitle, T &pNum);//�ִ밹��, ��ƮID�� ������ ���
	void LoadSetName(const std::string& strTitle, std::string* pName); //����Ʈ���� �������� �Ⱦ�

	void LoadIDList(const std::string& strTitle, std::vector<SNATIVEID> &_Setitem, BYTE MaxIDNum); //��Ʈ�� ���� ������ID�б�
	void LoadBonusList(const std::string& strTitle, std::vector<SSIB> &_vecBonus, BYTE MaxBonusNum);//ȿ�� ����Ʈ �б�
	void LoadSlotIndicationIcon(const std::string& strTitle, std::string& strSlotIndication);

	void LoadSetItemInfo(DWORD Count);
	bool ParseSetItem();	
	bool LoadFile_Lua(const std::string& strPath);//��Ʈ������ ���� �˻�

private:
	DWORD						m_dwMaxSetList;	//����Ʈ ��Ͽ� ���� ����Ʈ�� �ִ� ��Ʈ ��� ����Ʈ
	char						m_CountCallLoad;

//by dyha
public:	
	//��Ʈ ȿ�� ���� ����
	void				SearchSetItem(VSSETITEMINFO &vResult, const SITEMCUSTOM *SlotItem, const UINT iArraySize, bool bUseSubWeapon = false );
	
	//Tool
	void				CheckErrorInfoSetItem(bool bSetID, bool bItemID);
	
	//ToolTip, ����
	bool				IsUsableOpt(EMSPEC_ADDON INDEX);	//��� ���� �ɼ� Ȯ��
	bool				IsSearchableItem(EMSUIT INDEX);		//��Ʈ������ ��� ���� ������ Ÿ��
	bool				IsPerentOpt(EMSPEC_ADDON INDEX);	//ǥ�� ���� Ÿ�� Ȯ��
	bool				CheckPairItem( const SITEM &sItem );	//1�� �̻� ���� ������ ������ Ȯ��(����, �Ǽ����� ����2��)

	//Get Set
	const bool			IsSetItem(const SNATIVEID &sNativeID) const;		//��Ʈ ������ �Ǵ�
	const SETITEMLIST*	GetInfoSetItemList(const DWORD	   &dwSetID) const;	//��Ʈ������ ����Ʈ ���� ������ ��ȯ
	const SSETITEM*		GetInfoSetItem(const SNATIVEID &sNativeID) const;	//�ش� ��Ʈ�������� ���� �� ��Ʈ ����ü ��ȯ
	const SSETITEM*		GetInfoSetItem(const DWORD	   &dwSetID) const;		//�ش� ��Ʈ�������� ���� �� ��Ʈ ����ü ��ȯ

	const std::vector<SITLIST>   GetSetItemList() const {return m_vecSetList;} 
	const std::vector<SSETITEM>  GetSetItemInfoList() const {return m_vecSetItem;} 

	void				GetItemBonusStr(std::string *strResult, const SSETITEM* sTargetSetItem, const UINT nTargetBonus);
	void				GetItemBonusStr(std::string *strResult, const DWORD &dwTargetSetItemID, const UINT nTargetBonus);

private:
	std::vector<SITLIST>		m_vecSetList;	//��Ʈ ��� ����
	std::vector<SSETITEM>		m_vecSetItem;	//��Ʈ ����

protected:
	GLItemSet(void);

public:
	~GLItemSet(void);

};

#endif
