#ifndef GLLANDMANCLIENT_H_
#define GLLANDMANCLIENT_H_

#pragma once

#include <boost/tr1/unordered_map.hpp>
#include <string>
#include <map>

#include "../../EngineLib/G-Logic/GLDefine.h"
#include "../../EngineLib/DxLand/DxLandMan.h"

#include "../../RanLogic/GLLevelFile.h"
#include "../../RanLogic/Item/GLMoney.h"
#include "../../RanLogic/TriggerSystem/TriggerSystemType.h"
#include "../../RanLogic/PVE/PVEInDunID.h"

#include "../Char/GLCharClient.h"
#include "../Crow/GLCrowClient.h"
#include "../Material/GLMaterialClient.h"

#include "./Faction/FactionManagerClient.h"
#include "../Pet/GLAnyPet.h"
#include "../Summon/GLSummonClient.h"
#include "../LandEvent/GLLandEventClient.h"
#include "../Level/GLLevelFileClient.h"

class GLLandEventClient;
class CEVENT_NODE_CLIENT;
class GLCharacter;

class GLClientNode
{
public:
    GLClientNode() {}
    virtual ~GLClientNode() {}

public:	
	GLCHARCLIENTLIST		m_PCList;			//	PC 리스트.
	
	// PET
	//GLANYPETLIST			m_PETList;			//  PET 리스트.	
};
typedef SQUADNODE<GLClientNode>		CLIENTQUADNODE;
typedef GLQuadTree<GLClientNode>	CLIENTQUADTREE;

class GLLandManClient : public GLLevelFileClient
{
public:
    typedef std::tr1::unordered_map<std::string, std::tr1::shared_ptr<GLCharClient> > CHARMAP;
	typedef CHARMAP::iterator						            CHARMAP_ITER;
    typedef CHARMAP::const_iterator						        CHARMAP_CITER;
    typedef CHARMAP::value_type						            CHARMAP_VALUE;

    typedef std::map<DWORD, std::tr1::shared_ptr<GLCharClient> > CHARMAPID;
	typedef CHARMAPID::iterator				                  CHARMAPID_ITER;
    typedef CHARMAPID::const_iterator		                  CHARMAPID_CITER;
    typedef CHARMAPID::value_type                             CHARMAPID_VALUE;

	typedef std::map<DWORD, std::tr1::shared_ptr<GLAnyPet> > PETMAPID;
	typedef PETMAPID::iterator				              PETMAPID_ITER;
    typedef PETMAPID::const_iterator		              PETMAPID_CITER;
    typedef PETMAPID::value_type     		              PETMAPID_VALUE;
	
    typedef std::map<DWORD, std::tr1::shared_ptr<CItemClientDrop> > DROP_ITEM_MAP;
    typedef DROP_ITEM_MAP::iterator                              DROP_ITEM_ITER;
    typedef DROP_ITEM_MAP::const_iterator                        DROP_ITEM_CITER;
    typedef DROP_ITEM_MAP::value_type                            DROP_ITEM_VALUE;

    typedef std::map<DWORD, std::tr1::shared_ptr<CMoneyClientDrop> > DROP_MONEY_MAP;
    typedef DROP_MONEY_MAP::iterator                              DROP_MONEY_MAP_ITER;
    typedef DROP_MONEY_MAP::const_iterator                        DROP_MONEY_MAP_CITER;
    typedef DROP_MONEY_MAP::value_type                            DROP_MONEY_MAP_VALUE;

    typedef std::map<DWORD, std::tr1::shared_ptr<GLMaterialClient> > DROP_MATERIAL_MAP;
    typedef DROP_MATERIAL_MAP::iterator                           DROP_MATERIAL_MAP_ITER;
    typedef DROP_MATERIAL_MAP::const_iterator                     DROP_MATERIAL_MAP_CITER;
    typedef DROP_MATERIAL_MAP::value_type                         DROP_MATERIAL_MAP_VALUE;

    typedef std::map<DWORD, std::tr1::shared_ptr<GLCrowClient> > DROP_CROW_MAP;
    typedef DROP_CROW_MAP::iterator                           DROP_CROW_MAP_ITER;
    typedef DROP_CROW_MAP::const_iterator                     DROP_CROW_MAP_CITER;
    typedef DROP_CROW_MAP::value_type                         DROP_CROW_MAP_VALUE;

