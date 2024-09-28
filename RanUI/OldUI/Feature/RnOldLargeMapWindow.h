
#pragma	once

#include "../Util/UIGroupHelper.h"
#include "../../../RanLogicClient/Crow/GLCrowRenList.h"

#include "RnMapWindowUniqueID.h"

#include "RnMapWindowInterface.h"

#include "../../../RanLogicClient/Level/GLLevelFileClient.h"

#include "RnMapHelper.h"

#include "RnMapWindowInterface.h"

#include "../../../RanLogicClient/Level/GLLevelFileClient.h"

#include "RnMapHelper.h"

#include "../../../enginelib/GUInterface/BasicTextBox.h"

//#include <boost/pool/pool.hpp>

class CInnerInterface;
class CBasicScrollBarEx;
class GLMapAxisInfo;
class CBasicTextBox;
class GLGaeaClient;
struct GLPartySlotClient;
class EngineDeviceMan;

namespace RnMapPrivate
{
	class ISettingInterface;
	class IMapInterface;

	class RegionName;

}


class RnOldLargeMapWindow : public CUIGroupHelper 
{
public:

	struct Mark : public CUIControl
	{
	public:

// 		static void* operator new(std::size_t size)
// 		{
// 			GASSERT(s_pool.get_requested_size() == size);
// 			return s_pool.ordered_malloc();
// 		}
// 
// 		static void operator delete(void* pMem,std::size_t size)
// 		{
// 			GASSERT(s_pool.get_requested_size() == size);
// 			s_pool.ordered_free(pMem);
// 		}

		struct Comment
		{
			Comment(const std::string textComment ="",DWORD textColor = D3DCOLOR_XRGB(255,255,255)) 
			{
				text = textComment;
				color = textColor;
			}

			std::string text;
			DWORD       color;
		};

		Mark(EngineDeviceMan* pEngineDevice) : CUIControl(pEngineDevice) 
		{
			m_canTrade = NO_VISIBLE_TRADE;
			m_Position = D3DXVECTOR3(0,0,0);
		}
		
		enum TRADE { NO_VISIBLE_TRADE,NO_TRADE,CAN_TRADE};
		TRADE					 m_canTrade;
		std::string				 m_Name;
		std::vector<Comment>	 m_CommentVec;
		D3DXVECTOR3				 m_Position;
		SNATIVEID				 m_MapID;

		int						 m_iAddData1;

	private:
// 		static boost::pool<> s_pool;
	};

	typedef std::vector<Mark*> MarkVector;



	//private:
	//	typedef	std::map<DWORD,CUIControl*>		PARTY_ONMAP_MAP;
	//	typedef PARTY_ONMAP_MAP::iterator		PARTY_ONMAP_MAP_ITER;

	typedef std::vector<CUIControl*>		    GUILD_POS_VECTOR;
	typedef	GUILD_POS_VECTOR::iterator		    GUILD_POS_VECTOR_ITER;

	typedef std::vector<CUIControl*>		    CTF_POS_VECTOR;
	typedef	CTF_POS_VECTOR::iterator		    CTF_POS_VECTOR_ITER;

	typedef std::vector<CUIControl*>		    CTF_AUTHENTICATOR_VECTOR;
	typedef	CTF_AUTHENTICATOR_VECTOR::iterator	CTF_AUTHENTICATOR_VECTOR_ITER;

public:
	RnOldLargeMapWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice,
		RnMapPrivate::ISettingInterface* pSettingInterface,RnMapPrivate::IMapInterface* pChangeMap,
		RnMapPrivate::IMapInfoInterface* pInfoInterface,RnMapPrivate::IMapSearchInterface* pSearchInterface);

	virtual	~RnOldLargeMapWindow();

private:
	GLGaeaClient*		m_pGaeaClient;
	CInnerInterface*	m_pInterface;
	LPDIRECT3DDEVICEQ	m_pd3dDevice;

	RnMapPrivate::ISettingInterface* m_pSettingInterface;
	RnMapPrivate::IMapInterface*	 m_pChangeMap;
	RnMapPrivate::IMapInfoInterface* m_pInfoInterface;
	RnMapPrivate::IMapSearchInterface* m_pSearchInterface;

	GLLevelFileClient*	m_pLevelFile;
	bool				m_isEnableLButton;


	float				m_fPercentH_BACK;
	float				m_fPercentV_BACK;

	D3DXVECTOR2			m_vScrollGap;

	D3DXVECTOR2			m_vRenderSize;
	D3DXVECTOR2			m_vRenderPos;

