#include "../pch.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/File/StringFile.h"
#include "../../SigmaCore/String/StringUtils.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/Math/Random.h"

#include "../../EngineLib/GlobalParam.h"
#include "../../EngineLib/DxMeshs/SkinMesh/DxSkinDataDummy.h"
#include "../../EngineLib/G-Logic/GLogic.h"
#include "../../Enginelib/GUInterface/GameTextControl.h"
#include "../../EngineLib/Common/GLTexFile.h"

#include "../../RanLogic/GLUseFeatures.h"
#include "../../RanLogic/GLStringTable.h"
#include "../NpcTalk/NpcTalkMan.h"
#include "../Item/GLItemMan.h"
#include "../Crow/CrowAttack.h"
#include "../GLogicData.h"
#include "../GLogicDataMan.h"
#include "../GLStringTable.h"
#include "./GLCrowData.h"


// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

SCROWDATA::SCROWDATA()
    : m_pANIMATION(NULL)
{
    m_spNpcTalkDlg = std::tr1::shared_ptr<CNpcDialogueSet> (new CNpcDialogueSet);
    for ( int i=0; i<SCROWACTION::SALENUM; ++i )
        m_mapNpcSellPrice.clear();
    m_vecCrowAttack.clear();
	m_vecCrowAttackEvent.clear();

	m_vecCrowAttackPage.clear();
	m_vecCrowAttackEventPage.clear();

	for ( unsigned int i=0; i<EMMAXPAGE; ++i )
		m_vecCrowAttackPage.push_back( CROWATTACK_VEC() );

	for ( unsigned int i=0; i<EMMAXPAGE; ++i )
		m_vecCrowAttackEventPage.push_back( CROWATTACK_VEC() );

	m_fun_ptr_crow[ CROW_SALE_V_INDEX(GLInventorySale::CROW_SALE_V_1) ] = &SaleInvenLoadVer1;
	m_fun_ptr_crow[ CROW_SALE_V_INDEX(GLInventorySale::CROW_SALE_V_2) ] = &SaleInvenLoadVer2;
}

SCROWDATA::~SCROWDATA()
{
    SAFE_DELETE_ARRAY(m_pANIMATION);
}

SCROWDATA& SCROWDATA::Assign ( SCROWDATA &Data )
{
	m_sBasic = Data.m_sBasic;
	m_sAction = Data.m_sAction;
	m_sGenerate = Data.m_sGenerate;

	m_vecCrowAttack = Data.m_vecCrowAttack;
	m_vecCrowAttackEvent = Data.m_vecCrowAttackEvent;

	m_vecCrowAttackPage = Data.m_vecCrowAttackPage;
	m_vecCrowAttackEventPage = Data.m_vecCrowAttackEventPage;


#if defined( _ITEM_GEN_060417_H_ ) || defined( _QUESTION_ITEM_GEN_060417_H_ )
	m_sGenItem   = Data.m_sGenItem;
	m_sQtGenItem = Data.m_sQtGenItem;
#else
    m_cGenItem = Data.m_cGenItem;
#endif

	m_spNpcTalkDlg = Data.m_spNpcTalkDlg;

	for ( int i=0; i<SCROWACTION::SALENUM; ++i )
	{
		m_strSaleType[i] = Data.m_strSaleType[i];
		m_sSaleItems[i].Assign ( Data.m_sSaleItems[i] );
	}

	m_mapNpcSellPrice = Data.m_mapNpcSellPrice;

	if ( Data.m_pANIMATION )
	{
		SAFE_DELETE_ARRAY ( m_pANIMATION );
		m_pANIMATION = new VECANIATTACK[AN_TYPE_SIZE*AN_SUB_CROW_SIZE];

		for (int i=0; i<AN_TYPE_SIZE*AN_SUB_CROW_SIZE; ++i)
		{
			m_pANIMATION[i] = Data.m_pANIMATION[i];
		}
	}
	else
	{
		SAFE_DELETE_ARRAY ( m_pANIMATION );
	}

	return *this;
}