    typedef std::map<DWORD, std::tr1::shared_ptr<GLSummonClient> > DROP_SUMMON_MAP;
    typedef DROP_SUMMON_MAP::iterator                           DROP_SUMMON_MAP_ITER;
    typedef DROP_SUMMON_MAP::const_iterator                     DROP_SUMMON_MAP_CITER;
    typedef DROP_SUMMON_MAP::value_type                         DROP_SUMMON_MAP_VALUE;

public:
    GLLandManClient(GLGaeaClient* pGaeaClient);
    virtual ~GLLandManClient(void);
    
protected:
    //GLGaeaClient* m_pGaeaClient;

public:
	//BOOL					m_bPeaceZone;				//	평화지역 유무. GLLevelFile 에 이미 정의되어 있음.
	float					m_fAge;

	bool					m_bClubBattle;				//	클럽전 지역.
	bool					m_bClubBattleHall;			//	클럽전 지역.
	float					m_fCommission;				//	상거래 수수료.

	bool					m_bClubDeathMatch;			//	CDM지역
	bool					m_bClubDeathMatchHall;		//	CDM지역 

	bool					m_bClubBattleInProgress;		//	선도클럽결정전 진행중

	DWORD					m_dwClubMapID;					//  선도 및 CDM ID
	DWORD					m_dwGuidClub;					//	선도클럽 아이디.
	DWORD					m_dwGuidClubMarkVer;
	char					m_szGuidClubName[CHAR_SZNAME];	//	선도클럽 마크.

    SPVEInDunID             m_sPVEIndunID;

protected:
	D3DXVECTOR3				m_vMax;
	D3DXVECTOR3				m_vMin;

	LPDIRECT3DDEVICEQ		m_pd3dDevice;
	CLIENTQUADTREE			m_LandTree;

	// 충돌처리가 Thread 상에서 일어나서 shared_ptr 로 변경함.
	boost::shared_ptr<DxLandMan>	m_spLandMan;

protected:
	DWORD					m_dwCOUNT;
    DROP_ITEM_MAP           m_DropItem; //! 필드에 떨어진 아이템	
    DROP_MONEY_MAP          m_DropMoney; //! 필드에 떨어진 게임머니	

	DROP_CROW_MAP           m_DropCrow; //! Crow/Npc  
    DROP_MATERIAL_MAP       m_DropMaterial;	
	
    DROP_SUMMON_MAP         m_DropSummon; //! Summon

	// PET
	PETMAPID				m_PETArray;

	// LandEvent
	GLLandEventClient*      m_pLandEvent;

	CHARMAPID				m_PCArray; //! GaeaID/Pointer
	CHARMAP					m_mapPC; //! Cha Name/Pointe
	CHARMAPID				m_DBArry; //! DBNUM/Pointe
	
	
	TriggerSystem::SpTriggerFinger m_spTriggerFinger;	// 트리거 작동기

    Faction::ManagerClient* m_FactionMananger;

public:
	BOOL DropItem ( SDROP_CLIENT_ITEM *pItemDrop );
	BOOL DropMoney ( LONGLONG lnAmount, D3DXVECTOR3 vPos, DWORD dwGlobID, float fAge, BOOL bCanGet );
	
	BOOL DropCrow(SDROP_CROW* pCrowDrop);
	BOOL DropChar(std::tr1::shared_ptr<SDROP_CHAR> spCharDrop);
	BOOL DropMaterial ( SDROP_MATERIAL *pCrowDrop );

public:
	BOOL DropOutItem ( DWORD dwGlobID );
	BOOL DropOutMoney ( DWORD dwGlobID );

	void DropOutCrow ( DWORD dwGlobID );
	BOOL DropOutChar ( DWORD dwGlobID );
	BOOL DropOutMaterial ( DWORD dwGlobID );

	// PET
	BOOL DropPet ( PSDROPPET pPet );
	BOOL CreateAnyPet ( PSDROPPET pPet );
	BOOL DropOutPet ( DWORD dwGUID );
	std::tr1::shared_ptr<GLAnyPet> GetPet ( DWORD dwGUID );

	// Summon
	BOOL DropSummon ( PSDROPSUMMON pSummon );
	BOOL DropOutSummon ( DWORD dwGUID );
    std::tr1::shared_ptr<GLSummonClient> GetSummon(DWORD dwGUID);
    void SetDropSummon(DWORD GlobId, std::tr1::shared_ptr<GLSummonClient> spSummon);
    void DeleteDropSummon(DWORD GlobId);

