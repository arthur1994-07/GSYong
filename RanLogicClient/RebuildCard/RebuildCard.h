#pragma once

#include "../Widget/GLWidgetScript.h"
#include "../../RanLogic/Item/GLItemCustom.h"

class RebuildCard
{
public:
	static RebuildCard& GetInstance(void);

//UI ��ɺ� ���۵Ǵ� �� ���� ���� �Լ�
public:
	void OpenRebuildUI ( EMTYPE_ITEM_REMODEL _emTypeItemRemodel );			// ����ī�� UI ����				

	void ReqRebuildMoveItem(const bool bMsg, const WORD wPreInvenItemPosX, const WORD wPreInvenItemPosY );	// ���Կ� ���� ��� �������� ��Ͻ� �޽��� �߻�(Scaleform) (Clinet -> Server)
	void ReqRebuildMoveStamp(void);				// ���Կ� ������ ��Ͻ� �޽��� �߻� (Scaleform)(Clinet -> Server)

	void ReceiveRebuildMoveItem( WORD wPosX, WORD wPosY );	// ������ ��� Ȯ�� �޽���	(Server -> Clinet)
	void ReceiveRebuildMoveStemp(void);						// ������ ��� Ȯ�� �޽���	(Server -> Clinet)

	void RemoveRebuildItem(void);				// ���Կ��� ������ ���Ž� �߻��Ǵ� �޽���(Scaleform)
	void RemoveRebuildStemp(void);				// ���Կ��� ������ ���Ž� �߻��Ǵ� �޽���(Scaleform)

	bool CheckItemRebuildWindowClose(const bool bUseStamp, const ITEM_RANDOMOP_UTILL::FixOption& fixOption, const EMTYPE_ITEM_REMODEL emTypeItemRemodel );
	void SetItemRebuildSetCheckBox(const SITEMCUSTOM& sItemCustom, const ITEM_RANDOMOP_UTILL::FixOption& fixOption);

//Lua ȣ�� �Լ�
public:
	const bool InitRebuild(void);				// ����ī�� ���� ��� ���� �ʱ�ȭ						

	void ReqSelectCache(void);					// ����� ���� & �ѹ�;
	void ReqChangeRebuildType(bool bKeepOrigin);
	void ReqRebuildItem(void);					// ����ī�� ���;
	void ReqRebuildItem_Fix(void);				// ���� Ȯ��;
	void ReqRebuildItem_Cancel(void);			// ���� ���;
	void ReqRebuildItem_DirectFix(void);		// ���� �ٷ� Ȯ��;
	void ReqCloseWindow(void);					//����ī��� ����Ǵ� �޽����� ���� ����
	
	bool SetFixedOption( const unsigned int nIndex );		// ������ �ɼ� �Է�
	void ResetFixedOption(void);							// ������ �ɼ� ����
	const int GetFixedOption ( const unsigned int _nIndex );
	const int GetNFixOption(void) const;						// ������ �ɼ� ����
	void UpdateFixedOption ( void );							// ������ �ɼ� ���� Update UI;

	//ToolTip ���
	const int GetOptionMaxSize(void) const;					//���� �ɼ� �ִ� ����
	const int GetRandomOLock(void) const;	//�ɼ� ���� ���� ����
	const LONGLONG GetRebuildCost(void) const;				//���� ���
	const LONGLONG GetRebuildInput(void) const;				//���� ���
	
	// ���� �ɼ�
	lua_tinker::table GetRandomOpt(void) const;
	// ���� �ɼ� ����;
	lua_tinker::table GetRebuild_RandomOpt(void) const;

	// �⺻ �ɷ�ġ;
	lua_tinker::table GetBasicStats(void) const;

	// ������ ��ų;
	lua_tinker::table GetItemSkill(void) const;
	
	void SetPreInventoryItem( WORD wPosX, WORD wPosY );	
	void GetPreInventoryItemPos( WORD& wOutPosX, WORD& wOutPosY );	
	void ResetPreInventoryItem(void);

	// ����ɼ��� ���� ��½� ��� Ÿ���� ���� �� �Ѱܼ� ������� ��� ���� �Ǵ��� �Ѵ�;
	// ���� ��½� Ÿ��ENUM�� ������������ ��µ����� �����Ϳ� �����ϴ� �ε����� ���� �ٸ� �� �ִ�;
	// �Ʒ��Լ��� �������� ���� �� ���� �ε����� �´� Ÿ�� �����ϴ� �Լ�;
	// �Ʒ� �Լ��� ��������������� �������� ��������;
	INT GetAddonOpFixType(INT nIndex);
	const int GetAddonFixTypeRealIndex ( const BYTE _nType );

public:
	inline const WORD GetFixedBasicStatIndex ( void ) const { return m_wFixedBasicStatIndex; }

private:
	//GLCharData.h
	/*BOOL				m_bRebuildOpen;				// ����â ���� ����
	SINVEN_POS			m_sRebuildItem;				// ����â ��ϵ� �κ��丮 ������
	SINVEN_POS			m_sStampItem;				// ����â ��ϵ� ���� ī��;
	SINVEN_POS			m_sPreInventoryItem;		// ������ �κ��丮���� ���ø� �ӽ� ������	
	RandomOpt::FixOption	m_fixOption;			// ���� ī��� ���� ��Ų ������ �ɼ�;
	LONGLONG			m_i64RebuildCost;			// ������ �ʿ��� �ݾ�
	LONGLONG			m_i64RebuildInput;			// ����â ����� �ݾ�

	bool				m_bTracingUser;				// ���� �������� �������� �ƴ���*/
	
	SINVEN_POS			m_sPreIventoryItem;
	SITEM*					m_pItemStamp;

	WORD				m_wFixedBasicStatIndex;

private:
	RebuildCard(void);
	RebuildCard( const RebuildCard& value );
	//~RebuildCard(void);
};