HRESULT SCROWDATA::LoadFile(sc::BaseStream& SFile, BOOL bServer, bool bPastLoad)
{
	DWORD dwDataVer, dwDataSize;

	SFile >> dwDataVer;
	GASSERT(dwDataVer==VERSION);

	DWORD dwDATATYPE;
	SFile >> dwDATATYPE;
	while ( dwDATATYPE!=FILE_END_DATA )
	{
		switch ( dwDATATYPE )
		{
		case FILE_SBASIC:
			{
				SFile >> dwDataVer;
				SFile >> dwDataSize;

				if( bPastLoad )
				{
					if ( dwDataVer==0x0100 )
					{
						SCROWBASIC_100 sCrowBasicOld;
						GASSERT(sizeof(sCrowBasicOld)==dwDataSize);
						SFile.ReadBuffer ( &sCrowBasicOld, sizeof(sCrowBasicOld) );

						m_sBasic = sCrowBasicOld;
					}
					else if ( dwDataVer==0x0101 )
					{
						SCROWBASIC_101 sCrowBasicOld;
						GASSERT(sizeof(sCrowBasicOld)==dwDataSize);
						SFile.ReadBuffer ( &sCrowBasicOld, sizeof(sCrowBasicOld) );

						m_sBasic = sCrowBasicOld;
					}
					else if ( dwDataVer==0x0102 )
					{
						SCROWBASIC_102 sCrowBasicOld;
						GASSERT(sizeof(sCrowBasicOld)==dwDataSize);
						SFile.ReadBuffer ( &sCrowBasicOld, sizeof(sCrowBasicOld) );

						m_sBasic = sCrowBasicOld;
					}
					else if ( dwDataVer==0x0103 )
					{
						SCROWBASIC_103 sCrowBasicOld;
						GASSERT(sizeof(sCrowBasicOld)==dwDataSize);
						SFile.ReadBuffer ( &sCrowBasicOld, sizeof(sCrowBasicOld) );

						m_sBasic = sCrowBasicOld;
					}
					else if ( dwDataVer==0x0104 )
					{
						SCROWBASIC_104 sCrowBasicOld;
						GASSERT(sizeof(sCrowBasicOld)==dwDataSize);
						SFile.ReadBuffer ( &sCrowBasicOld, sizeof(sCrowBasicOld) );

						m_sBasic = sCrowBasicOld;
					}
					else if ( dwDataVer==0x0105 )
					{
						SCROWBASIC_105 sCrowBasicOld;
						GASSERT(sizeof(sCrowBasicOld)==dwDataSize);
						SFile.ReadBuffer ( &sCrowBasicOld, sizeof(sCrowBasicOld) );

						m_sBasic = sCrowBasicOld;
					}
					else if ( dwDataVer==0x0106)
					{
						SCROWBASIC_105 sCrowBasicOld;
						GASSERT(sizeof(sCrowBasicOld)==dwDataSize);
						SFile.ReadBuffer ( &sCrowBasicOld, sizeof(sCrowBasicOld) );

						m_sBasic = sCrowBasicOld;
					}
					else if ( dwDataVer==0x0107)
					{
						SCROWBASIC_106 sCrowBasicOld;
						GASSERT(sizeof(sCrowBasicOld)==dwDataSize);
						SFile.ReadBuffer ( &sCrowBasicOld, sizeof(sCrowBasicOld) );

						m_sBasic = sCrowBasicOld;
					}
					else if ( dwDataVer==0x0108)
					{
						SCROWBASIC_108 sCrowBasicOld;
						GASSERT(sizeof(sCrowBasicOld)==dwDataSize);
						SFile.ReadBuffer ( &sCrowBasicOld, sizeof(sCrowBasicOld) );

						m_sBasic = sCrowBasicOld;
					}
					else if ( dwDataVer==0x0109)
					{
						SCROWBASIC_109 sCrowBasicOld;
						GASSERT(sizeof(sCrowBasicOld)==dwDataSize);
						SFile.ReadBuffer ( &sCrowBasicOld, sizeof(sCrowBasicOld) );

						m_sBasic = sCrowBasicOld;
					}
					else if ( dwDataVer==0x0110)
					{
						SCROWBASIC_110 sCrowBasicOld;
						GASSERT(sizeof(sCrowBasicOld)==dwDataSize);
						SFile.ReadBuffer ( &sCrowBasicOld, sizeof(sCrowBasicOld) );

						m_sBasic = sCrowBasicOld;
					}
					else if ( dwDataVer==0x0111)
					{
						SCROWBASIC_111 sCrowBasicOld;
						GASSERT(sizeof(sCrowBasicOld)==dwDataSize);
						SFile.ReadBuffer ( &sCrowBasicOld, sizeof(sCrowBasicOld) );

						m_sBasic = sCrowBasicOld;
					}
					else if ( dwDataVer==0x0112)
					{
						SCROWBASIC_112 sCrowBasicOld;
						GASSERT(sizeof(sCrowBasicOld)==dwDataSize);
						SFile.ReadBuffer ( &sCrowBasicOld, sizeof(sCrowBasicOld) );

						m_sBasic = sCrowBasicOld;
					}
					else if ( dwDataVer==0x0113)
					{
						SCROWBASIC_113 sCrowBasicOld;
						GASSERT(sizeof(sCrowBasicOld)==dwDataSize);
						SFile.ReadBuffer ( &sCrowBasicOld, sizeof(sCrowBasicOld) );

						m_sBasic = sCrowBasicOld;
					}
					else if ( dwDataVer==0x0114)
					{
						SCROWBASIC_114 sCrowBasicOld;
						GASSERT(sizeof(sCrowBasicOld)==dwDataSize);
						SFile.ReadBuffer ( &sCrowBasicOld, sizeof(sCrowBasicOld) );

						m_sBasic = sCrowBasicOld;
					}
					else if ( dwDataVer==0x0115)
					{
						SCROWBASIC_115 sCrowBasicOld;
						GASSERT(sizeof(sCrowBasicOld)==dwDataSize);
						SFile.ReadBuffer ( &sCrowBasicOld, sizeof(sCrowBasicOld) );

						m_sBasic = sCrowBasicOld;
					}
                    else if ( dwDataVer==0x0116)
                    {
                        SCROWBASIC_116 sCrowBasicOld;
                        GASSERT(sizeof(sCrowBasicOld)==dwDataSize);
                        SFile.ReadBuffer ( &sCrowBasicOld, sizeof(sCrowBasicOld) );

                        m_sBasic = sCrowBasicOld;
                    }
                    else if ( dwDataVer==0x0117)
                    {
                        SCROWBASIC_117 sCrowBasicOld;
                        GASSERT(sizeof(sCrowBasicOld)==dwDataSize);
                        SFile.ReadBuffer ( &sCrowBasicOld, sizeof(sCrowBasicOld) );

                        m_sBasic = sCrowBasicOld;
                    }
					else if ( dwDataVer==0x0118)
					{
						SCROWBASIC_118 sCrowBasicOld;
						GASSERT(sizeof(sCrowBasicOld)==dwDataSize);
						SFile.ReadBuffer ( &sCrowBasicOld, sizeof(sCrowBasicOld) );

						m_sBasic = sCrowBasicOld;
					}
					else if ( dwDataVer == 0x0119 )
					{
						SCROWBASIC_119 sCrowBasicOld;
						GASSERT(sizeof(sCrowBasicOld)==dwDataSize);
						SFile.ReadBuffer ( &sCrowBasicOld, sizeof(sCrowBasicOld) );

						m_sBasic = sCrowBasicOld;
					}
					else if ( dwDataVer == 0x0120 )
					{
						SCROWBASIC_120 sCrowBasicOld;
						GASSERT(sizeof(sCrowBasicOld)==dwDataSize);
						SFile.ReadBuffer ( &sCrowBasicOld, sizeof(sCrowBasicOld) );

						m_sBasic = sCrowBasicOld;
					}
                    else if ( dwDataVer == 0x0121 )
                    {
                        SCROWBASIC_121 sCrowBasicOld;
                        GASSERT(sizeof(sCrowBasicOld)==dwDataSize);
                        SFile.ReadBuffer ( &sCrowBasicOld, sizeof(sCrowBasicOld) );

                        m_sBasic = sCrowBasicOld;
                    }
					else if ( dwDataVer==SCROWBASIC::VERSION )
					{
						GASSERT(sizeof(SCROWBASIC)==dwDataSize);
						SFile.ReadBuffer ( &m_sBasic, sizeof(SCROWBASIC) );
					}
					else
					{
						MessageBox ( NULL, "SCROWBASIC unknown data version.", "ERROR", MB_OK );
						SFile.SetOffSet ( SFile.GetfTell() + dwDataSize );
					}
				}else{
					if ( dwDataVer==SCROWBASIC::VERSION )
					{
						GASSERT(sizeof(SCROWBASIC)==dwDataSize);
						SFile.ReadBuffer ( &m_sBasic, sizeof(SCROWBASIC) );
					}
					else
					{
						MessageBox ( NULL, "SCROWBASIC unknown data version.", "ERROR", MB_OK );
						SFile.SetOffSet ( SFile.GetfTell() + dwDataSize );
						return E_FAIL;
					}
				}
			}
			break;

		case FILE_SACTION:
			{
				SFile >> dwDataVer;
				SFile >> dwDataSize;

				if( dwDataVer==0x0100 )
				{
					SCROWACTION_100 sOldData;

					GASSERT(sizeof(sOldData)==dwDataSize);
					SFile.ReadBuffer ( &sOldData, sizeof(sOldData) );
				
					m_sAction = sOldData;
				}
				else if( dwDataVer==0x0101 )
				{
					SCROWACTION_101 sOldData;

					GASSERT(sizeof(sOldData)==dwDataSize);
					SFile.ReadBuffer ( &sOldData, sizeof(sOldData) );
				
					m_sAction = sOldData;
				}
				else if( dwDataVer==0x0102 )
				{
					SCROWACTION_102 sOldData;

					GASSERT(sizeof(sOldData)==dwDataSize);
					SFile.ReadBuffer ( &sOldData, sizeof(sOldData) );
				
					m_sAction = sOldData;
				}
				/*
				else if ( dwDataVer==0x0103 )
				{
					SCROWACTION_103 sOldData;

					GASSERT(sizeof(sOldData)==dwDataSize);
					SFile.ReadBuffer ( &sOldData, sizeof(sOldData) );
				
					m_sAction = sOldData;
				}
				*/
				else if( dwDataVer==0x0103 )
				{
					m_sAction.LOAD103 ( SFile );
				}
				else if( dwDataVer == 0x0104 )
				{
					m_sAction.LOAD104( SFile );
				}
				else if( dwDataVer == 0x0105 )
				{
					m_sAction.LOAD105( SFile );
				}
				else if( dwDataVer == 0x0106 )
				{
					m_sAction.LOAD106( SFile );
				}
				else if( dwDataVer == 0x0107 )
				{
					m_sAction.LOAD107( SFile );
				}
				else if( dwDataVer == 0x0108 )
				{
					m_sAction.LOAD108( SFile );
				}
				else if( dwDataVer == 0x0109 )
				{
					m_sAction.LOAD109( SFile );
				}
				else if( dwDataVer==SCROWACTION::VERSION )
				{
					m_sAction.LOAD ( SFile );
				}
				else
				{
					MessageBox ( NULL, "SCROWACTION unknown data version.", "ERROR", MB_OK );
					SFile.SetOffSet ( SFile.GetfTell() + dwDataSize );
				}
			}
			break;

		case FILE_SGEN:
			{
				SFile >> dwDataVer;
				SFile >> dwDataSize;
				if ( dwDataVer==0x0100 )
				{
					SCROWGEN_100 sOldData;
					GASSERT(sizeof(sOldData)==dwDataSize);
					SFile.ReadBuffer ( &sOldData, sizeof(sOldData) );
					
					m_sGenerate.Assign ( sOldData );
				}
				else if ( dwDataVer==0x0101 )
				{
					SCROWGEN_101 sOldData;
					GASSERT(sizeof(sOldData)==dwDataSize);
					SFile.ReadBuffer ( &sOldData, sizeof(sOldData) );
					
					m_sGenerate.Assign ( sOldData );
				}
				else if ( dwDataVer==0x0102 )
				{
					m_sGenerate.LOAD_0102( SFile );
				}
                else if ( dwDataVer==0x0103 )
                {
                    m_sGenerate.LOAD_0103( SFile );
                }
				else if ( dwDataVer==SCROWGEN::VERSION )
				{
					//mjeon.fix
					//GASSERT(sizeof(SCROWGEN)==dwDataSize);
					m_sGenerate.LOAD( SFile );
				}
				else
				{
					MessageBox ( NULL, "SCROWGEN unknown data version.", "ERROR", MB_OK );
					SFile.SetOffSet ( SFile.GetfTell() + dwDataSize );
				}
			}
			break;

		case FILE_SATTACK:
			{
				SFile >> dwDataVer;
				SFile >> dwDataSize;

				if ( dwDataVer == 0x0100 )
				{
					SCROWATTACK_100 sOLDATTACK;
					SCROWATTACK sCrowAttack;

					if ( m_vecCrowAttackPage.size() <= 0 )
					{
						GASSERT( 0 && "CrowAttackPage Size is Invalid." );
					}

					CROWATTACK_VEC& vecPageBasic = m_vecCrowAttackPage.at( 0 );
					for ( int i = 0; i < 3; ++i )
					{
						SFile.ReadBuffer ( &sOLDATTACK, sizeof(SCROWATTACK_100) );
						sCrowAttack = sOLDATTACK;
						m_vecCrowAttack.push_back( sCrowAttack );
						vecPageBasic.push_back( sCrowAttack );
					}					

				}
				else if ( dwDataVer == 0x0101 )
				{
					SCROWATTACK_101 sOLDATTACK;
					SCROWATTACK sCrowAttack;
					
					if ( m_vecCrowAttackPage.size() <= 0 )
					{
						GASSERT( 0 && "CrowAttackPage Size is Invalid." );

					}
					CROWATTACK_VEC& vecPageBasic = m_vecCrowAttackPage.at( 0 );
					for ( int i = 0; i < 3; ++i )
					{
						SFile.ReadBuffer ( &sOLDATTACK, sizeof(SCROWATTACK_101) );
						sCrowAttack = sOLDATTACK;
						m_vecCrowAttack.push_back( sCrowAttack );
						vecPageBasic.push_back( sCrowAttack );
					}

				}
				else if ( dwDataVer == 0x0102 )
				{
					SCROWATTACK_102 sOLDATTACK;
					SCROWATTACK sCrowAttack;
					
					if ( m_vecCrowAttackPage.size() <= 0 )
					{
						GASSERT( 0 && "CrowAttackPage Size is Invalid." );
					}

					CROWATTACK_VEC& vecPageBasic = m_vecCrowAttackPage.at( 0 );
					for ( int i = 0; i < 3; ++i )
					{
						SFile.ReadBuffer ( &sOLDATTACK, sizeof(SCROWATTACK_102) );
						sCrowAttack = sOLDATTACK;
						m_vecCrowAttack.push_back( sCrowAttack );
						vecPageBasic.push_back( sCrowAttack );
					}
				}
				else if ( dwDataVer==0x0103 )
				{
					SCROWATTACK_103 sOLDATTACK;
					SCROWATTACK sCrowAttack;
					
					if ( m_vecCrowAttackPage.size() <= 0 )
					{
						GASSERT( 0 && "CrowAttackPage Size is Invalid." );
					}

					CROWATTACK_VEC& vecPageBasic = m_vecCrowAttackPage.at( 0 );
					for ( int i = 0; i < 3; ++i )
					{
						SFile.ReadBuffer ( &sOLDATTACK, sizeof(SCROWATTACK_103) );
						sCrowAttack = sOLDATTACK;
						m_vecCrowAttack.push_back( sCrowAttack );
						vecPageBasic.push_back( sCrowAttack );
					}
				}
				else if ( dwDataVer==0x0104 )
				{
					SCROWATTACK_104 sOLDATTACK;
					SCROWATTACK sCrowAttack;
					
					if ( m_vecCrowAttackPage.size() <= 0 )
					{
						GASSERT( 0 && "CrowAttackPage Size is Invalid." );
					}

					CROWATTACK_VEC& vecPageBasic = m_vecCrowAttackPage.at( 0 );
					for ( int i = 0; i < 3; ++i )
					{
						SFile.ReadBuffer ( &sOLDATTACK, sizeof(SCROWATTACK_104) );
						sCrowAttack = sOLDATTACK;
						m_vecCrowAttack.push_back( sCrowAttack );
						vecPageBasic.push_back( sCrowAttack );
					}
				}
				else if ( dwDataVer==0x0105 )
				{
					SCROWATTACK sCrowAttack;

					if ( m_vecCrowAttackPage.size() <= 0 )
					{
						GASSERT( 0 && "CrowAttackPage Size is Invalid." );
					}

					CROWATTACK_VEC& vecPageBasic = m_vecCrowAttackPage.at( 0 );
					for ( int i = 0; i < 3; ++i )
					{
						sCrowAttack.LOAD_0105( SFile );
						m_vecCrowAttack.push_back( sCrowAttack );
						vecPageBasic.push_back( sCrowAttack );
					}
				}
				else if ( dwDataVer==0x0106 )
				{
					SCROWATTACK sCrowAttack;

					if ( m_vecCrowAttackPage.size() <= 0 )
					{
						GASSERT( 0 && "CrowAttackPage Size is Invalid." );
					}

					CROWATTACK_VEC& vecPageBasic = m_vecCrowAttackPage.at( 0 );
					for ( int i = 0; i < 3; ++i )
					{
						sCrowAttack.LOAD_0106_0108( SFile );
						m_vecCrowAttack.push_back( sCrowAttack );
						vecPageBasic.push_back( sCrowAttack );
					}
				}
				else if ( dwDataVer==0x0107 )
				{
					SCROWATTACK sCrowAttack;

					DWORD dwSize = 0;

					SFile >> dwSize;

					if ( m_vecCrowAttackPage.size() <= 0 )
					{
						GASSERT( 0 && "CrowAttackPage Size is Invalid." );
					}

					CROWATTACK_VEC& vecPageBasic = m_vecCrowAttackPage.at( 0 );
					for ( DWORD i = 0; i < dwSize; ++i )
					{
						sCrowAttack.LOAD_0106_0108( SFile );
						m_vecCrowAttack.push_back( sCrowAttack );
						vecPageBasic.push_back( sCrowAttack );
					}
				}
				else if ( 0x0108==dwDataVer )
				{
					SCROWATTACK sCrowAttack;

					DWORD dwSize = 0;
					DWORD dwPageIdx = 0;

					SFile >> dwSize;
					SFile >> dwPageIdx;

					if ( dwPageIdx < 0 || dwPageIdx >= m_vecCrowAttackPage.size() )
					{
						GASSERT( 0 && "CrowAttackPage Size is Invalid." );
					}

					CROWATTACK_VEC& vecPageBasic = m_vecCrowAttackPage.at( dwPageIdx );
					for ( DWORD i = 0; i < dwSize; ++i )
					{
						sCrowAttack.LOAD_0106_0108( SFile );
						if ( dwPageIdx == 0 )
							m_vecCrowAttack.push_back( sCrowAttack );
						vecPageBasic.push_back( sCrowAttack );
					}
				}
				else if ( SCROWATTACK::VERSION==dwDataVer )
				{
					SCROWATTACK sCrowAttack;

					DWORD dwSize = 0;
					DWORD dwPageIdx = 0;

					SFile >> dwSize;
					SFile >> dwPageIdx;

					if ( dwPageIdx < 0 || dwPageIdx >= m_vecCrowAttackPage.size() )
					{
						GASSERT( 0 && "CrowAttackPage Size is Invalid." );
					}

					CROWATTACK_VEC& vecPageBasic = m_vecCrowAttackPage.at( dwPageIdx );
					for ( DWORD i = 0; i < dwSize; ++i )
					{
						sCrowAttack.LOAD( SFile );
						if ( dwPageIdx == 0 )
							m_vecCrowAttack.push_back( sCrowAttack );
						vecPageBasic.push_back( sCrowAttack );
					}
				}
				else
				{
					MessageBox ( NULL, "SCROWATTACK unknown data version.", "ERROR", MB_OK );
					SFile.SetOffSet ( SFile.GetfTell() + dwDataSize );
				}
			}
			break;

		case FILE_SATTACK_EVENT:
			{
				SFile >> dwDataVer;
				SFile >> dwDataSize;

				if ( 0x0108==dwDataVer )
				{
					SCROWATTACK sCrowAttack;

					DWORD dwSize = 0;
					DWORD dwPageIdx = 0;

					SFile >> dwSize;
					SFile >> dwPageIdx;

					CROWATTACK_VEC& vecPageBasic = m_vecCrowAttackEventPage.at( dwPageIdx );
					for ( DWORD i = 0; i < dwSize; ++i )
					{
						sCrowAttack.LOAD_0106_0108( SFile );
						if ( dwPageIdx == 0 )
							m_vecCrowAttackEvent.push_back( sCrowAttack );
						vecPageBasic.push_back( sCrowAttack );
					}
				}
				else if ( SCROWATTACK::VERSION==dwDataVer )
				{
					SCROWATTACK sCrowAttack;

					DWORD dwSize = 0;
					DWORD dwPageIdx = 0;

					SFile >> dwSize;
					SFile >> dwPageIdx;

					CROWATTACK_VEC& vecPageBasic = m_vecCrowAttackEventPage.at( dwPageIdx );
					for ( DWORD i = 0; i < dwSize; ++i )
					{
						sCrowAttack.LOAD( SFile );
						if ( dwPageIdx == 0 )
							m_vecCrowAttackEvent.push_back( sCrowAttack );
						vecPageBasic.push_back( sCrowAttack );
					}
				}
				else
				{
					MessageBox ( NULL, "SCROWATTACK unknown data version.", "ERROR", MB_OK );
					SFile.SetOffSet ( SFile.GetfTell() + dwDataSize );
				}
			}
			break;
		};

		SFile >> dwDATATYPE;
	};

	//	Note : 만약 Crow가 판매 목록을 가지고 있다면 읽어온다.
	if ( GLItemMan::GetInstance().ValidTable() )
	{
		for ( int i=0; i<SCROWACTION::SALENUM; ++i )
		{
			//SaleInvenLoadFile ( m_sAction.m_strSaleFile[i].c_str(), m_sSaleItems[i], m_strSaleType[i], m_mapNpcSellPrice );

			GLInventorySale::CROW_SALE_TYPE_VERSION vTypeVer = GLInventorySale::CROW_SALE_NONE;

			switch( SaleInvenLoadFileVersion( m_sAction.m_strSaleFile[i].c_str(), i) )
			{
			case CROW_LOAD_SALE_SUCCESS     : vTypeVer = GLInventorySale::CROW_SALE_V_1; break;
			case CROW_LOAD_EXCHANGE_SUCCESS : vTypeVer = GLInventorySale::CROW_SALE_V_2; break;						

			case CROW_LOAD_ERROR1			: sc::writeLogError( sc::string::format("SaleInvenLoadFile %1% 파라메타 갯수 오류"     , m_sAction.m_strSaleFile[i].c_str() )); break;
			case CROW_LOAD_ERROR2			: sc::writeLogError( sc::string::format("SaleInvenLoadFile() %1% 아이템 파일 로드 실패", m_sAction.m_strSaleFile[i].c_str() )); break;
			case CROW_LOAD_ERROR3			: sc::writeLogError( sc::string::format("SaleInvenLoadFile %1% 위치 중복"			   , m_sAction.m_strSaleFile[i].c_str() )); break;
			case CROW_LOAD_ERROR4			: sc::writeLogError( sc::string::format("SaleInvenLoadFile() %1% cInventory의 파라메타의 갯수가 잘못되었습니다.", m_sAction.m_strSaleFile[i].c_str() )); break;
			case CROW_LOAD_ERROR5			: sc::writeLogError( sc::string::format("SaleInvenLoadFile %1%"						   , m_sAction.m_strSaleFile[i].c_str() )); break;
			case CROW_LOAD_ETC				: break;
			}

			m_sSaleItems[i].SetSaleFuncType( vTypeVer );
		}
	}



	//11-07-22 Item 관련은 모두 GenItem이 처리하도록 변경 (bjju)
#if defined( _ITEM_GEN_060417_H_ ) || defined( _QUESTION_ITEM_GEN_060417_H_ )
    BOOST_FOREACH( const std::string& strIter, m_sGenerate.m_vecGeneralGenItem )
        m_sGenItem.LOADFILE( strIter.c_str() );

    BOOST_FOREACH( const std::string& strIter, m_sGenerate.m_vecSpecialGenItem )
        m_sQtGenItem.LOADFILE( strIter.c_str() );
#else
	m_cGenItem.AddInfoGenMoney ( static_cast<float>(m_sGenerate.m_wGenMoney_Rate), m_sGenerate.m_dwGenMoney );
	m_cGenItem.AddInfoGenItem  ( static_cast<float>(m_sGenerate.m_wGenItem_Rate),  m_sGenerate.m_sGenItemID );

    BOOST_FOREACH( const std::string& strIter, m_sGenerate.m_vecGeneralGenItem )
	{
		SGENINFO sGenInfo;
        m_cGenItem.GetGenInfo( SFile.GetFileName(), strIter.c_str(), sGenInfo);
		m_cGenItem.AddGenInfo(sGenInfo);
	}

    BOOST_FOREACH( const std::string& strIter, m_sGenerate.m_vecSpecialGenItem )
	{
		SGENINFO sGenInfo;
        m_cGenItem.GetGenInfo( SFile.GetFileName(), strIter.c_str(), sGenInfo);
		m_cGenItem.AddGenInfo(sGenInfo);
	}
#endif


	if (bServer)
    {
        // 아래쪽에서 다시 Load 한다. 주석처리.2011-09-26 jgkim
        //LoadAniSet(m_sAction.m_strSkinObj.c_str());


        LoadAniSet(m_sAction.m_strSkinObj.c_str());
	
        std::tr1::shared_ptr<NpcTalkMan> spTalkMan = GLogicData::GetInstance().GetNpcTalkMan();
        if (spTalkMan)
        {
            std::tr1::shared_ptr<CNpcDialogueSet> spDlgSet = spTalkMan->Find(GetTalkFile());
            if (spDlgSet)
                m_spNpcTalkDlg = spDlgSet;
        }
	}
	return S_OK;
}

