#pragma once

#include "../../SigmaCore/Util/GLSingletone.h"
#include "../../enginelib/ServiceProviderDefine.h"
#include "../RanLogic/RandomOption/RandomOptionDefine.h"

class GLGaeaServer;
struct SCHARDATA2;
class GLInventory;

namespace Item
{
	enum eItemValidationResult
	{
		eSuccess, 
		eNothingConvert, 
		eInvalidItemData, 
		eCharacterNull, 
		eInventoryNull, 
		eInventorySpace, 
		ePileNumZero, 
		eInsertFail, 
		eGaeaServerNull, 
		eInvalidRandomOption, 
	};

	class GLItemValidation : public CSingletone< GLItemValidation > 
	{
	public:
		bool Load( const std::string& strFileName, GLGaeaServer* pGaeaServer );
		int Do( EMSERVICE_PROVIDER ServiceProvider, SCHARDATA2* pCharacter );

	public:
		struct ItemExchange 
		{
			SNATIVEID sOldItemID;
			SNATIVEID sNewItemID;
			DWORD OldItemRatio;
			DWORD NewItemRatio; 
			DWORD Remainder;

			ItemExchange()
				: sOldItemID( false )
				, sNewItemID( false )
				, OldItemRatio( 1 )
				, NewItemRatio( 1 )
				, Remainder( 1 )
			{
			}
		};

		struct ItemIDChange
		{
			SNATIVEID sOldItemID;
			SNATIVEID sNewItemID;

			ItemIDChange()
				: sOldItemID( false )
				, sNewItemID( false )
			{
			}
		};

	protected:
		friend CSingletone;

	protected:
		std::string m_strFilePath;
		GLGaeaServer* m_pServer;
		std::vector< ItemExchange > m_ItemExchange;
		std::vector< ItemIDChange > m_ItemIDChange;

	protected:
		int ExchangeItem( SCHARDATA2* pCharacter );
		int RestoreTimelimitCostumeItem( SCHARDATA2* pCharacter );
		int CheckRandomOption( SCHARDATA2* pCharacter );
		int ChangeItemID( SCHARDATA2* pCharacter );
		int ValidateVehicleRandomOption( SCHARDATA2* pCharacter );
		int ValidateWrappedItem( SCHARDATA2* pCharacter );
		int ValidateRandomOption( SCHARDATA2* pCharacter );

	private:
		GLItemValidation();
		~GLItemValidation();

	private:
		int exchangeItem( 
			DWORD ChaDbNum, 
			const SNATIVEID& OldItemID, 
			const SNATIVEID& NewItemID, 
			DWORD OldItemRatio, 
			DWORD NewItemRatio, 
			DWORD Remainder, 
			GLInventory* pInventory, 
			bool bTest = false );
		bool checkRandomOption( const SITEMCUSTOM& sItemCustom );
		bool isOverStat( const RandomOption::RANDOM_OPTION& RandomType, const short& RandomValue );

		bool loadCommon();
		bool loadServer();

		void reportError( DWORD ChaDbNum, int nResult );
	};

}