private:
	int					m_nWORLD_START_X;
	int					m_nWORLD_START_Y;
	int					m_nWORLD_SIZE_X;
	int					m_nWORLD_SIZE_Y;

	int					m_nMAP_TEXSIZE_X;
	int					m_nMAP_TEXSIZE_Y;

	int					m_nVIEW_START_X;
	int					m_nVIEW_START_Y;
	int					m_nVIEW_SIZE_X;
	int					m_nVIEW_SIZE_Y;

	D3DXVECTOR2			m_vWORLD_START;
	D3DXVECTOR2			m_vWORLD_SIZE;
	D3DXVECTOR2			m_vMAP_TEXSIZE;

private:
	BOOL				m_bFirstGap;
	D3DXVECTOR2			m_vGap;

	int					m_PosX;
	int					m_PosY;

	int					m_visiblePosX;
	int					m_visiblePosY;

	D3DXVECTOR2			m_vMOUSE_BACK;

private:
	CBasicScrollBarEx*	m_pScrollBarH;
	CBasicScrollBarEx*	m_pScrollBarV;

	CUIControl*			m_pMap;	
	CUIControl*			m_pPlayerMark;
	CUIControl*			m_pPlayerDirection;

	CUIControl*			m_pMousePosBack;
	CBasicTextBox*		m_pMousePosText;

	CBasicButton*		m_pLBThumb;
	CBasicButton*		m_pRBThumb;


	

private:
	//PARTY_ONMAP_MAP	m_mapPARTY;
	//UIGUID			m_CONTROL_NEWID;	
	GUILD_POS_VECTOR        	m_vecGuild;
	CTF_POS_VECTOR	            m_vecCTF;
	CTF_AUTHENTICATOR_VECTOR    m_vecCTFAuthenticator;
	bool				        m_bAUTO_MOVE;




	MarkVector    m_GateMarkVec;
	MarkVector    m_NormalNpcMarkVec;

    std::vector<RnMapPrivate::RegionName*>    m_ZoneNameVec;    // 지역 이름
    std::vector<RnMapPrivate::RegionName*>    m_GateNameVec;    // 게이트 이름

	MarkVector    m_BusStationMarkVec;
	MarkVector    m_LockerManagerMarkVec;
	MarkVector    m_StartPositionMarkVec;
	MarkVector    m_ClubManagerMarkVec;
// 	MarkVector    m_ItemSearchMarkVec;
// 	MarkVector    m_ItemMixMarkVec;
	MarkVector    m_PostBoxMarkVec;


	MarkVector    m_PartyMarkVec;

	MarkVector    m_MarkTotalVec;

	Mark*		  m_pSearchCharacter;	


	typedef std::vector<GLMobSchedule*> MobScheduleVec;
	std::map<DWORD,MobScheduleVec> m_MobScheduleVecMap;


	std::map<DWORD,MarkVector>    m_MonsterMarkMap;
private:
	CUIControl*			m_pTargetMark;
	int					m_iTargetX;
	int					m_iTargetY;

public:
	void	Initialize();
	void	ConvIntDataFloatData();
	void	CreateSubControl();
	void	UPDATE_CHARDIR();

private:
	void	SetAutoMove( bool bAutoMove )	{ m_bAUTO_MOVE = bAutoMove; }
	void	TranslateMeg( UIGUID cID, DWORD dwMsg, CUIControl * pUIControl );
	void	TranslateMegThumb( UIGUID cID, DWORD dwMsg, CUIControl * pUIControl );

	
	bool    IsMyCharacterMap();

public:
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual	void	Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

	virtual	void	TranslateUIMessage ( UIGUID cID, DWORD dwMsg );
	virtual	HRESULT	Render ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual	void	SetVisibleSingle ( BOOL bVisible );

	void ShowTooltip(DWORD monsterID);