void SCROWDATA::LoadAniSet ( const char* szSkinObj )
{
	SAFE_DELETE_ARRAY(m_pANIMATION);
	m_pANIMATION = new VECANIATTACK[AN_TYPE_SIZE*AN_SUB_CROW_SIZE];

	if ( szSkinObj[0]=='\0' )	return;

	DxSkinDataDummy DataDummy;
	BOOL bOK = DataDummy.LoadFile ( szSkinObj );
	if ( !bOK )
	{
		CString StrMsg;
		StrMsg.Format ( "[%s] Skin File Load Fail.", szSkinObj );
		MessageBox ( NULL, StrMsg.GetString(), "ERROR", MB_OK );
		return;
	}

	size_t nSIZE = DataDummy.m_vecANIMINFO.size();
	for ( size_t n=0; n<nSIZE; ++n )
	{
		const SANIMCONINFO *pAnimConInfo = DataDummy.m_vecANIMINFO[n];

		SANIATTACK sAniAttack;
		sAniAttack.Assign ( pAnimConInfo );

		if ( pAnimConInfo->m_SubType >= AN_SUB_CROW_SIZE ) 
		{
            std::string ErrorMsg(
                sc::string::format(
                    "[%1%] Crow Animation Sub Ani Error [%2%][%3%]", szSkinObj, pAnimConInfo->m_MainType, pAnimConInfo->m_SubType));
			sc::writeLogError(ErrorMsg);
            MessageBox(NULL, ErrorMsg.c_str(), "ERROR", MB_OK);
			continue;
		}

		int nIndex = pAnimConInfo->m_MainType*AN_SUB_CROW_SIZE + pAnimConInfo->m_SubType;
		m_pANIMATION[nIndex].push_back ( sAniAttack );

        /*  현재 CharEdit 에서 수정한 사항이 반영이 안되는 문제가 있어서
            아래와 같이 최신 애니데이터를 가져다 쓰도록 수정을 하였으나 
            수정하면서 변경된 사항이 오히려 어색해지는 문제가 있어 일딴은 보류
            향후 기획팀과 하나씩 찾아서 수정하기로 함

        for (CROWATTACK_VEC_ITER it = m_vecCrowAttack.begin(); it != m_vecCrowAttack.end(); ++it)
        {
            if (it->strAniFile == pAnimConInfo->m_strCfgFile)
            {
                if (it->sAniAttack.m_dwSTime != sAniAttack.m_dwSTime)
                {
                    TCHAR temp[256] = {0};
                    _sntprintf_s(temp, 256, _T("(%02d, %02d) (%s) (%s) (애니 시작 시간 틀림) (old %.4f) (cur %.4f)\n"), m_sBasic.sNativeID.wMainID, m_sBasic.sNativeID.wSubID, szSkinObj, pAnimConInfo->m_strBinFile.c_str(), (float)it->sAniAttack.m_dwSTime / UNITANIKEY_PERSEC, (float)sAniAttack.m_dwSTime / UNITANIKEY_PERSEC);
                    OutputDebugStr(temp);
                }

                if (it->sAniAttack.m_dwETime != sAniAttack.m_dwETime)
                {
                    TCHAR temp[256] = {0};
                    _sntprintf_s(temp, 256, _T("(%02d, %02d) (%s) (%s) (애니 끝 시간 틀림) (old %.4f) (cur %.4f)\n"), m_sBasic.sNativeID.wMainID, m_sBasic.sNativeID.wSubID, szSkinObj, pAnimConInfo->m_strBinFile.c_str(), (float)it->sAniAttack.m_dwETime / UNITANIKEY_PERSEC, (float)sAniAttack.m_dwETime / UNITANIKEY_PERSEC);
                    OutputDebugStr(temp);
                }

                if (it->sAniAttack.m_UNITTIME != sAniAttack.m_UNITTIME)
                {
                    TCHAR temp[256] = {0};
                    _sntprintf_s(temp, 256, _T("(%02d, %02d) (%s) (%s) (m_UNITTIME) (old %.4f) (cur %.4f)\n"), m_sBasic.sNativeID.wMainID, m_sBasic.sNativeID.wSubID, szSkinObj, pAnimConInfo->m_strBinFile.c_str(), it->sAniAttack.m_UNITTIME, sAniAttack.m_UNITTIME);
                    OutputDebugStr(temp);
                }

                if (it->sAniAttack.m_dwFlags != sAniAttack.m_dwFlags)
                {
                    TCHAR temp[256] = {0};
                    _sntprintf_s(temp, 256, _T("(%02d, %02d) (%s) (%s) (애니 옵션 틀림) (old %d) (cur %d)\n"), m_sBasic.sNativeID.wMainID, m_sBasic.sNativeID.wSubID, szSkinObj, pAnimConInfo->m_strBinFile.c_str(), it->sAniAttack.m_dwFlags, sAniAttack.m_dwFlags);
                    OutputDebugStr(temp);
                }

                if (it->sAniAttack.m_MainType != sAniAttack.m_MainType)
                {
                    TCHAR temp[256] = {0};
                    _sntprintf_s(temp, 256, _T("(%02d, %02d) (%s) (%s) (애니 메인 타입 틀림) (old %d) (cur %d)\n"), m_sBasic.sNativeID.wMainID, m_sBasic.sNativeID.wSubID, szSkinObj, pAnimConInfo->m_strBinFile.c_str(), it->sAniAttack.m_MainType, sAniAttack.m_MainType);
                    OutputDebugStr(temp);
                }

                if (it->sAniAttack.m_SubType != sAniAttack.m_SubType)
                {
                    TCHAR temp[256] = {0};
                    _sntprintf_s(temp, 256, _T("(%02d, %02d) (%s) (%s) (애니 서브 타입 틀림) (old %d) (cur %d)\n"), m_sBasic.sNativeID.wMainID, m_sBasic.sNativeID.wSubID, szSkinObj, pAnimConInfo->m_strBinFile.c_str(), it->sAniAttack.m_SubType, sAniAttack.m_SubType);
                    OutputDebugStr(temp);
                }

                if (it->sAniAttack.m_wDivCount != sAniAttack.m_wDivCount)
                {
                    TCHAR temp[256] = {0};
                    _sntprintf_s(temp, 256, _T("(%02d, %02d) (%s) (%s) (데미지 갯수 틀림) (old %d) (cur %d)\n"), m_sBasic.sNativeID.wMainID, m_sBasic.sNativeID.wSubID, szSkinObj, pAnimConInfo->m_strBinFile.c_str(), it->sAniAttack.m_wDivCount, sAniAttack.m_wDivCount);
                    OutputDebugStr(temp);
                }

                it->sAniAttack = sAniAttack;
            }
        }
        */
	}
}

