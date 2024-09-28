#ifndef _DB_ACTION_GAME_PET_H_
#define _DB_ACTION_GAME_PET_H_

#pragma once

#include "../../../RanLogic/Pet/GLPet.h"
#include "../../../RanLogic/Item/GLItem.h"
#include "DbAction.h"

class GLPetField;

namespace db
{

//! 새로운 팻을 생성한다
class PetCreate : public DbAction
{
public:
	/**
	* 새로운 팻을 생성한다.
	* \param nChaNum  팻 소유자 캐릭터 번호
	* \param szPetName 팻이름
	* \param nPetType 팻의 타입
	* \param nPetMID 팻 MID
	* \param nPetSid 팻 SID
	* \param nPetStyle 팻의 스타일
	* \param nPetColor 팻의 컬러
	* \return sc::db::DB_ERROR 또는 성공했을때는 Pet 의 고유번호
	*/
	PetCreate(
		DWORD dwClientID,
		int nChaNum,
		const TCHAR* szPetName,
		int nPetType,
		int nPetMID,
		int nPetSID,
		int nPetStyle,
		int nPetColor,
		WORD wPosX,
		WORD wPosY,
		int nPetCardMID,
		int nPetCardSID
		);
	virtual ~PetCreate() {};
	virtual int Execute(NetServer* pServer) override;

protected:
	int m_nChaNum;
	CString m_strPetName;
	int m_nPetType;
	int m_nPetMID;
	int m_nPetSID;
	int m_nPetStyle;
	int m_nPetColor;
	
	WORD m_wPosX;
	WORD m_wPosY;

