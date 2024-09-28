#pragma once

namespace TexasHoldem
{
	namespace GLTexasHoldem
	{
		extern const bool LoadFile( const std::string FileName );
	}

	class GLTexasHoldemSetting
	{
	private:
		GLTexasHoldemSetting(void);

	public:
		static GLTexasHoldemSetting* const getInstance(void);
		~GLTexasHoldemSetting(void);

	public:
		friend const bool TexasHoldem::GLTexasHoldem::LoadFile( const std::string FileName );

	private:
		const bool load();

	public:
		const DWORD GetRequirLevel() { return m_wRequireLevel; }
		const DWORD GetPointTransferItemMid() { return m_wPointTransferItemMid; }
		const DWORD GetPointTransferItemSid() { return m_wPointTransferItemSid; }
		std::string GetAddress() { return m_strAddress; }
		float GetCommision() { return m_fCommision; }
		bool CanChipPurchaseInGameMoney() { return m_bUseChipPurchaseInGameMoney; }
		bool CanChipPurchaseInPoint() { return m_bUseChipPurchaseInPoint; }

	private:
		std::string m_strFilePath;

		DWORD m_wRequireLevel;	//! 필요 레벨
		WORD m_wPointTransferItemMid;
		WORD m_wPointTransferItemSid;
		std::string m_strAddress;
		float m_fCommision;
		bool m_bUseChipPurchaseInGameMoney;
		bool m_bUseChipPurchaseInPoint;

	};
}