HRESULT SCROWDATA::SaveFile(sc::SerialFile& SFile)
{
	SFile << static_cast<DWORD> ( VERSION );

	SFile << static_cast<DWORD> ( FILE_SBASIC );
	{
		SFile << static_cast<DWORD> ( SCROWBASIC::VERSION );
		SFile << static_cast<DWORD> ( sizeof(SCROWBASIC) );

		CString cstrName;
		cstrName.Format( _T("CN_%03d_%03d"), sNativeID.wMainID, sNativeID.wSubID ); // by 경대
		StringCchCopy ( m_szName, CHAR_SZNAME, cstrName.GetString() );
		SFile.WriteBuffer ( &m_sBasic, sizeof(SCROWBASIC) );
	}

	SFile << static_cast<DWORD> ( FILE_SACTION );
	{
		SFile << static_cast<DWORD> ( SCROWACTION::VERSION );
		
		SFile.BeginBlock ();
		{
			m_sAction.SAVE ( SFile );
		}
		SFile.EndBlock ();
	}

	SFile << static_cast<DWORD> ( FILE_SGEN );
	{
		SFile << static_cast<DWORD> ( SCROWGEN::VERSION );
		SFile << static_cast<DWORD> ( sizeof(SCROWGEN) );
		m_sGenerate.SAVE(SFile);
	}

	for ( unsigned int i=0; i<EMMAXPAGE; ++i )
	{
		if ( i < 0 || i > m_vecCrowAttackPage.size() )
			continue;

		CROWATTACK_VEC vecTemp = m_vecCrowAttackPage.at( i );
		SFile << static_cast<DWORD> ( FILE_SATTACK );
		{
			SFile << static_cast<DWORD> ( SCROWATTACK::VERSION );
			SFile.BeginBlock();
			{
				DWORD dwSize = static_cast<DWORD>(vecTemp.size());
				SFile << dwSize;
				SFile << i;

				for ( DWORD j = 0; j < dwSize; ++j )
				{
					vecTemp[j].SAVE( SFile );
				}

			}
			SFile.EndBlock();
		}
	}

	for ( unsigned int i=0; i<EMMAXPAGE; ++i )
	{
		if ( i < 0 || i > m_vecCrowAttackPage.size() )
			continue;

		CROWATTACK_VEC vecTemp = m_vecCrowAttackEventPage.at( i );
		SFile << static_cast<DWORD> ( FILE_SATTACK_EVENT );
		{
			SFile << static_cast<DWORD> ( SCROWATTACK::VERSION );
			SFile.BeginBlock();
			{
				DWORD dwSize = static_cast<DWORD>(vecTemp.size());
				SFile << dwSize;
				SFile << i;

				for ( DWORD j = 0; j < dwSize; ++j )
				{
					vecTemp[j].SAVE( SFile );
				}

			}
			SFile.EndBlock();
		}
	}

	/*SFile << static_cast<DWORD> ( FILE_SATTACK );
	{
		SFile << static_cast<DWORD> ( SCROWATTACK::VERSION );
		SFile.BeginBlock();
		{
			DWORD dwSize = static_cast<DWORD>(m_vecCrowAttack.size());
			SFile << dwSize;

			for ( DWORD i = 0; i < dwSize; ++i )
			{
				m_vecCrowAttack[i].SAVE( SFile );
			}
			
		}
		SFile.EndBlock();
	}*/

	SFile << static_cast<DWORD> ( FILE_END_DATA );

	return S_OK;
}

