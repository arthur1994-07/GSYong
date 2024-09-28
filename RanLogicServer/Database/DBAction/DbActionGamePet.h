#ifndef _DB_ACTION_GAME_PET_H_
#define _DB_ACTION_GAME_PET_H_

#pragma once

#include "../../../RanLogic/Pet/GLPet.h"
#include "../../../RanLogic/Item/GLItem.h"
#include "DbAction.h"

class GLPetField;

namespace db
{

//! ���ο� ���� �����Ѵ�
class PetCreate : public DbAction
{
public:
	/**
	* ���ο� ���� �����Ѵ�.
	* \param nChaNum  �� ������ ĳ���� ��ȣ
	* \param szPetName ���̸�
	* \param nPetType ���� Ÿ��
	* \param nPetMID �� MID
	* \param nPetSid �� SID
	* \param nPetStyle ���� ��Ÿ��
	* \param nPetColor ���� �÷�
	* \return sc::db::DB_ERROR �Ǵ� ������������ Pet �� ������ȣ
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

//! ���� �̸��� �����Ѵ�
class PetRename : public DbAction
{
public:
	/**
	* ���� �̸��� �����Ѵ�.
	* \param nPetNum ���� ������ȣ
	* \param szPetName ���̸�	
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

//! ���� �������� �����Ѵ�.
class PetExchange : public DbAction
{
public:
	//! ���� �������� �����Ѵ�.
	//! \param nChaNum ���� ���ο� ������
	//! \param nPetNum �������� ������ �� ��ȣ
	PetExchange(int nChaNum, int nPetNum);
	virtual ~PetExchange();
	virtual int Execute(NetServer* pServer) override;

protected:
	int m_nChaNum;
	int m_nPetNum;
};

//! ���� �÷��� �����Ѵ�.
class PetColorSet : public DbAction
{
public:
	/**
	* ���� �÷��� �����Ѵ�.
	* \param nPetNum �÷��� �����Ϸ��� ���� ��ȣ
	* \param nPetColor ���� �÷�
	*/
	PetColorSet(int nChaNum, int nPetNum, int nPetColor);
	virtual ~PetColorSet() {};
	virtual int Execute(NetServer* pServer) override;

protected:
	int m_nChaNum;
	int m_nPetNum;
	int m_nPetColor;
};

//! ���� ��Ÿ���� �����Ѵ�.
class PetStyleSet : public DbAction
{
public:
	/**
	* ���� ��Ÿ���� �����Ѵ�.
	* \param nPetNum ��Ÿ���� �����Ϸ��� ���� ��ȣ
	* \param nPetStyle ���� ��Ÿ��
	*/
	PetStyleSet(int nChaNum, int nPetNum, int nPetStyle);
	virtual ~PetStyleSet() {};
	virtual int Execute(NetServer* pServer) override;

protected:
	int m_nChaNum;
	int m_nPetNum;
	int m_nPetStyle;
};

//! ���� �������� �����Ѵ�. �������� 0-100 ����
class PetFullSet : public DbAction
{
public:
	/**
	* ���� �������� �����Ѵ�. �������� 0-100 ����
	* \param nPetNum �������� ������ ���� ��ȣ
	* \param fPetFull ������
	*/
	PetFullSet(int nChaNum, int nPetNum, int nPetFull);
	virtual ~PetFullSet() {};
	virtual int Execute(NetServer* pServer) override;

protected:
	int m_nChaNum;
	int m_nPetNum;
	int m_nPetFull;
};

//! ���� ��ų ������Ʈ
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

//! ���� �κ��丮 ������Ʈ
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

//! ���� ��ų ���� ���� ������Ʈ
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

//! ���� �����Ѵ�.
class PetDelete : public DbAction
{
public:
	/**
	* ���� �����Ѵ�.
	* \param nPetNum ������ ���� ��ȣ
	*/
    PetDelete(int nChaNum, int nPetNum);
	virtual ~PetDelete() {};
	virtual int Execute(NetServer* pServer) override;

protected:
	int m_nChaNum;
	int m_nPetNum;
};

//! ���� ������ �����´�.
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

//! ���� �������� �����´�
//! \param dwPetNum �� ��ȣ
//! \return ���� ������ or sc::db::DB_ERROR
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

//! �Ҹ�� ���� ����Ʈ�� �����´�.
//! \param nChaNum �Ҹ�� �� ����Ʈ�� ����Ϸ��� ĳ���� ��ȣ
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

//! ���� ��Ȱ��Ų��.
//! \param nPetNum ��Ȱ�� �� ��ȣ
//! \param nChaNum ��Ȱ�� �䱸�� ĳ���� ��ȣ
//! \return �� ��ȣ or sc::db::DB_ERROR
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

//! �� ��Ų ���� ī�带 �����Ų��.
//! \param nPetNum ��Ȱ�� �� ��ȣ
//! \param nChaNum ��Ȱ�� �䱸�� ĳ���� ��ȣ
//! \return �� ��ȣ or sc::db::DB_ERROR
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