	int m_nPetCardMID;
	int m_nPetCardSID;
};

//! 팻의 이름을 변경한다
class PetRename : public DbAction
{
public:
	/**
	* 팻의 이름을 변경한다.
	* \param nPetNum 팻의 고유번호
	* \param szPetName 팻이름	
	*/
	PetRename(DWORD dwClientID, int nChaNum, int nPetNum, const TCHAR* szPetName, WORD wPosX, WORD wPosY);
	virtual ~PetRename() {};
	virtual int Execute(NetServer* pServer) override;

protected:
	int m_nChaNum;
	int m_nPetNum;
	CString m_strPetName;
	WORD m_wPosX;
	WORD m_wPosY;
};

//! 팻의 소유권을 이전한다.
class PetExchange : public DbAction
{
public:
	//! 팻의 소유권을 이전한다.
	//! \param nChaNum 팻의 새로운 소유자
	//! \param nPetNum 소유권을 이전할 팻 번호
	PetExchange(int nChaNum, int nPetNum);
	virtual ~PetExchange();
	virtual int Execute(NetServer* pServer) override;

protected:
	int m_nChaNum;
	int m_nPetNum;
};

//! 팻의 컬러를 변경한다.
class PetColorSet : public DbAction
{
public:
	/**
	* 팻의 컬러를 변경한다.
	* \param nPetNum 컬러를 변경하려는 팻의 번호
	* \param nPetColor 팻의 컬러
	*/
	PetColorSet(int nChaNum, int nPetNum, int nPetColor);
	virtual ~PetColorSet() {};
	virtual int Execute(NetServer* pServer) override;

protected:
	int m_nChaNum;
	int m_nPetNum;
	int m_nPetColor;
};

//! 팻의 스타일을 변경한다.
class PetStyleSet : public DbAction
{
public:
	/**
	* 팻의 스타일을 변경한다.
	* \param nPetNum 스타일을 변경하려는 팻의 번호
	* \param nPetStyle 팻의 스타일
	*/
	PetStyleSet(int nChaNum, int nPetNum, int nPetStyle);
	virtual ~PetStyleSet() {};
	virtual int Execute(NetServer* pServer) override;

protected:
	int m_nChaNum;
	int m_nPetNum;
	int m_nPetStyle;
};

//! 팻의 포만도를 설정한다. 포만도는 0-100 사이
class PetFullSet : public DbAction
{
public:
	/**
	* 팻의 포만도를 설정한다. 포만도는 0-100 사이
	* \param nPetNum 포만도를 설정할 팻의 번호
	* \param fPetFull 포만도
	*/
	PetFullSet(int nChaNum, int nPetNum, int nPetFull);
	virtual ~PetFullSet() {};
	virtual int Execute(NetServer* pServer) override;

protected:
	int m_nChaNum;
	int m_nPetNum;
	int m_nPetFull;
};

//! 팻의 스킬 업데이트
class PetSkillSet : public DbAction
{
public:
	PetSkillSet(
		int nChaNum,
		int nPetNum,
		const PETSKILL& PetSkill,
		DWORD dwActiveSlotFlag );
	virtual ~PetSkillSet() {};
	virtual int Execute(NetServer* pServer) override;

protected:
	int m_nChaNum;
	int m_nPetNum;
	PETSKILL m_PetSkill;
	DWORD m_dwActiveSlotFlag;
};

//! 팻의 인벤토리 업데이트
class PetInvenSet : public DbAction
{
public:
	PetInvenSet(
		int nChaNum,
		int nPetNum,
		GLPET* pPet);
	virtual ~PetInvenSet() {};
	virtual int Execute(NetServer* pServer) override;

protected:
	int m_nChaNum;
	int m_nPetNum;
	std::vector< SINVENITEM_SAVE > m_vecItem;
	//se::ByteStream m_ByteStream;
};

//! 팻의 스킬 슬롯 갯수 업데이트
class PetSkillSlotNumSet : public DbAction
{
public:
	PetSkillSlotNumSet(
		int nChaNum,
		int nPetNum,
		WORD wSkillSlotNum );
	virtual ~PetSkillSlotNumSet() {};
	virtual int Execute(NetServer* pServer) override;

protected:
	int m_nChaNum;
	int m_nPetNum;
	WORD m_wSkillSlotNum;
};

//! 팻을 삭제한다.
class PetDelete : public DbAction
{
public:
	/**
	* 팻을 삭제한다.
	* \param nPetNum 삭제할 팻의 번호
	*/
    PetDelete(int nChaNum, int nPetNum);
	virtual ~PetDelete() {};
	virtual int Execute(NetServer* pServer) override;

protected:
	int m_nChaNum;
	int m_nPetNum;
};

//! 팻의 정보를 가져온다.
class PetGet : public DbAction
{
public:
    explicit PetGet(GLPetField* pPet, DWORD dwPetNum, DWORD dwClientID, DWORD dwCharNum, WORD wPosX, WORD wPosY, bool bLMTItemCheck=false, bool bCardInfo=false, bool bTrade=false );
	virtual ~PetGet() {};
	virtual int Execute(NetServer* pServer) override;

protected:
	GLPetField* m_pPet;
	DWORD  m_dwPetNum;
	bool   m_bLMTItemCheck;
	bool   m_bCardInfo;
	bool   m_bTrade;
	DWORD  m_dwCharNum;
	WORD   m_wPosX;
	WORD   m_wPosY;
};

//! 팻의 포만감을 가져온다
//! \param dwPetNum 팻 번호
//! \return 팻의 포만감 or sc::db::DB_ERROR
class PetFullGet : public DbAction
{
public:
    PetFullGet( int nChaNum, DWORD dwPetNum, DWORD dwClientID, WORD wCureVolume, BOOL bRatio, SNATIVEID sID, int emType, WORD HoldPosX, WORD HoldPosY );
	virtual ~PetFullGet() {};
	virtual int Execute(NetServer* pServer) override;

protected:
	int	m_nChaNum;
	DWORD m_dwPetNum;
	int m_nFull;
	WORD m_wCureVolume;
	BOOL m_bRatio;
	SNATIVEID m_sNativeID;
	int m_emType;
    WORD m_HoldPosX;
    WORD m_HoldPosY;
};

//! 소멸된 팻의 리스트를 가져온다.
//! \param nChaNum 소멸된 펫 리스트를 출력하려는 캐릭터 번호
//! \reutrn sc::db::DB_OK or sc::db::DB_ERROR
class PetListRestoreGet : public DbAction
{
public:
	PetListRestoreGet(int nChaNum, DWORD dwClientID);
	virtual ~PetListRestoreGet();
	virtual int Execute(NetServer* pServer) override;
    
protected:
	int m_nChaNum;
};

//! 팻을 부활시킨다.
//! \param nPetNum 부활할 팻 번호
//! \param nChaNum 부활을 요구한 캐릭터 번호
//! \return 팻 번호 or sc::db::DB_ERROR
class PetRestore : public DbAction
{
public:
	PetRestore( DWORD dwPetNum, DWORD dwClientID, int nChaNum, WORD wPosX, WORD wPosY, WORD HoldPosX, WORD HoldPosY );
	virtual ~PetRestore();
	virtual int Execute(NetServer* pServer) override;

protected:
	int		m_dwPetNum;
	int		m_nChaNum;
	WORD    m_wPosX;
	WORD    m_wPosY;
    WORD    m_HoldPosX;
    WORD    m_HoldPosY;
};

//! 팻 스킨 변경 카드를 적용시킨다.
//! \param nPetNum 부활할 팻 번호
//! \param nChaNum 부활을 요구한 캐릭터 번호
//! \return 팻 번호 or sc::db::DB_ERROR
class PetSkinPackSet : public DbAction
{
public:
	PetSkinPackSet( int nChaNum, DWORD dwPetNum, int nPetSkinMID, int nPetSkinSID, int nPetSkinScale, int nPetSkinTime );
	virtual ~PetSkinPackSet() {};
	virtual int Execute( NetServer* pServer);

protected:
	int   m_nChaNum;
	DWORD m_dwPetNum;
	int   m_nPetSkinMID;
	int   m_nPetSkinSID;
	int   m_nPetSkinScale;
	int   m_nPetSkinTime;
};

} // namespace db

#endif // _DB_ACTION_GAME_PET_H_