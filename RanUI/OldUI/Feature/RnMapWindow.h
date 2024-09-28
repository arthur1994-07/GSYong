
#pragma	once


#include "../Util/UIWindowEx.h"
#include "../Util/TapSelectorEx.h"


#include "RnMapWindowInterface.h"
#include "RnMapWindowUniqueID.h"

#include "RnMapHelper.h"

#include "../../../EngineLib/GUInterface/SmartComboBoxRollOver.h"


class GLGaeaClient;
class CInnerInterface;
class EngineDeviceMan;

class CBasicVarTextBox;

class RnMapSettingTap;
class RnMapInfoTap;
class RnMapPartyTap;
class RnMapSearchTap;

class RnMapGuidanceUI;
class RnOldLargeMapWindow;
struct NET_MSG_GENERIC;

class RnMapWindow : public CUIWindowEx , private CTapSelectorEx<DEFAULT_RADIO_BUTTON> ,
	public RnMapWindowInterface, public RnMapPrivate::IMapInterface
{
public:
	RnMapWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);

	void CreateSubControl();

// 	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );

	//////////////////////////////////////////////////////////////////////////
	virtual void    VISIBLE_TARGET_POS( BOOL bVisible );
	virtual void	SetMapLevelFile(const GLLevelFileClient& levFile, const CString& strMapName );
    virtual void	MsgMapSearchNameList(const std::vector<std::pair<int, std::string> >& nameList);
	virtual void	MsgMapSearchNameDetailInfo(int level,const std::string& charName,const std::string& title,int school,int emClass,
		bool isParty,const std::string& clubName,DWORD dwGaeaID);
	virtual void	MsgMapSearchNamePosition(int level,const std::string& charName,DWORD mapID,const D3DXVECTOR3& pos);

	virtual void MsgMapPartyInfoListBoard(bool isExistMyBoard,DWORD mapID,
		const std::vector<std::string>& NameVec,const std::vector<DWORD>& MaxPlayerVec,
		const std::vector<DWORD>& CurrentPlayerVec,const std::vector<std::string>& TitleVec);

	virtual void MsgMapPartyInfoDetail(DWORD mapID,const std::vector<GLPARTY_LINK_MEM>& memberVec);

    virtual void MsgMapGuidanceAuthInfo(const NET_MSG_GENERIC* _pMessage);
	virtual void MsgMapCTIAuthInfo ( const NET_MSG_GENERIC* pMsg );

	virtual void	    SetMapLevelFile(DWORD mapID);
	virtual std::string GetMapName(DWORD mapID);
	virtual DWORD       GetSelectMap();



private:
	virtual void	SetMapLevelFile(const GLLevelFileClient& levFile );

	const GLLevelFileClient* GetMapLevelFile();

private:
	virtual HRESULT InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual void SetVisibleSingle(BOOL isVisible);

	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

	virtual  void EventSelectedTap( UIGUID ControlID );
	virtual void EventOverTap(UIGUID ControlID);



private:
	void MakeRadioButton(const std::string& radioKey,UIGUID radioID,const std::string& radioText,DWORD align,
		const std::string& buttonTextPosKey,DWORD color,
		const std::string& imagePosKey,const std::string& imageOnKey,const std::string& imageOffKey);


	CUIGroup* GetTapWindow( UIGUID controlID );
	

// 	void FloorMapTexturePos();
// 	void SetDefaultPosition();
// 	void Initialize();
private:
	GLGaeaClient* m_pGaeaClient;

	LPDIRECT3DDEVICEQ m_pd3dDevice;

	RnButton*		  m_pMapListBtn;
	DWORD			  m_pSelectMapID;

	RnMapSettingTap*    m_pSettingTap; 
	RnMapInfoTap*       m_pInfoTap;
	RnMapPartyTap*      m_pPartyTap;
	RnMapSearchTap*		m_pSearchTap;
	CUIGroup*			m_pCurrentTap;

    RnMapGuidanceUI*    m_pGuidanceUI;

	CSmartComboBoxRollOver* m_pMapListRollOver;



	RnOldLargeMapWindow* m_pOldLargeMapWindow; 
// 	CUIControl*			m_pMapImage;
// 
// 
// 	int			m_CurMousePosX;
// 	int			m_CurMousePosY;
// 
// 	bool		m_isMapImageDrag;
// 	int			m_MapImageDragPosX;
// 	int			m_MapImageDragPosY;
// 
// 	int			m_nMAP_TEXSIZE_X;
// 	int			m_nMAP_TEXSIZE_Y;
// 
// 	int			m_nVIEW_START_X;
// 	int			m_nVIEW_START_Y;
// 	int			m_nVIEW_SIZE_X;
// 	int			m_nVIEW_SIZE_Y;
// 
// 	D3DXVECTOR2			m_vMapTexturePos;
// 	D3DXVECTOR2			m_vMapTextureSize;
// 
// 	bool				m_bAUTO_MOVE;
};