public:
	void	UpdateWithManualFor_NoID_UI( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

	void	Update(BOOL bFirstControl);
	

	void	SetMapLevelFile (const GLLevelFileClient& levFile, const CString& strMapName );
	const   GLLevelFileClient* GetMapLevelFile() { return m_pLevelFile; }


	void	GetMobSchVec(std::vector<GLMobSchedule*>* pCrowVec);
	void	GetMonster(std::vector<DWORD>* pVec);


	void	OnLButtonUp();
	void    EnableLButton(bool isEnable) { m_isEnableLButton = isEnable; }

	void	UPDATE_TARGET_POS( int iX, int iY );
	void	VISIBLE_TARGET_POS( BOOL bVisible );

	void	SetDefaultPosition();

private:

	void	MakeScrollGap();
	const D3DXVECTOR2 GetScrollGap();    

	void	UPDATE_CHAR_POS ( CUIControl * pControl, const D3DXVECTOR3& vPlayerPos,const SNATIVEID& id);
	void	UPDATE_CHAR_DIRECTION ( CUIControl* pControl, const D3DXVECTOR3& vPlayerPos, const D3DXVECTOR3& vDirection, const SNATIVEID& id );
	void	UPDATE_CHAR_ROTATE ( const D3DXVECTOR3& vCharDir );

	void	UPDATE_MOUSE_POS ( const int nMouseX, const int nMouseY );

	void	UPDATE_PARTY();
	void	UPDATE_PARTY_DEL();
	void	UPDATE_PARTY_INS(GLPartySlotClient* const pSelfClient);

	void	UPDATE_GUILD();
	void	RESET_GUILD();
	bool	IS_GUILD_UPDATE();
	void	UPDATE_GUILD_INFO( bool bVisible );

	void    UPDATE_CTF();
	void	RESET_CTF();
	bool	IS_CTF_UPDATE();

	//void	AddPartyMemberOnMAP ( const DWORD& dwGaeaID, const D3DXVECTOR3& vPos, const int nPartyID );

    void CreateZoneName(const std::vector<GLMobSchedule*>& crowVec, std::vector<RnMapPrivate::RegionName*>& MarkVec);
    void CreateGateName(const std::vector<GLMobSchedule*>& crowVec, std::vector<RnMapPrivate::RegionName*>& GateNameVec);
	void CreateMarkHelper(const std::string& ImageKey,std::vector<GLMobSchedule*>& crowVec,
		MarkVector& MarkVec);
	//////////////////////////////////////////////////////////////////////////
	void MakeGate();
	void MakeNormalNpc();
    void MakeZoneNameNpc();     // 지역 이름 표시용 NPC
    void MakeGateNameNpc();     // 게이트 이름 표시용 NPC
	void MakeBusStation();
	void MakeLookerRoom();
	void MakeStartPosition();
	void MakeClubManager();
// 	void MakeItemSearch();
// 	void MakeItemMix();	
	void MakePostBox();
	void MakePartyMember();

	//////////////////////////////////////////////////////////////////////////
	void MakeSearchCharacter();

	//////////////////////////////////////////////////////////////////////////
	
	void UpdateGate();
	void UpdateNormalNpc();
	void UpdateZoneName();
    void UpdateGateName();
	void UpdateBusStation();
	void UpdateLookerRoom();
	void UpdateStartPosition();
	void UpdateClubManager();
// 	void UpdateItemSearch();
// 	void UpdateItemMix();	
	void UpdatePostBox();
	void UpdatePartyMember();
	//////////////////////////////////////////////////////////////////////////
	void UpdateSearchCharacter();

	//////////////////////////////////////////////////////////////////////////
	
	void MakeMonsterInfo();

	void MakeNpcInfo();
	void UpdateMonsterInfo();

	//////////////////////////////////////////////////////////////////////////
//	virtual void GetMonster(int startIndex,int endIndex,std::vector<DWORD>* pVec);


	//////////////////////////////////////////////////////////////////////////


private:
	GLPartySlotClient* const FindSelfClient(void);
	BOOL	IsSameMap ( GLPartySlotClient *pSelf, GLPartySlotClient *pMember );

private:
	//CUIControl*	MAKE_MARK ( const int nPartyID );

	void	CONVERT_WORLD2MAP ( const D3DXVECTOR3& vPos, int& nPosX, int& nPosY );
	void	CONVERT_MOUSE2MAP ( const int nMousePosX, const int nMousePosY, int& nPosX, int& nPosY );
	void	CONVERT_MAP2WORLD ( const int nPosX, const int nPosY, D3DXVECTOR3& vPos );
	void	DoCHARPOS_OnCENTER_InVIEW ( int nMapPosX, int nMapPosY );

public:
    const bool Convert_WordlPos2MapRenderPos ( const D3DXVECTOR3& vPos, int& nPosX, int& nPosY, const int nRederHalfSizeX, const int nRederHalfSizeY );
};

