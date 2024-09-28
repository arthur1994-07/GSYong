#ifndef _GLOGIC_SERVER_GLPETFILED_H_
#define _GLOGIC_SERVER_GLPETFILED_H_

#pragma once

#include <boost/tr1/memory.hpp>
#include "../../SigmaCore/Container/GLList.h"
#include "../../Enginelib/NaviMesh/actor.h"
#include "../../RanLogic/Pet/GLPet.h"

//#include "../Character/GLChar.h"

class GLGaeaServer;
class GLChar;

namespace db
{
    class DbAction;
    typedef std::tr1::shared_ptr<DbAction> DbActionPtr;
}

class GLPetField : public GLPET
{
public:
    GLPetField(GLGaeaServer* pServer);
    virtual ~GLPetField();

private:
    GLGaeaServer* m_pGLGaeaServer;

public:
	Actor				m_actorMove;

	D3DXVECTOR3			m_vDir;
	D3DXVECTOR3			m_vDirOrig;
	D3DXVECTOR3			m_vPos;
	D3DXMATRIX			m_matTrans;

	//GLChar*			m_pOwner;

	D3DXVECTOR3			m_vTarPos;

	DWORD				m_dwActionFlag;
	float				m_fTIMER;

	DWORD				m_OwnerDbNum;		//! 주인의 Char Db Num
	DWORD				m_OwnerSvrGroupNum; //! 주인의 ServerGroupNum

protected:
	bool				m_bValid;
	PETDELAY_MAP		m_SKILLDELAY;
	WORD				m_wAniSub;

public:
	GLLandMan*				m_pLandMan;
	sc::SGLNODE<GLPetField*>* m_pLandNode;

	LANDQUADNODE*			m_pQuadNode;			//	쿼드 트리 노드. ( 직접 포인터 형을 지정하기 난감함. )
	sc::SGLNODE<GLPetField*>*	m_pCellNode;			//	셀 리스트 노드.

public:
    void AddGameDbJob(db::DbActionPtr spJob);
    void AddGameDbAdoJob(db::DbActionPtr spJob);

	HRESULT Create ( GLLandMan* pLandMan, GLChar* pOwner, GLPET* pPetData );
	HRESULT SetPosition ( GLLandMan* pLandMan );
	HRESULT SetJumpPos( const D3DXVECTOR3& vPos ); // Gen이 아닌 좌표이동.
	void	ResetActorMove();

	HRESULT FrameMove( float fTime, float fElapsedTime );
	HRESULT DeleteDeviceObject ();

	HRESULT UpdateClientState ( float fElapsedTime );
	void UpdatePetState(void);
	HRESULT SkillProcess( float fElapsedTime );

	BOOL  IsSTATE ( DWORD dwStateFlag )		 			{ return m_dwActionFlag&dwStateFlag; }
	void  SetSTATE ( DWORD dwStateFlag )				{ m_dwActionFlag |= dwStateFlag; }
	void  ReSetSTATE ( DWORD dwStateFlag )				{ m_dwActionFlag &= ~dwStateFlag; }
	void  ReSetAllSTATE ()								{ m_dwActionFlag = 0; }

	void	SetValid ()									{ m_bValid = true; }
	void	ReSetValid ()								{ m_bValid = false; }
	bool	IsValid () const							{ return m_bValid; }
	bool	IsItemProtect();

	bool IsUsePetSkinPack() { return m_sPetSkinPackData.bUsePetSkinPack; }

	void	CleanUp ();

	bool	CheckSkill ( SNATIVEID sSkillID, WORD wSlot );
	void	AccountSkill ( SNATIVEID sSkillID );
	void	UpdateSkillDelay ( float fElapsedTime );
	void	UpdateSkillState ( float fElapsedTime );
	void	ReSetSkillDelay ()							{ m_SKILLDELAY.clear(); }
	void	ActiveSkill();	//	현재 구동중인 스킬을 적용한다.

	//void	MsgProcess(NET_MSG_GENERIC* nmg);

	// Access
	inline void SetPosition(const D3DXVECTOR3& vPos) { m_vPos=vPos; }

	//! 해당 스킬에대해 활성화 되어 있는 슬롯을 찾는다
	DWORD GetActiveSkillSlotFlag(const SNATIVEID& sSkillID);

    void LogPetSkillChange();

public:
	void ReqNetMsg_Drop(GLMSG::SNETPET_DROP_PET& Msg);

	HRESULT	MsgPetGoto(NET_MSG_GENERIC* nmg);
	HRESULT	MsgPetStop(NET_MSG_GENERIC* nmg);
	HRESULT	MsgPetUpdateMoveState(NET_MSG_GENERIC* nmg);
	HRESULT	MsgUpdateVelocity(NET_MSG_GENERIC* nmg);
	HRESULT MsgUpdateState(NET_MSG_GENERIC* nmg);
	//HRESULT MsgChangeSkill(NET_MSG_GENERIC* nmg);
	//HRESULT MsgUpdateSkillState(NET_MSG_GENERIC* nmg);

	HRESULT MsgPetRename(NET_MSG_GENERIC* nmg);
	HRESULT PetRenameFeedBack(NET_MSG_GENERIC* nmg);

	HRESULT	MsgPetChangeColor(NET_MSG_GENERIC* nmg);
	HRESULT	MsgPetChangeStyle(NET_MSG_GENERIC* nmg);
	HRESULT	MsgPetChangeActiveSkill(NET_MSG_GENERIC* nmg);
	HRESULT	MsgPetAccInvenExSlot( NET_MSG_GENERIC* nmg );
	HRESULT	MsgPetAccInvenToSlot( NET_MSG_GENERIC* nmg );
	HRESULT	MsgPetAccSlotToInven( NET_MSG_GENERIC* nmg );
	HRESULT	MsgPetLearnSkill(NET_MSG_GENERIC* nmg);
	HRESULT	MsgPetRemoveSlotItem(NET_MSG_GENERIC* nmg);
	HRESULT MsgPetSkinPackItem(NET_MSG_GENERIC* nmg);
	HRESULT MsgPetSkillSlotExpansion (  NET_MSG_GENERIC* nmg );
    // NET_MSG_PET_REQ_FUNNY
	HRESULT MsgPetFunnyReq( NET_MSG_GENERIC* nmg);

    //! 리미트 아이템 업데이트
    void UpdateTimeLmtItem( GLChar* pChar );

	// 착용아이템 해제
	// DbUpdate true 면 바로 디비에서 삭제된다.
	void PetPutOnItemReset( ACCESSORYTYPE Type, bool DbUpdate = false );
	void PetPutOnItemReset( EMSUIT emSuit, bool DbUpdate = false );

	// 디비 업데이트(insert or update)
	void PetPutOnItemUpdate( ACCESSORYTYPE Type, bool JustInvenTypeUpdate = false );
	void PetPutOnItemUpdate( EMSUIT emSuit, bool JustInvenTypeUpdate = false );

	bool GetAccessoryType( EMSUIT emSuit, ACCESSORYTYPE& Type );
	bool IsValidSlot( EMSUIT emSuit );

};

typedef GLPetField* PGLPETFIELD;

typedef sc::CGLLIST<PGLPETFIELD>	GLPETLIST;
typedef sc::SGLNODE<PGLPETFIELD>	GLPETNODE;

#endif // _GLOGIC_SERVER_GLPETFILED_H_