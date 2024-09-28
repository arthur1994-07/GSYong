#pragma once

#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../InnerInterface.h"
#include "../Util/UIGroupHelper.h"

class CCheckBox;
class CItemImage;
class CBasicTextBoxEx;
class CharacterView;

namespace
{
 
}
class CReferCharWearTab : public CUIGroupHelper
{
public:
    enum // ������ �ٲ�� �ȵ�
    {
        WEAR_HAT            = 0,// ����
        WEAR_TOP            = 1,// ����g
        WEAR_BELT           = 2,// ��Ʈ
        WEAR_PANTS          = 3,// ����
        WEAR_GLOVES         = 4,// �尩
        WEAR_SHOES          = 5,// �Ź�
        WEAR_WEAPON         = 6,// ����
        WEAR_DECORATION    = 7,// �Ͱ���1
        WEAR_EARRINGS    = 8,// �Ͱ���2
        WEAR_NECKLACE       = 9,// �����
        WEAR_BRACELET       = 10,//����
        WEAR_RING_A         = 11,//����1
        WEAR_RING_B         = 12,//����2
        WEAR_ACCESSORY_A    = 13,//��ű�1
        WEAR_ACCESSORY_B    = 14,//��ű�2

        WEAR_COUNT          // �հ�
    };

	enum
    {
        CART_MAX_COUNT = 10,
    };

private:

    enum
    {
        ID_CHECKBOX_ALLWEAR = NO_ID + 1,
        ID_CHECKBOX_PREVIEW,                // �� ĳ���Ϳ� ��������
        ID_CHECKBOX_COMPARE,                // �� �����۰� ���ϱ�
        ID_CHECKBOX_GRADE,                  // ����(��ȭ)��ġ ǥ���ϱ�
        ID_UICONTROL_CART_HELP,             // ��� ����(���� ǥ�ø� �ϴ°�?)
        ID_BUTTON_CART,                     // ���
        ID_BUTTON_WHISPER,                  // �ӼӸ�
        ID_BUTTON_ADDFRIEND,                // ģ�����
        ID_BUTTON_INVITEPARTY,              // ��Ƽ�ʴ�
        ID_BUTTON_REFRESH,                  // ���ΰ�ħ

        ID_ITEMIMG_INDEX    = 100,
        ID_ITEMIMG_STATR    = WEAR_HAT + ID_ITEMIMG_INDEX,
        ID_ITEMIMG_END      = ID_ITEMIMG_STATR + WEAR_COUNT,

        ID_CHECKBOX_INDEX   = 200,
        ID_CHECKBOX_START   = WEAR_HAT + ID_CHECKBOX_INDEX,
        ID_CHECKBOX_END     = ID_CHECKBOX_START + WEAR_COUNT,

        ID_CARTITEM_INDEX   = 300,
        ID_CARTITEM_START   = ID_CARTITEM_INDEX + ID_CARTITEM_INDEX,
        ID_CARTITEM_END     = ID_CARTITEM_START + CART_MAX_COUNT
    };

public:
    CReferCharWearTab( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );

    void                CreateSubControl();
    virtual void        TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
	virtual void		Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual void        Refresh();
    virtual void        SetVisibleSingle(BOOL isVisible);
	void				UpdateCharacterView();
    void                DeleteUIMeshData();

	GLCHARLOGIC*		GetAnotherChar() { return &m_cStatTarget; }


private:
    EMSLOT              WEAR2EMSLOT( int nIndex );
	void				SetCharacterData();
	void				CheckPutonMyCharacter( GLCHARLOGIC* pTarget );
	void				CheckPutonTargetCharacter( GLCHARLOGIC* pTarget );
    void                SetAnotherCharInformation();
    void                SetAnotherCharText( CBasicTextBoxEx* pTextBox, int wData );
    void                SetMyCharInformation();
    void                SetMyCharText( CBasicTextBoxEx* pTextBox, int wData );
    void                SetItemImage( WORD eWear, SITEM* pItem );
    void                SetGradeText( WORD eWear, const SITEMCUSTOM& sItem );
    BOOL                IsAllCheckBoxChecked();
    void                SetAllCheckBox( BOOL bCheck );
    void                SetAllGradeTextVisible( BOOL bVisible );
	void				InsertWishList( const SITEMCUSTOM& sItem );
	void				EraseWishList( UINT nIndex );
	void				MoveWishInventory();

private:
	GLGaeaClient*       m_pGaeaClient;
    CInnerInterface*    m_pInterface;

	CharacterView*		m_pCharacterView;

	GLCHARLOGIC*        m_pTarget;                      // ĳ���� ���� ����
	GLCHARLOGIC			m_cRenderTarget;				// ȭ�� ��������
	GLCHARLOGIC			m_cStatTarget;					// �ɷ�ġ �񱳿�

    CCheckBox*          m_pCheckBoxAllWear;
    CItemImage*         m_pArrayItemImage[WEAR_COUNT];  // �������� ������ �̹���
    CCheckBox*          m_pArrayCheckBox[WEAR_COUNT];
    CBasicTextBox*      m_pArrayGradeCount[WEAR_COUNT];
    CCheckBox*          m_pCheckBoxPreview;
    CCheckBox*          m_pCheckBoxCompare;
    CCheckBox*          m_pCheckBoxGrade; 
    
    CBasicTextBoxEx*    m_pAnotherCharBadge;
    CBasicTextBoxEx*    m_pAnotherCharLevel;
    CBasicTextBoxEx*    m_pAnotherCharStatus;
    CBasicTextBoxEx*    m_pAnotherCharAbility;
    CBasicTextBoxEx*    m_pAnotherCharResistance;

    CBasicTextBoxEx*    m_pMyCharLevel;
    CBasicTextBoxEx*    m_pMyCharStatus;
    CBasicTextBoxEx*    m_pMyCharAbility;
    CBasicTextBoxEx*    m_pMyCharResistance;

	SITEMCUSTOM			m_sCartItem[CART_MAX_COUNT];
    CItemImage*         m_pArrayCartImage[CART_MAX_COUNT];
};