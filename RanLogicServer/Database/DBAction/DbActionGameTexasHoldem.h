#pragma once

#include "DbAction.h"

namespace TexasHoldem 
{ 
	struct sChipPrice;
}

namespace db
{
	class TexasHoldemChipMapLoad : public DbAction
	{
	public:
		TexasHoldemChipMapLoad();
		virtual ~TexasHoldemChipMapLoad();

	protected:
		std::vector< TexasHoldem::sChipPrice > m_vecChip;

	public:
		virtual int Execute( NetServer* pServer ) override;
	};

	class TexasHoldemChipMapVersionLoad : public DbAction
	{
	public:
		TexasHoldemChipMapVersionLoad();
		virtual ~TexasHoldemChipMapVersionLoad();

	protected:
		LONGLONG m_Version;

	public:
		virtual int Execute( NetServer* pServer ) override;
	};

	class TexasHoldemChipSave : public DbAction
	{
	public:
		TexasHoldemChipSave( DWORD dwChaDbNum, LONGLONG chip, bool bPlus = false );
		virtual ~TexasHoldemChipSave();

	protected:
		DWORD m_dwChaDbNum;
		LONGLONG m_chip;
		bool m_bPlus;

	public:
		virtual int Execute( NetServer* pServer ) override;
	};

	class TexasHoldemChipLogSave : public DbAction
	{
	public:
		TexasHoldemChipLogSave( DWORD dwChaDbNum, LONGLONG chip, LONGLONG GameMoney, LONGLONG Point );
		virtual ~TexasHoldemChipLogSave();

	protected:
		DWORD m_dwChaDbNum;
		LONGLONG m_chip;
		LONGLONG m_GameMoney;
		LONGLONG m_Point;

	public:
		virtual int Execute( NetServer* pServer ) override;
	};

	class TexasHoldemChipLoad : public DbAction
	{
	public:
		TexasHoldemChipLoad( DWORD dwChaDbNum, bool bEndGameNotify = false );
		virtual ~TexasHoldemChipLoad();

	protected:
		DWORD m_dwChaDbNum;
		LONGLONG m_chip;
		bool m_bEndGameNotify;

	public:
		virtual int Execute( NetServer* pServer ) override;
	};

	class TexasHoldemAuthKeySave : public DbAction
	{
	public:
		TexasHoldemAuthKeySave( DWORD dwChaDbNum, UUID& sGUID );
		virtual ~TexasHoldemAuthKeySave();

	protected:
		DWORD m_dwChaDbNum;
		UUID m_sGUID;

	public:
		virtual int Execute( NetServer* pServer ) override;
	};

	class TexasHoldemGameTimeSave : public DbAction
	{
	public:
		TexasHoldemGameTimeSave( DWORD dwChaDbNum, DWORD dwUserNum, int nSecond );
		virtual ~TexasHoldemGameTimeSave();

	protected:
		DWORD m_dwChaDbNum;
		DWORD m_dwUserNum;
		int m_nSecond;

	public:
		virtual int Execute( NetServer* pServer ) override;
	};

} // namespace db