	// LandEvent
	template <typename LANDEVENTNODE>
    inline LANDEVENTNODE* DropLandEvent(
        const DWORD dwGUID,
        const D3DXVECTOR3& vInPos,
        const FLOAT fEndTime,
        const FLOAT fEventTime )
    {
        return m_pLandEvent->NewEvent<LANDEVENTNODE>( dwGUID, this, vInPos, fEndTime, fEventTime );
    }

    /**
        PVE 컨텐츠용 인던의 고유 ID 를 설정 한다.
        생성시 설정 되어 인던 진행시 사용 한다.        

     */
    void                SetPVEIndunID(const SPVEInDunID id) { m_sPVEIndunID = id; }
    const SPVEInDunID   GetPVEIndunID() const { return m_sPVEIndunID; }

	HRESULT ClearDropObj ();

	BOOL ResetClientObjects ();

    // TriggerSystem    
    void StartTriggerFinger(int nStage, int nLevel);
    void SendTriggerMessage(TriggerSystem::SMessage* msg);
    void PostTriggerMessage(TriggerSystem::SpMessage msg);

public:
	BOOL ChangeCharMap( char* szOldName, char* szNewName );

public:
    std::tr1::shared_ptr<CItemClientDrop> GetItem(DWORD dwGlobID);
    void SetDropItem(DWORD GlobId, std::tr1::shared_ptr<CItemClientDrop> spDropItem);
    void DeleteDropItem(DWORD GlobId);

	std::tr1::shared_ptr<CMoneyClientDrop> GetMoney(DWORD dwGlobID);
    void SetDropMoney(DWORD GlobId, std::tr1::shared_ptr<CMoneyClientDrop> spDropMoney);
    void DeleteDropMoney(DWORD GlobId);

	std::tr1::shared_ptr<GLCrowClient> GetCrow(DWORD dwID);
    void SetDropCrow(DWORD GlobId, std::tr1::shared_ptr<GLCrowClient> spCrow);
    void DeleteDropCrow(DWORD GlobId);

	std::tr1::shared_ptr<GLCharClient> GetChar(DWORD dwID);
	std::tr1::shared_ptr<GLCharClient> GetCharByDBnum(DWORD dwDBnum);
	
    std::tr1::shared_ptr<GLMaterialClient> GetMaterial(DWORD dwID);
    void SetDropMaterial(DWORD GlobId, std::tr1::shared_ptr<GLMaterialClient> spDropMaterial);
    void DeleteDropMaterial(DWORD GlobId);

	CEVENT_NODE_CLIENT* GetLandEventNode ( DWORD dwID );

	std::tr1::shared_ptr<GLCharClient> FindChar(const std::string& strName);
	std::tr1::shared_ptr<GLCharClient> FindChar(DWORD dwID);
	std::tr1::shared_ptr<GLCharClient> FindCharByDBnum(DWORD dwDBnum);

public:
	void SETITEM_PICKDELAY ( DWORD dwGlobID );
	void SETMONEY_PICKDELAY ( DWORD dwGlobID );

public:
	BOOL ISITEM_PICKDELAY ( DWORD dwGlobID );
	BOOL ISMONEY_PICKDELAY ( DWORD dwGlobID );

	void AllCharClientUpdateSuit();

public:
	WORD GetTargetBodyRadius ( const STARGETID &sTargetID );

public:
	void DisplayAllUser ();

public:
	const char*				GetGLLandName ()		{ return m_strFileName.c_str(); }

public:
	DxLandMan*							GetLandMan();
	const boost::shared_ptr<DxLandMan>&	GetLandManSp()	{ return m_spLandMan; }

	boost::shared_ptr<NavigationMesh>	GetNaviMesh ();
	CLIENTQUADTREE*						GetLandTree ()			{ return &m_LandTree; }
	GLLandEventClient*					GetLandEvent ()         { return m_pLandEvent; }

	BOOL IsPeaceZone ()								{ return m_bPeaceZone; }

	BOOL GetUsedMaterialSystem() const;

    __forceinline Faction::ManagerClient* const GetFactionManager(void) { return m_FactionMananger; };

public:
	HRESULT FrameMove ( float fTime, float fElapsedTime, const CLIPVOLUME &cv, BOOL bNotRendering );
	HRESULT FrameMoveItem ( float fElapsedTime );
	