VECANIATTACK& SCROWDATA::GetAnimation ( EMANI_MAINTYPE emMType, EMANI_SUBTYPE emSType )
{
	static VECANIATTACK nullANIMATION;
	if (!m_pANIMATION)
        return nullANIMATION;

	int nIndex = emMType*AN_SUB_CROW_SIZE + emSType;
	if (nIndex >= AN_TYPE_SIZE*AN_SUB_CROW_SIZE)
        return nullANIMATION;

	return m_pANIMATION[nIndex];
}

const char* SCROWDATA::GetName()
{
    if (GLOBAL_PARAM::bShowContentID)
        return m_szName;

	char* szName = NULL;
	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		szName = const_cast< char* >( CGameTextMan::GetInstance().GetText( m_szName, 0, CGameTextMan::EM_CROW_TEXT ).GetString() );
	else
		szName = const_cast< char* >( GLStringTable::GetInstance().GetString(m_szName, GLStringTable::CROW) );

	if (szName == NULL )
		return m_szName;
	// 몬스터 이름이 없는 경우도 있다;
	else if ( strcmp( szName, "Skip" ) == 0 || strcmp( szName, "skip" ) == 0 )
	{
		static TCHAR szEmpty[] = {""};
		return szEmpty;
	}
	else 
        return szName;
}

