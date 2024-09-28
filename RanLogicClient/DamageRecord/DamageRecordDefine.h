#pragma once

namespace DamageRecord
{
	// UI ǥ�� ����(VIEW_SLOT_NSIZE) ����� UI�ʵ� ���� �۾�(ũ��, ���Ե�) ���־�� ��;
	enum
	{
		VIEW_SLOT_NSIZE = 5,
		LOG_ATTRIBUTE_NSIZE = 6,
	};

	extern const char strAttribute[LOG_ATTRIBUTE_NSIZE][32];

	class LogID
	{
		enum
		{
			INVALID = 0xffffffff,
		};
	public:		
		const bool operator ==(const LogID& rhs) const;
		const bool operator !=(const LogID& rhs) const;
		const bool operator <(const LogID& rhs) const;		

		LogID(void);
		LogID(const LogID& rhs);
		LogID(const DWORD _dwDbNum);
		LogID(const WORD _wMainID, const WORD _wSubID);

	public:
		union
		{
			DWORD dwDbNum;
			struct 
			{
				WORD wSubID;
				WORD wMainID;				
			};
		};
		DWORD nType;		
	};
}