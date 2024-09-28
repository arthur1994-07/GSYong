#pragma	once

#include "../Util/UIWindowEx.h"

class CInnerInterface;
class GLGaeaClient;
class CUIMeshRender;
class CUIMeshData_SkinChar;
class CItemImage;
class CSwapImage;
class GLCharacter;

class CItemPreviewWindow : public CUIWindowEx
{
protected:
    enum
    {
        ITEMPREVIEW_NO_ID = ET_CONTROL_NEXT,

        ITEMPREVIEW_MESHRENDER,

        ITEMPREVIEW_SLOT_HEAD,
        ITEMPREVIEW_SLOT_UPPER,
        ITEMPREVIEW_SLOT_LOWER,
        ITEMPREVIEW_SLOT_HAND,
        ITEMPREVIEW_SLOT_FOOT,
        ITEMPREVIEW_SLOT_WEAPON,

        ITEMPREVIEW_WINDOW_ARROW_TURN_LEFT,
        ITEMPREVIEW_WINDOW_ARROW_TURN_RIGHT,

        ITEMPREVIEW_SLOT_BEGIN = ITEMPREVIEW_SLOT_HEAD,
    };

protected:
  

    enum EMPREVIEWSLOT
    {
        EMPREVIEWSLOT_HEAD,
        EMPREVIEWSLOT_UPPER,
        EMPREVIEWSLOT_LOWER,
        EMPREVIEWSLOT_HAND,
        EMPREVIEWSLOT_FOOT,
        EMPREVIEWSLOT_WEAPON,

		EMPREVIEWSLOT_SIZE,

		// 기획팀에서 셋팅을 코스츔이 되도록 해버렸다. 지원해줘야한다. (EMPREVIEWSLOT_DECORATION,EMPREVIEWSLOT_EARRINGS)
		//EMPREVIEWSLOT_DECORATION,
		//EMPREVIEWSLOT_EARRINGS,
    };

public:
    struct SPIECEINFO
    {
        std::string szFILENAME;
        std::string szSELFBODYEFF;
        BYTE        cGRADE;
        DWORD       dwMAINCOLOR;
        DWORD       dwSUBCOLOR;
        BOOL        bNONCHARDATA;

        SPIECEINFO() 
            : cGRADE(0)
            , dwMAINCOLOR(0)
            , dwSUBCOLOR(0)
            , bNONCHARDATA(FALSE)
        {
        }

        void RESET()
        {
            szFILENAME.clear();
            szSELFBODYEFF.clear();
            cGRADE       = 0;
            dwMAINCOLOR  = 0;
            dwSUBCOLOR   = 0;
            bNONCHARDATA = FALSE;
        }
    };

public:
    static const float TURN_SPEED;
    static const float CAMERA_POSITION_Z;
    static const float CAMERA_POSITION_Z_MIN;
    static const float CAMERA_POSITION_Z_MAX;
    static const float CAMERA_POSITION_Y;

public:
    CItemPreviewWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
    virtual	~CItemPreviewWindow();

public:
    virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
    virtual	void SetVisibleSingle ( BOOL bVisible );

public:
    void	                CreateSubControl ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
    const BOOL              LoadObjectData ( GLCharacter* pCharacter );
    void                    SetItem        ( const SITEMCUSTOM& sItemCustom );
    void                    SetItem        ( const SITEMCUSTOM& sItemCustom, const EMSLOT emSlot );
    void                    ResetItem      ( const EMSLOT emSlot );
    void                    ResetAllItem   ();
	void					DeleteUIMeshData();

public:
    void                    TurnLeft       ( const float fAngle );
    void                    TurnRight      ( const float fAngle );

public:
    const BOOL              IsPreviewItem ( const SITEMCUSTOM& sItemCustom, const EMCHARINDEX emCharIndex );
    const BOOL              IsPreviewItem ( const SITEMCUSTOM& sItemCustom, const EMCHARINDEX emCharIndex, const EMSLOT emSlot );
    const EMSLOT            Item_2_Slot   ( const SITEMCUSTOM& sItemCustom );
    const EMSLOT            Item_2_Slot   ( const SITEM* pItem );

private:
    const EMPIECECHAR       SetPiece      ( const SPIECEINFO& sInfo );
    void                    ResetPiece    ( const EMPIECECHAR emPiece );

private:
    void                    InitGeometry      ();

private:
    const DWORD             ConvertColor       ( const WORD wColor );
    const EMPREVIEWSLOT     Slot_2_PreviewSlot ( const EMSLOT emSlot );

private:
    CBasicTextBox*	        CreateStaticControl (const char* szControlKeyword, CD3DFontPar* pFont, int nAlign, const UIGUID& cID = NO_ID );
    CItemImage*		        CreateItemImage ( const char* szControl, UIGUID ControlID );

private:
    CUIMeshRender*          m_pMeshRender;

    CSwapImage*             m_pImage_Arrow_Turn_Left;
    CSwapImage*             m_pImage_Arrow_Turn_Right;

    CItemImage*			    m_pItemImageArray  [EMPREVIEWSLOT_SIZE];	
    SITEMCUSTOM			    m_sItemCustomArray [EMPREVIEWSLOT_SIZE];

private:
    CUIMeshData_SkinChar*   m_pMeshData;
    D3DXMATRIX              m_matWorld;
    D3DXMATRIX              m_matProj;
    D3DXMATRIX              m_matView;

private:
    EMCHARINDEX             m_emCharIndex;
    EMANI_SUBTYPE           m_emAniSubType;
    FLOAT                   m_fCamDistance;
    FLOAT                   m_fTurnAngle;
    BOOL                    m_bTurnLeft;
    BOOL                    m_bTurnRight;

protected:
    GLGaeaClient*           m_pGaeaClient;

};