const std::string SCROWDATA::GetNameStr() const
{
    if (GLOBAL_PARAM::bShowContentID)
        return m_szName;

	const std::string RetStr = GLUseFeatures::GetInstance().IsUsingDataIntegrate() ? 
		CGameTextMan::GetInstance().GetText( m_szName, 0, CGameTextMan::EM_CROW_TEXT ).GetString():GLStringTable::GetInstance().GetCrowString(m_szName);

	if (!RetStr.empty() && strcmp( RetStr.c_str(), "Skip" ) != 0)
	{
		return RetStr;
	}
	else
	{
		return m_szName;
	}
}

VOID SCROWDATA::SaveCsvHead ( std::fstream &SFile )
{
	SCROWBASIC::SaveCsvHead( SFile );
	SCROWACTION::SaveCsvHead( SFile );
	SCROWGEN::SaveCsvHead( SFile );

	for ( unsigned int i=0; i<EMMAXPAGE; ++i )
	{
		for( int j=0; j<EMMAXATTACK; ++j )
		{
			SFile << j+1 << " bReal" << ",";
			SCROWATTACK::SaveCsvHead( SFile, j+1 );
		}
	}

	for ( unsigned int i=0; i<EMMAXPAGE; ++i )
	{
		for( int j=0; j<EMMAXATTACK; ++j )
		{
			SFile << j+1 << " bReal" << ",";
			SCROWATTACK::SaveCsvHead( SFile, j+1 );
		}
	}

	SFile << std::endl;
}