	HRESULT Render( CLIPVOLUME &CV, LPDIRECT3DSURFACEQ pColorBuffer );
	HRESULT Render_2Pass( CLIPVOLUME &CV );
	HRESULT Render_MobItem( CLIPVOLUME &CV );
	HRESULT Render_MobItem_SoftAlpha( CLIPVOLUME &CV );
	HRESULT Render_MobItem_MaterialEffect( CLIPVOLUME &CV );
	HRESULT Render_MobItem_Shadow( CLIPVOLUME &CV, BOOL bWorldSystemOLD );
	HRESULT Render_MobItem_Reflect( CLIPVOLUME &CV );
	HRESULT Render_AlphaObejct( CLIPVOLUME &CV, LPDIRECT3DSURFACEQ pColorBuffer );
	void	Render_SingleEffect( CLIPVOLUME &CV );
	void	RenderShadow();
	void	RenderShadow_SpotLight1( LPDIRECT3DDEVICEQ pd3dDevice );
	void	RenderShadow_SpotLight2( LPDIRECT3DDEVICEQ pd3dDevice );
	void	RenderStarGlow();
    void	Render_LightShafts();

	void	RegistName( const CLIPVOLUME &CV );

public:
	HRESULT Create ( const char* szSetFileName, LPDIRECT3DDEVICEQ pd3dDevice, D3DXVECTOR3 &vBasicPos, const MapID& mapID = MapID(), BOOL bPeaceZone=FALSE );
	HRESULT ActiveMap ();
	void CleanUp ();

public:
	BOOL LoadFile ( const char *szFile );

public:
	HRESULT InitDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice);
	HRESULT RestoreDeviceObjects();

	HRESULT InvalidateDeviceObjects();
	HRESULT DeleteDeviceObjects();

	const bool IsCollision(const D3DXVECTOR3& vPoint0, const D3DXVECTOR3& vPoint1, EMCOLLMODE emCullMode) const;

    void GetCollisionDropItem(
        const D3DXVECTOR3& vFromPt,
        const D3DXVECTOR3& vTargetPt,
        std::vector<STARGETID>& vDetect) const;

    void GetSpherePointCollisionDropItem(
        const D3DXVECTOR3& vTarPos,
        float fLength,
        std::vector<STARGETID>& vDetect) const;

    void GetNearDropItem(
        float& fMinDist,
        D3DXVECTOR3& vMinPos,
        float& fMinDistNotCan,
        D3DXVECTOR3& vMinPosNotCan,
        bool& bFind,
        const D3DXVECTOR3& vCharPos) const;

    void GetCollisionDropMoney(
        const D3DXVECTOR3& vFromPt,
        const D3DXVECTOR3& vTargetPt,
        std::vector<STARGETID>& vDetect) const;

    void GetSpherePointCollisionDropMoney(
        const D3DXVECTOR3& vTarPos,
        float fLength,
        std::vector<STARGETID>& vDetect) const;

    void GetNearDropMoney(
        float& fMinDist,
        D3DXVECTOR3& vMinPos,
        float& fMinDistNotCan,
        D3DXVECTOR3& vMinPosNotCan,
        bool& bFind,
        const D3DXVECTOR3& vCharPos) const;

    void GetCollisionLineToSphereDropMaterial(
        VEC_SK_TAR& vecDetectID,
        const D3DXVECTOR3& vLineDir,
        const D3DXVECTOR3& vP0,
        const D3DXVECTOR3& vP1,
        const D3DXVECTOR3& vFromPt,
        const D3DXVECTOR3& vTargetPt,
        float fRadius) const;

    void GetCollisionLineToAabbDropMaterial(
        VEC_SK_TAR& vecDetectID,
        const D3DXVECTOR3& vFromPt,
        const D3DXVECTOR3& vTargetPt,
        bool CheckReActionAble,
        GLCharacter* pChar,
        BOOL bEnemy) const;

    void GetSpherePointCollisionDropMaterial(
        VEC_SK_TAR& vecDetectID,
        const D3DXVECTOR3& vTarPos,
        float fLength,
        bool CheckValidBody) const;

    void GetCollisionLineToSphereDropSummon(
        VEC_SK_TAR& vecDetectID,
        const D3DXVECTOR3& vLineDir,
        const D3DXVECTOR3& vP0,
        const D3DXVECTOR3& vP1,
        const D3DXVECTOR3& vFromPt,
        const D3DXVECTOR3& vTargetPt,
        float fRadius) const;

    void GetCollisionLineToAabbDropSummon(
        VEC_SK_TAR& vecDetectID,
        const D3DXVECTOR3& vFromPt,
        const D3DXVECTOR3& vTargetPt,
        bool CheckReActionAble,
        GLCharacter* pChar,
        BOOL bEnemy) const;

    void GetSpherePointCollisionDropSummon(
        const D3DXVECTOR3& vTarPos,
        float fLength,
        std::vector<STARGETID>& vDetect,
        bool CheckValidBody) const;
    bool FindStorageOpenNpc(
        const D3DXVECTOR3& vPos,
        GLCharacter* pChar,
        DWORD nPartyMember,
        DWORD& nNpcID,
        DWORD& nTalkID);

    void DetectCrowDie(
        VEC_SK_TAR& vecDetectID,
        const D3DXVECTOR3& vFromPt,
        const D3DXVECTOR3& vTargetPt);

    void DetectCrowWithCylinder(
        VEC_SK_TAR& vecDetectID,
        const D3DXVECTOR3& vLineDir,
        const D3DXVECTOR3& vP0,
        const D3DXVECTOR3& vP1,
        float fRadius);

    void GetCollisionLineToAabbDropCrow(
        VEC_SK_TAR& vecDetectID,
        const D3DXVECTOR3& vFromPt,
        const D3DXVECTOR3& vTargetPt,
        bool CheckValidBody,
        bool CheckVisibleDetect,
        bool CheckDie,
        bool CheckReactionable,
        GLCharacter* pChar,
        BOOL bEnemy);

    void GetSpherePointCollisionDropCrow(
        VEC_SK_TAR& vecDetectID,
        const D3DXVECTOR3& vTarPos,
        float fLength,
        bool CheckValidBody);

    BOOL GetCollisionMobToPoint(const D3DXVECTOR3& vPoint, WORD wBodyRadius);

    std::tr1::shared_ptr<GLCrowClient> GetQuestNpc(const GLQUEST_STEP& step);

    void GetCollisionLineToSphereDropSummon(
        VEC_SK_TAR& vecDetectID,
        const D3DXVECTOR3& vLineDir,
        const D3DXVECTOR3& vP0,
        const D3DXVECTOR3& vP1,
        const D3DXVECTOR3& vFromPt,
        const D3DXVECTOR3& vTargetPt,
        float fRadius,
        bool CheckValidBody);

    void GetCollisionLineToAabbDropSummon(
        VEC_SK_TAR& vecDetectID,
        const D3DXVECTOR3& vFromPt,
        const D3DXVECTOR3& vTargetPt,
        bool CheckValidBody,
        bool CheckReactionable,
        GLCharacter* pChar,
        BOOL bEnemy);

    void GetSpherePointCollisionDropSummon(
        VEC_SK_TAR& vecDetectID,
        const D3DXVECTOR3& vTarPos,
        float fLength,
        bool CheckValidBody);

    void PrintChar();

    void GetCollisionLineToAabbChar(
        VEC_SK_TAR& vecDetectID,
        const D3DXVECTOR3& vFromPt,
        const D3DXVECTOR3& vTargetPt,
        bool CheckDie,
        bool CheckVisible,
        bool CheckValidBody,
        bool CheckReActionAble,
        GLCharacter* pChar,
        BOOL bEnemy);

    void GetCollisionLineToSphereChar(
        VEC_SK_TAR& vecDetectID,
        const D3DXVECTOR3& vLineDir,
        const D3DXVECTOR3& vP0,
        const D3DXVECTOR3& vP1,
        const D3DXVECTOR3& vFromPt,
        const D3DXVECTOR3& vTargetPt,
        float fRadius,
        bool CheckVisible,
        bool CheckValidBody);

    void GetSpherePointCollisionChar(
        VEC_SK_TAR& vecDetectID,
        const D3DXVECTOR3& vTarPos,
        float fLength,
        bool CheckVisible,
        bool CheckValidBody);

	D3DMATRIX m_mat;
	D3DXVECTOR3 m_vVector;
	BOOL IsObstacle( D3DXVECTOR3 vThisPos, D3DXVECTOR3 vPreviousPos );
	BOOL IsInObstacle( float fRadius, float fWidth, float fLength );
	void SetObstacleMat( D3DMATRIX mat );
	D3DMATRIX GetObstacleMat();
	void SetObstacleVec( D3DXVECTOR3 vec );
	D3DXVECTOR3 GetObstacleVec();
	BOOL IsIn( float fRadius, float fLength );

	// 난사군도
public:
	void SetWorldBattleFlag( const TCHAR* pWorldBattleFlag );
};
typedef GLLandManClient* PLANDMANCLIENT;

#endif // GLLANDMANCLIENT_H_
