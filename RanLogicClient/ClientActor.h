#pragma once

#include "../EngineLib/NaviMesh/Actor.h"
#include "../EngineLib/G-Logic/GLDefine.h"
#include "../RanLogic/Actor/GLActor.h"

struct NET_MSG_GENERIC;

class GLGaeaClient;
class ClientActor : public GLActor
{
public:
	virtual const SNATIVEID GetCrowID(void) const { return SNATIVEID(); } // GetCharID()와 통합 할것;
	virtual const char* GetName() const = 0;
	virtual WORD GetLevel() const { return 0; }
	virtual const EMCHARINDEX GetCharIndex() const { return GLCI_FIGHTER_M; }
	virtual DWORD GetClubID() const { return CLUB_NULL; }
	virtual const WORD GetCharSchool() const { return 0; }

public:
	//	virtual BOOL	IsACTION( EMACTIONTYPE emCur ) const = 0;
	//	virtual BOOL	IsValidBody()	const			{ return TRUE; } 
	virtual BOOL	IsPartyMaster()					{ return FALSE; }
	virtual BOOL	IsPartyMem()					{ return FALSE; }
	virtual bool	IsClubMaster()					{ return false; }
	virtual BOOL	IsCDCertify()					{ return FALSE; }
	virtual bool 		IsSafeZone()	const		{ return false; }
	virtual bool 		IsCDMSafeTime() const		{ return false; }
	virtual BOOL	IsNonDamaged()	const			{ return FALSE; }
	virtual BOOL	IsDominated() const				{ return FALSE; }
	virtual BOOL	IsVisibleDetect(void) const		{ return TRUE; }

public:

    /**
        이름을 렌더링 안 할지 설정 한다.
        타켓팅이 불가능 하게 할지 설정 한다.
        우리는 장애물 같은 물체는 이름을 렌더링하지 않고 타켓팅을 안되게 할 수 있다.

     */
    virtual const bool IsHideNaming() const { return false; }
	virtual const BOOL IsNonTargetable(void) const = 0;   
	virtual const bool IsHideTooltip(void) const { return false; }
	virtual const BOOL IsFeignDie() const { return FALSE; }

public:
	virtual void UPDATE_DATA_FINISH(const DWORD dwSkillFactIndex) = 0;

public:
	virtual WORD				GetBodyRadius() 		= 0;
	virtual float 				GetBodyHeight() 		= 0;    
	virtual GLDWDATA			GetHp() const		= 0;
	virtual D3DXVECTOR3 		GetPosBodyHeight()		= 0;    
	virtual const D3DXVECTOR3& 	GetMaxOrg() 			= 0;
	virtual const D3DXVECTOR3& 	GetMinOrg() 			= 0;
	virtual float				GetDirection()			= 0;

public:
	virtual void 				ReceiveDamage(const int nDamage, DWORD dwDamageFlag, WORD wSchool, const STARGETID& sACTOR) = 0;
	virtual void 				ReceiveHeal(const int nHeal, DWORD dwHealFlag, WORD wSchool, const STARGETID& sACTOR) = 0;
	virtual void 				ReceiveBlocking(const STARGETID& sACTOR) = 0;
	virtual void 				ReceiveAVoid( const STARGETID& sACTOR ) = 0;
	virtual void 				ReceiveSwing() 			= 0;

public:
	virtual void				FactEffectProcessing ( const EMFACTEFFPROCESSING emFactEffactCase ) = 0;

public:
	virtual void				MsgProcess( NET_MSG_GENERIC* nmg ) = 0;    

public:
	virtual void Reset(void) override;

public:
	ClientActor(GLGaeaClient* const pGaeaClient, const EMCROW emCrow);
	virtual ~ClientActor(void);


protected:
	GLGaeaClient* const m_pGaeaClient;
};