VOID SCROWDATA::SaveCsv ( std::fstream &SFile )
{
	m_sBasic.SaveCsv( SFile );
	m_sAction.SaveCsv( SFile );
	m_sGenerate.SaveCsv( SFile );

	for ( unsigned int i=0; i<EMMAXPAGE; ++i )
	{
		if ( i < 0 || i > m_vecCrowAttackPage.size() )
			continue;

		for( DWORD j=0; j<EMMAXATTACK; ++j )
		{
			SCROWATTACK sCrowAttack;
			BOOL bReal = FALSE;

			CROWATTACK_VEC vecTemp = m_vecCrowAttackPage.at( i );
			if ( j < vecTemp.size() )
			{
				sCrowAttack = vecTemp[j];
				bReal = TRUE;
			}

			SFile << bReal << ",";
			sCrowAttack.SaveCsv( SFile );
		}
	}

	for ( unsigned int i=0; i<EMMAXPAGE; ++i )
	{
		if ( i < 0 || i > m_vecCrowAttackEventPage.size() )
			continue;

		for( DWORD j=0; j<EMMAXATTACK; ++j )
		{
			SCROWATTACK sCrowAttack;
			BOOL bReal = FALSE;

			CROWATTACK_VEC vecTemp = m_vecCrowAttackEventPage.at( i );
			if ( j < vecTemp.size() )
			{
				sCrowAttack = vecTemp[j];
				bReal = TRUE;
			}

			SFile << bReal << ",";
			sCrowAttack.SaveCsv( SFile );
		}
	}

	SFile << std::endl;
}

