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
    enum // 순서가 바뀌면 안됨
    {
        WEAR_HAT            = 0,// 모자
        WEAR_TOP            = 1,// 상의g
        WEAR_BELT           = 2,// 벨트
        WEAR_PANTS          = 3,// 하의
        WEAR_GLOVES         = 4,// 장갑
        WEAR_SHOES          = 5,// 신발
        WEAR_WEAPON         = 6,// 무기
        WEAR_DECORATION    = 7,// 귀걸이1
        WEAR_EARRINGS    = 8,// 귀걸이2
        WEAR_NECKLACE       = 9,// 목걸이
        WEAR_BRACELET       = 10,//팔찌
        WEAR_RING_A         = 11,//반지1
        WEAR_RING_B         = 12,//반지2
        WEAR_ACCESSORY_A    = 13,//장신구1
        WEAR_ACCESSORY_B    = 14,//장신구2

        WEAR_COUNT          // 합계
    };

	enum
    {
        CART_MAX_COUNT = 10,
    };

private:

    enum
    {
        ID_CHECKBOX_ALLWEAR = NO_ID + 1,
        ID_CHECKBOX_PREVIEW,                // 내 캐릭터에 입혀보기
        ID_CHECKBOX_COMPARE,                // 내 아이템과 비교하기
        ID_CHECKBOX_GRADE,                  // 연마(강화)수치 표시하기
        ID_UICONTROL_CART_HELP,             // 담기 도움말(툴팁 표시를 하는가?)
        ID_BUTTON_CART,                     // 담기
        ID_BUTTON_WHISPER,                  // 귓속말
        ID_BUTTON_ADDFRIEND,                // 친구등록
        ID_BUTTON_INVITEPARTY,              // 파티초대
        ID_BUTTON_REFRESH,                  // 새로고침

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

	GLCHARLOGIC*        m_pTarget;                      // 캐릭터 정보 원본
	GLCHARLOGIC			m_cRenderTarget;				// 화면 렌더링용
	GLCHARLOGIC			m_cStatTarget;					// 능력치 비교용

    CCheckBox*          m_pCheckBoxAllWear;
    CItemImage*         m_pArrayItemImage[WEAR_COUNT];  // 착용중인 아이템 이미지
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