VOID SCROWDATA::LoadCsv(CStringArray& StrArray)
{
    int iCsvCur = 0;
	m_sBasic.LoadCsv(StrArray, iCsvCur);
	m_sAction.LoadCsv(StrArray, iCsvCur);
	m_sGenerate.LoadCsv(StrArray, iCsvCur);
	
	m_vecCrowAttack.clear();
	m_vecCrowAttackEvent.clear();

	m_vecCrowAttackPage.clear();
	m_vecCrowAttackEventPage.clear();

	for ( unsigned int i=0; i<EMMAXPAGE; ++i )
	{
		m_vecCrowAttackPage.push_back( CROWATTACK_VEC() );
		CROWATTACK_VEC& vecTemp = m_vecCrowAttackPage.at( i );
		for ( int j=0; j<EMMAXATTACK; ++j )
		{
			SCROWATTACK sCrowAttack;

			BOOL bReal = FALSE;
			bReal = (BOOL)atol(StrArray[iCsvCur++]);
			sCrowAttack.LoadCsv(StrArray, iCsvCur);
			if (bReal)
			{
				vecTemp.push_back(sCrowAttack);
				if ( i == 0 )
					m_vecCrowAttack.push_back(sCrowAttack);
			}
		}
	}

	for ( unsigned int i=0; i<EMMAXPAGE; ++i )
	{
		m_vecCrowAttackEventPage.push_back( CROWATTACK_VEC() );
		CROWATTACK_VEC& vecTemp = m_vecCrowAttackEventPage.at( i );
		for ( int j=0; j<EMMAXATTACK; ++j )
		{
			SCROWATTACK sCrowAttack;

			BOOL bReal = FALSE;
			bReal = (BOOL)atol(StrArray[iCsvCur++]);
			sCrowAttack.LoadCsv(StrArray, iCsvCur);
			if (bReal)
			{
				vecTemp.push_back(sCrowAttack);

				if ( i == 0 )
					m_vecCrowAttackEvent.push_back(sCrowAttack);
			}
		}
	}
}

bool SCROWDATA::IsBasicTalk(int nAction) const
{
    if (m_spNpcTalkDlg)
        return m_spNpcTalkDlg->IsBasicTalk(nAction);
    else
        return false;
}

std::tr1::shared_ptr<SNpcTalk> SCROWDATA::GetTalk(DWORD dwTalkID) const
{
    if (m_spNpcTalkDlg)
    {
        return m_spNpcTalkDlg->GetTalk(dwTalkID);
    }
    else
    {
        std::tr1::shared_ptr<SNpcTalk> NullPtr;
        return NullPtr;
    }
}

int SCROWDATA::SaleInvenLoadFileVersion( const char* szFileName, int iArrayIdx )
{
	if( iArrayIdx >= SCROWACTION::SALENUM) 
		return CROW_LOAD_ETC;

	if (!szFileName)
		return CROW_LOAD_ETC;
	if (strlen(szFileName) == 0)
		return CROW_LOAD_ETC;
	
	std::string strPath;
	strPath = GLOGIC::GetPath();
	strPath += szFileName;	

	gltexfile glTextFile;
	glTextFile.reg_sep("\t ,[]");

	if (GLOGIC::UseLogicZipFile()) // by 경대
		glTextFile.SetZipFile(GLogicData::GetInstance().GetGlogicZipFile());

	if (!glTextFile.open(strPath, true, GLOGIC::UseLogicPackFile()))
	{		
		return CROW_LOAD_ERROR5;
	}

	WORD dwVERSION = 0;

	glTextFile.getflag ( "VERSION", 1, 1, dwVERSION, true );

	if( dwVERSION > 0 )
	{
		return m_fun_ptr_crow[CROW_SALE_V_INDEX(dwVERSION)](&glTextFile, m_sSaleItems[iArrayIdx], m_strSaleType[iArrayIdx], m_mapNpcSellPrice );
	}

	return CROW_LOAD_ETC;
}

namespace COMMENT
{
	std::string szCROWACT_UP[EMCROWACT_UP_NSIZE] =
	{
		"선제공격 하지 않음",
		"가장가까운 적 공격",
		"자신보다 낮은 레벨 공격",
		"자신보다 낮은 HP 공격",
		"빛 속성 공격",
		"어둠 속성 공격",
		"상태이상 공격",
		"무기들고 있는자 공격",
		"뛰고 있을때 공격",
		"공격받으면 도망간다",
	};
	
	std::string szCROWACT_DN[EMCROWACT_DN_NSIZE] =
	{
		"변함 없음",
		"낮은 HP를 찾는다",
		"공격 받으면 도망간다",
	};

	std::string NPC_TYPE [NPC_TYPE_SIZE] = 
	{
		"일반 NPC",
		"우체통",
		"인증기",
		"홀짝게임기",
		"텍사스홀덤",
        "클릭트리거 (인던)",
	};

};
