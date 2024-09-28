#include "../pch.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/String/StringUtils.h"
#include "../../SigmaCore/String/StringUtil.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/File/StringFile.h"
#include "../../SigmaCore/File/SerialFile.h"
#include "../../SigmaCore/Loading/NSLoadingDirect.h"
#include "../../SigmaCore/File/TxtFile.h"

#include "../../EngineLib/GlobalParam.h"
#include "../../EngineLib/DxTools/DebugSet.h"
#include "../../EngineLib/Common/SeqRandom.h"
#include "../../EngineLib/Common/SUBPATH.h"
#include "../../EngineLib/DxLand/DxLandMan.h"
#include "../../EngineLib/DxResponseMan.h"
#include "../../EngineLib/G-Logic/GLOGIC.h"
#include "../../EngineLib/G-Logic/GLPeriod.h"
#include "../../EngineLib/DxMeshs/SkinMesh/DxSkinDataDummy.h"

#include "../NpcTalk/NpcDialogueSet.h"
#include "../GLogicData.h"
#include "../GLogicDataMan.h"
#include "../GLStringTable.h"
#include "../../enginelib/GUInterface/GameTextControl.h"
#include "../GLUseFeatures.h"
#include "./SpecialEffect/GLSpecExperiencePoint.h"

#include "./GLSkillDef.h"
#include "./GLSkill_SubData.h"
#include "./GLSkill_Basic.h"
#include "./GLSkill_Apply.h"
#include "./GLSkill_Learn.h"
#include "./GLSkill_ExtData.h"
#include "./GLSkill_SpecialSkill.h"
#include "./GLSkill.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

GLSkillMan& GLSkillMan::GetInstance()
{
	static GLSkillMan Instance;
	return Instance;
}

const char* GLSkillMan::_FILEHEAD = "GLCROW";

namespace SKILL
{
	static std::string MakeNewIconFileName( const CString& strIconFileName, const SNATIVEID& sIconID )
	{
		static const char* szExt = ".png";

		int nExtPos = strIconFileName.ReverseFind( '.' );

		CString strFileName = strIconFileName.Left( nExtPos );

		CString strNewFileName;
		strNewFileName.Format( "%s_%d_%d%s", strFileName.GetBuffer(), sIconID.Sid(), sIconID.Mid(), szExt );

		return strNewFileName.GetBuffer();
	}
};

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
GLSKILL::GLSKILL(void)
:m_bMultiSkill (FALSE)
{
}

GLSKILL& GLSKILL::operator= (const GLSKILL& value)
{
	m_sBASIC		 = value.m_sBASIC;
	m_sAPPLY		 = value.m_sAPPLY;
	m_sLEARN		 = value.m_sLEARN;
	m_sEXT_DATA		 = value.m_sEXT_DATA;
	m_sSPECIAL_SKILL = value.m_sSPECIAL_SKILL;
	return *this;
}

const bool GLSKILL::HaveSpec(const EMSPEC_ADDON emSPEC)
{
	for ( std::vector<SKILL::SSPEC_ADDON>::const_iterator _iter(m_sAPPLY.vecSPEC.begin()); _iter != m_sAPPLY.vecSPEC.end(); ++_iter )
	{
		const SKILL::SSPEC_ADDON& specAddon = *_iter;
		if ( specAddon.emSPEC == emSPEC )
			return true;
	}

	return false;		
}

const bool GLSKILL::HaveSpec(const EMSPEC_ADDON emSPEC, SKILL::SSPEC_ADDON& resSpecAddon)
{
	for ( std::vector<SKILL::SSPEC_ADDON>::const_iterator _iter(m_sAPPLY.vecSPEC.begin()); _iter != m_sAPPLY.vecSPEC.end(); ++_iter )
	{
		const SKILL::SSPEC_ADDON& specAddon = *_iter;
		if ( specAddon.emSPEC == emSPEC )
		{
			resSpecAddon = specAddon;
			return true;
		}
	}
	return false;		
}

BOOL GLSKILL::SaveFile ( sc::SerialFile &SFile )
{
	SFile << DWORD(VERSION);

	SFile << (DWORD)FILE_SBASIC;
	{
		SFile << DWORD(SKILL::SBASIC::VERSION);
		SFile << (DWORD)sizeof(SKILL::SBASIC);

		CString cstrName;
		cstrName.Format( _T("SN_%03d_%03d"), m_sBASIC.sNATIVEID.wMainID, m_sBASIC.sNATIVEID.wSubID ); // by 경대
		StringCchCopy( m_sBASIC.szNAME, SKILL::MAX_SZNAME, cstrName.GetString() );
		SFile.WriteBuffer ( &m_sBASIC, sizeof(SKILL::SBASIC) );
	}

	SFile << (DWORD)FILE_SAPPLY;
	{
		SFile << DWORD(SKILL::SAPPLY::VERSION);
		SFile << (DWORD)sizeof(SKILL::SAPPLY);
		m_sAPPLY.SAVE( SFile );
	}

	SFile << (DWORD)FILE_SLEARN;
	{
		SFile << DWORD(SKILL::SLEARN::VERSION);
		SFile << (DWORD)sizeof(SKILL::SLEARN);
		SFile.WriteBuffer ( &m_sLEARN, sizeof(SKILL::SLEARN) );
	}

	SFile << (DWORD)FILE_SEXT_DATA;
	{
		SFile << DWORD(SKILL::SEXT_DATA::VERSION);
		SFile << (DWORD)sizeof(SKILL::SEXT_DATA);
		
		CString cstrName;
		cstrName.Format( _T("SD_%03d_%03d"), m_sBASIC.sNATIVEID.wMainID , m_sBASIC.sNATIVEID.wSubID ); // by 경대
		m_sEXT_DATA.strCOMMENTS = cstrName.GetString();
		m_sEXT_DATA.SAVE ( SFile );
	}

	SFile << (DWORD)FILE_SPECIAL_SKILL;
	{
		SFile << DWORD(SKILL::SSPECIAL_SKILL::VERSION);
		SFile << (DWORD)sizeof(SKILL::SSPECIAL_SKILL);
		m_sSPECIAL_SKILL.SAVE( SFile );
	}
	
	SFile << (DWORD)FILE_END_DATA;

	return TRUE;
}

BOOL GLSKILL::LoadFile(sc::BaseStream& SFile, bool bPastLoad)
{
	DWORD dwVersion;
	SFile >> dwVersion;
	GASSERT(dwVersion==VERSION);

	DWORD dwDataType;
	DWORD dwVer=0, dwSize=0;

	SFile >> dwDataType;
	while (dwDataType!=FILE_END_DATA)
	{
		switch (dwDataType)
		{
		case FILE_SBASIC:
			{
				SFile >> dwVer;
				SFile >> dwSize;

				if( bPastLoad )
				{
					switch ( dwVer )
					{
					case 0x0100:
						{
							GASSERT(sizeof(SKILL::SBASIC_100)==dwSize);
							SKILL::SBASIC_100 sOldData;
							SFile.ReadBuffer ( &sOldData, sizeof(SKILL::SBASIC_100) );
							m_sBASIC.Assign ( sOldData );
						}
						break;
					case 0x0101:
						{
							GASSERT(sizeof(SKILL::SBASIC_101)==dwSize);
							SKILL::SBASIC_101 sOldData;
							SFile.ReadBuffer ( &sOldData, sizeof(SKILL::SBASIC_101) );
							m_sBASIC.Assign ( sOldData );
						}
						break;
					case 0x0102:
						{
							GASSERT(sizeof(SKILL::SBASIC_102)==dwSize);
							SKILL::SBASIC_102 sOldData;
							SFile.ReadBuffer ( &sOldData, sizeof(SKILL::SBASIC_102) );
							m_sBASIC.Assign ( sOldData );
						}
						break;
					case 0x0103:
						{
							GASSERT(sizeof(SKILL::SBASIC_103)==dwSize);
							SKILL::SBASIC_103 sOldData;
							SFile.ReadBuffer ( &sOldData, sizeof(SKILL::SBASIC_103) );
							m_sBASIC.Assign ( sOldData );
						}
						break;
					case 0x0104:
						{
							GASSERT(sizeof(SKILL::SBASIC_105)==dwSize);
							SKILL::SBASIC_105 sOldData;
							SFile.ReadBuffer ( &m_sBASIC, sizeof(SKILL::SBASIC_105) );
							m_sBASIC.Assign ( sOldData );
						}
						break;
					case 0x0105:
						{
							GASSERT(sizeof(SKILL::SBASIC_105)==dwSize);
							SKILL::SBASIC_105 sOldData;
							SFile.ReadBuffer ( &sOldData, sizeof(SKILL::SBASIC_105) );
							m_sBASIC.Assign ( sOldData );
						}
						break;
					case 0x0106:
						{
							GASSERT(sizeof(SKILL::SBASIC_106)==dwSize);
							SKILL::SBASIC_106 sOldData;
							SFile.ReadBuffer ( &sOldData, sizeof(SKILL::SBASIC_106) );
							m_sBASIC.Assign ( sOldData );
						}
						break;
					case 0x0107:
						{
							GASSERT(sizeof(SKILL::SBASIC_107)==dwSize);
							SKILL::SBASIC_107 sOldData;
							SFile.ReadBuffer ( &sOldData, sizeof(SKILL::SBASIC_107) );
							m_sBASIC.Assign ( sOldData );
						}
						break;
					case 0x0108:
						{
							GASSERT(sizeof(SKILL::SBASIC_108)==dwSize);
							SKILL::SBASIC_108 sOldData;
							SFile.ReadBuffer ( &sOldData, sizeof(SKILL::SBASIC_108) );
							m_sBASIC.Assign ( sOldData );
						}
						break;
					case 0x0109:
						{
							GASSERT(sizeof(SKILL::SBASIC_109)==dwSize);
							SKILL::SBASIC_109 sOldData;
							SFile.ReadBuffer ( &sOldData, sizeof(SKILL::SBASIC_109) );
							m_sBASIC.Assign ( sOldData );
						}
						break;
					case 0x0110:
						{
							GASSERT(sizeof(SKILL::SBASIC_110)==dwSize);
							SKILL::SBASIC_110 sOldData;
							SFile.ReadBuffer ( &sOldData, sizeof(SKILL::SBASIC_110) );
							m_sBASIC.Assign ( sOldData );
						}
						break;
					case 0x0111:
						{
							GASSERT(sizeof(SKILL::SBASIC_111)==dwSize);
							SKILL::SBASIC_111 sOldData;
							SFile.ReadBuffer ( &sOldData, sizeof(SKILL::SBASIC_111) );
							m_sBASIC.Assign ( sOldData );
						}
						break;
					case SKILL::SBASIC_112::VERSION:
						{
							GASSERT(sizeof(SKILL::SBASIC_112)==dwSize);
							
							SKILL::SBASIC_112 sOldData;
							SFile.ReadBuffer ( &sOldData, sizeof(SKILL::SBASIC_112) );
							m_sBASIC.Assign ( sOldData );
						}
						break;
					case SKILL::SBASIC_113::VERSION:
						{
							GASSERT(sizeof(SKILL::SBASIC_113)==dwSize);
							SKILL::SBASIC_113 sOldData;
							SFile.ReadBuffer ( &sOldData, sizeof(SKILL::SBASIC_113) );
							m_sBASIC.Assign ( sOldData );
						}
						break;
					case SKILL::SBASIC_114::VERSION:
						{
							GASSERT(sizeof(SKILL::SBASIC_114)==dwSize);
							SKILL::SBASIC_114 sOldData;
							SFile.ReadBuffer ( &sOldData, sizeof(SKILL::SBASIC_114) );
							m_sBASIC.Assign ( sOldData );
						}
						break;
					case SKILL::SBASIC::VERSION:
						{
							GASSERT( sizeof( SKILL::SBASIC ) == dwSize );
							SFile.ReadBuffer( &m_sBASIC, sizeof( SKILL::SBASIC ) );
						}
						break;
					default:
						{
							MessageBox ( NULL, "SKILL::SBASIC unknown data version.", "ERROR", MB_OK );
							SFile.SetOffSet(SFile.GetfTell()+dwSize);
						}
						break;
					}
				}
				else
				{
					if( dwVer == SKILL::SBASIC::VERSION )
					{
						GASSERT( sizeof( SKILL::SBASIC ) == dwSize );
						SFile.ReadBuffer( &m_sBASIC, sizeof( SKILL::SBASIC ) );
					}
					else
					{
						MessageBox ( NULL, "SKILL::SBASIC unknown data version.", "ERROR", MB_OK );
						SFile.SetOffSet(SFile.GetfTell()+dwSize);
						return E_FAIL;
					}
				} //if..else
			} //case FILE_SBASIC
			break;
		case FILE_SAPPLY:
			{
				SFile >> dwVer;
				SFile >> dwSize;

				if ( dwVer==0x0100 )
				{
					SKILL::SAPPLY_100 sOldApply;
					GASSERT(0x0100==dwVer&&sizeof(SKILL::SAPPLY_100)==dwSize);

					SFile.ReadBuffer ( &sOldApply, sizeof(SKILL::SAPPLY_100) );
					m_sAPPLY.Assign ( sOldApply );
				}
				else if ( dwVer==0x0101 )
				{
					SKILL::SAPPLY_101 sOldApply;
					GASSERT(0x0101==dwVer&&sizeof(SKILL::SAPPLY_101)==dwSize);

					SFile.ReadBuffer ( &sOldApply, sizeof(SKILL::SAPPLY_101) );
					m_sAPPLY.Assign ( sOldApply );

				}
				else if ( dwVer==0x0102 )
				{
					SKILL::SAPPLY_102 sOldApply;
					GASSERT(0x0102==dwVer&&sizeof(SKILL::SAPPLY_102)==dwSize);

					SFile.ReadBuffer ( &sOldApply, sizeof(SKILL::SAPPLY_102) );
					m_sAPPLY.Assign ( sOldApply );

				}
				else if ( dwVer==0x0103 )
				{
					SKILL::SAPPLY_103 sOldApply;
					GASSERT(0x0103==dwVer&&sizeof(SKILL::SAPPLY_103)==dwSize);

					SFile.ReadBuffer ( &sOldApply, sizeof(SKILL::SAPPLY_103) );
					m_sAPPLY.Assign ( sOldApply );

				}
				else if ( dwVer==0x0104 )
				{
					SKILL::SAPPLY_104 sOldApply;
					GASSERT(0x0104==dwVer&&sizeof(SKILL::SAPPLY_104)==dwSize);

					SFile.ReadBuffer ( &sOldApply, sizeof(SKILL::SAPPLY_104) );
					m_sAPPLY.Assign ( sOldApply );

				}
				else if ( dwVer==0x0105 )
				{
					SKILL::SAPPLY_105 sOldApply;
					GASSERT(0x0105==dwVer&&sizeof(SKILL::SAPPLY_105)==dwSize);

					SFile.ReadBuffer ( &sOldApply, sizeof(SKILL::SAPPLY_105) );
					m_sAPPLY.Assign ( sOldApply );
				}
				else if ( dwVer==0x0106 )
					m_sAPPLY.LOAD_106 ( SFile );
				else if ( dwVer==0x0107 )
					m_sAPPLY.LOAD_107 ( SFile );
				else if ( dwVer==0x0108 )
					m_sAPPLY.LOAD_108 ( SFile );
				else if ( dwVer==0x0109 )
					m_sAPPLY.LOAD_109 ( SFile );
				else if ( dwVer==0x0110 )
					m_sAPPLY.LOAD_110 ( SFile );
				else if ( dwVer==0x0111 )
					m_sAPPLY.LOAD_111 ( SFile );
				else if ( dwVer==0x0112)
					m_sAPPLY.LOAD_112 ( SFile );
				else if ( dwVer==SKILL::SAPPLY::VERSION )
					m_sAPPLY.LOAD_113 ( SFile );
				else
				{
					GASSERT(0&&"SKILL::SAPPLY 를 읽어드릴 수 없는 버전입니다.");
					SFile.SetOffSet(SFile.GetfTell()+dwSize);
				}
			}
			break;
		case FILE_SLEARN:
			{
				SFile >> dwVer;
				SFile >> dwSize;

				if ( dwVer==0x0100 )
				{
					GASSERT(sizeof(SKILL::SLEARN_100)==dwSize);
					SKILL::SLEARN_100 sLEARN_OLD;
					SFile.ReadBuffer ( &sLEARN_OLD, sizeof(SKILL::SLEARN_100) );
					m_sLEARN.Assign ( sLEARN_OLD );
				}
				else if ( dwVer==0x0101 )
				{
					GASSERT(sizeof(SKILL::SLEARN_101)==dwSize);
					SKILL::SLEARN_101 sLEARN_OLD;
					SFile.ReadBuffer ( &sLEARN_OLD, sizeof(SKILL::SLEARN_101) );
					m_sLEARN.Assign ( sLEARN_OLD );
				}
				else if ( dwVer==0x0102 )
				{
					GASSERT(sizeof(SKILL::SLEARN_102)==dwSize);
					SKILL::SLEARN_102 sLEARN_OLD;
					SFile.ReadBuffer ( &sLEARN_OLD, sizeof(SKILL::SLEARN_102) );
					m_sLEARN.Assign ( sLEARN_OLD );
				}
				else if ( dwVer==0x0103 )
				{
					GASSERT(sizeof(SKILL::SLEARN_103)==dwSize);
					SKILL::SLEARN_103 sLEARN_OLD;
					SFile.ReadBuffer ( &sLEARN_OLD, sizeof(SKILL::SLEARN_103) );
					m_sLEARN.Assign ( sLEARN_OLD );
				}
				else if ( dwVer==0x0104 )
				{
					GASSERT(sizeof(SKILL::SLEARN_104)==dwSize);
					SKILL::SLEARN_104 sLEARN_OLD;
					SFile.ReadBuffer ( &sLEARN_OLD, sizeof(SKILL::SLEARN_104) );
					m_sLEARN.Assign ( sLEARN_OLD );
				}
				else if ( dwVer==SKILL::SLEARN::VERSION )
				{
					GASSERT(sizeof(SKILL::SLEARN)==dwSize);
					SFile.ReadBuffer ( &m_sLEARN, sizeof(SKILL::SLEARN) );
				}
				else
				{
					GASSERT(0&&"SKILL::SLEARN 를 읽어드릴 수 없는 버전입니다.");
					SFile.SetOffSet(SFile.GetfTell()+dwSize);
				}
			}
			break;
		case FILE_SEXT_DATA:
			{
				SFile >> dwVer;
				SFile >> dwSize;

				if ( dwVer==0x0100 )
				{
					SKILL::SEXT_DATA_100 sOLD_DATA;

					GASSERT(sizeof(SKILL::SEXT_DATA_100)==dwSize);
					SFile.ReadBuffer ( &sOLD_DATA, sizeof(SKILL::SEXT_DATA_100) );
					m_sEXT_DATA.Assign ( sOLD_DATA );
				}
				else if ( dwVer==0x0101 )
				{
					SKILL::SEXT_DATA_101 sOLD_DATA;

					GASSERT(sizeof(SKILL::SEXT_DATA_101)==dwSize);
					SFile.ReadBuffer ( &sOLD_DATA, sizeof(SKILL::SEXT_DATA_101) );
					m_sEXT_DATA.Assign ( sOLD_DATA );
				}
				else if ( dwVer==0x0102 )
				{
					SKILL::SEXT_DATA_102 sOLD_DATA;

					GASSERT(sizeof(SKILL::SEXT_DATA_102)==dwSize);
					SFile.ReadBuffer ( &sOLD_DATA, sizeof(SKILL::SEXT_DATA_102) );
					m_sEXT_DATA.Assign ( sOLD_DATA );
				}
				else if ( dwVer==0x0103 )
				{
					SKILL::SEXT_DATA_103 sOLD_DATA;
					GASSERT(sizeof(sOLD_DATA)==dwSize);
					sOLD_DATA.LOAD ( SFile );

					m_sEXT_DATA.Assign ( sOLD_DATA );
				}
				else if( dwVer==0x0104 )
				{
					SKILL::SEXT_DATA_104 sOLD_DATA;
					GASSERT(sizeof(sOLD_DATA)==dwSize);
					sOLD_DATA.LOAD ( SFile );

					m_sEXT_DATA.Assign ( sOLD_DATA );
				}
				else if( dwVer==0x0105 )
				{
					SKILL::SEXT_DATA_105 sOLD_DATA;
					GASSERT(sizeof(sOLD_DATA)==dwSize);
					sOLD_DATA.LOAD ( SFile );

					m_sEXT_DATA.Assign ( sOLD_DATA );
				}
				else if( dwVer==0x0106 )
				{
					SKILL::SEXT_DATA_106 sOLD_DATA;
					GASSERT(sizeof(sOLD_DATA)==dwSize);
					sOLD_DATA.LOAD ( SFile );

					m_sEXT_DATA.Assign ( sOLD_DATA );
				}
				else if( dwVer==0x0107 )
				{
					SKILL::SEXT_DATA_107 sOLD_DATA;
					GASSERT(sizeof(sOLD_DATA)==dwSize);
					sOLD_DATA.LOAD ( SFile );

					m_sEXT_DATA.Assign ( sOLD_DATA );
				}
				else if( dwVer == 0x0108 )
				{
					SKILL::SEXT_DATA_108 sOLD_DATA;
					GASSERT(sizeof(sOLD_DATA)==dwSize);
					sOLD_DATA.LOAD ( SFile );

					m_sEXT_DATA.Assign ( sOLD_DATA );
				}
				else if( dwVer == 0x0109 )
				{
					SKILL::SEXT_DATA_109 sOLD_DATA;
					GASSERT(sizeof(sOLD_DATA)==dwSize);
					sOLD_DATA.LOAD ( SFile );

					m_sEXT_DATA.Assign ( sOLD_DATA );
				}
				else if( dwVer == 0x0110 )
				{
					SKILL::SEXT_DATA_110 sOLD_DATA;
					GASSERT(sizeof(sOLD_DATA)==dwSize);
					sOLD_DATA.LOAD ( SFile );

					m_sEXT_DATA.Assign ( sOLD_DATA );
				}				
				else if( dwVer == 0x0111 )
				{
					SKILL::SEXT_DATA_111 sOLD_DATA;
					GASSERT(sizeof(sOLD_DATA)==dwSize);
					sOLD_DATA.LOAD ( SFile );

					m_sEXT_DATA.Assign ( sOLD_DATA );
				}
				else if( dwVer == SKILL::SEXT_DATA::VERSION )
				{
					m_sEXT_DATA.LOAD ( SFile );
				}
				else
				{
					GASSERT(0&&"SKILL::SEXT_DATA 를 읽어드릴 수 없는 버전입니다.");
					SFile.SetOffSet(SFile.GetfTell()+dwSize);
				}
			}
			break;
		case FILE_SPECIAL_SKILL:
			SFile >> dwVer;
			SFile >> dwSize;

			m_sSPECIAL_SKILL.ResetAll();

			if ( dwVer==0x0100 )
			{
				m_sSPECIAL_SKILL.LOAD_100 ( SFile );
			}
			else if ( dwVer==0x0101 )
			{
				m_sSPECIAL_SKILL.LOAD_101 ( SFile );
			}
			else if ( dwVer==0x0102 )
			{
				m_sSPECIAL_SKILL.LOAD_102 ( SFile );
			}
			else if ( dwVer==0x0103 )
			{
				m_sSPECIAL_SKILL.LOAD_103 ( SFile );
			}
			else if ( dwVer==0x0104 )
			{
				m_sSPECIAL_SKILL.LOAD_104 ( SFile );
			}
			else if ( dwVer==SKILL::SSPECIAL_SKILL::VERSION )
			{
				m_sSPECIAL_SKILL.LOAD ( SFile );
			}
			else
			{
				GASSERT(0&&"SKILL::SPECIAL_SKILL 를 읽어드릴 수 없는 버전입니다.");
				SFile.SetOffSet(SFile.GetfTell()+dwSize);
			}
			break;
		};

		SFile >> dwDataType;
	};

	return TRUE;
}

VOID GLSKILL::SaveCsvHead ( std::fstream &SFile )
{
	SKILL::SBASIC::SaveCsvHead( SFile );
	SKILL::SLEARN::SaveCsvHead( SFile );
	SKILL::SEXT_DATA::SaveCsvHead( SFile );
	SKILL::SSPECIAL_SKILL::SaveCsvHead( SFile );
	SKILL::SAPPLY::SaveCsvHead( SFile );
	SFile << "Anim M Cfg" << ",";
	SFile << "Anim M FStart" << ",";
	SFile << "Anim M FMax" << ",";
	SFile << "Anim M FEnd" << ",";
	SFile << "Anim M StrikeCount" << ",";
	SFile << "Anim M FStrikeEnd" << ",";
	SFile << "Anim W Cfg" << ",";
	SFile << "Anim W Start" << ",";
	SFile << "Anim W FMax" << ",";
	SFile << "Anim W FEnd" << ",";
	SFile << "Anim W StrikeCount" << ",";
	SFile << "Anim W FStrikeEnd" << ",";
	SFile << std::endl;
}

VOID GLSKILL::SaveCsv ( std::fstream &SFile )
{
	m_sBASIC.SaveCsv( SFile );
	m_sLEARN.SaveCsv( SFile );
	m_sEXT_DATA.SaveCsv( SFile );
	m_sSPECIAL_SKILL.SaveCsv( SFile );
	m_sAPPLY.SaveCsv( SFile );
	SaveCsvAnimIfo( SFile );
	SFile << std::endl;
}

VOID GLSKILL::SaveCsvAnimIfo ( std::fstream &SFile )
{
	EMCLASSINDEX emClassIndex = GLCONST_CHAR::GetClassInfoUsingSkill(m_sBASIC.GetId());
	if ( emClassIndex == GLCL_NUM_CLASS || emClassIndex == GLCL_ETC )
	{
		SFile << " , , , , , , , , , , , ,";
		return;
	}
	// Man
	{
		EMCHARINDEX emCharIndex = CharClassToIndex(CharSexandClassIndexToCharClass(1, emClassIndex));
		SANIMCONINFO* pAnimInfo = GLCONST_CHAR::cCONSTCLASS[emCharIndex].m_pSkinDataDummy->FindAniInfo(m_sEXT_DATA.emANIMTYPE, m_sEXT_DATA.emANISTYPE);
		if ( pAnimInfo != NULL )
		{
			SFile << pAnimInfo->m_strCfgFile.c_str() << ",";
			SFile << pAnimInfo->m_dwSTime/pAnimInfo->m_UNITTIME << ",";
			SFile << pAnimInfo->m_dwETime/pAnimInfo->m_UNITTIME << ",";
			SFile << pAnimInfo->m_dwETimeOrig/pAnimInfo->m_UNITTIME << ",";
			if ( pAnimInfo->m_wStrikeCount > 0 )
			{
				SFile << pAnimInfo->m_wStrikeCount << ",";
				SFile << pAnimInfo->m_sStrikeEff[pAnimInfo->m_wStrikeCount-1].m_dwFrame/pAnimInfo->m_UNITTIME << ",";
			}
			else
			{
				SFile << " , ,";
			}
		}
		else
		{
			SFile << " , , , , , ,";
		}
	}
	// Woman
	{
		EMCHARINDEX emCharIndex = CharClassToIndex(CharSexandClassIndexToCharClass(2, emClassIndex));
		SANIMCONINFO* pAnimInfo = GLCONST_CHAR::cCONSTCLASS[emCharIndex].m_pSkinDataDummy->FindAniInfo(m_sEXT_DATA.emANIMTYPE, m_sEXT_DATA.emANISTYPE);
		if ( pAnimInfo != NULL )
		{
			SFile << pAnimInfo->m_strCfgFile.c_str() << ",";
			SFile << pAnimInfo->m_dwSTime/pAnimInfo->m_UNITTIME << ",";
			SFile << pAnimInfo->m_dwETime/pAnimInfo->m_UNITTIME << ",";
			SFile << pAnimInfo->m_dwETimeOrig/pAnimInfo->m_UNITTIME << ",";
			if ( pAnimInfo->m_wStrikeCount > 0 )
			{
				SFile << pAnimInfo->m_wStrikeCount << ",";
				SFile << pAnimInfo->m_sStrikeEff[pAnimInfo->m_wStrikeCount-1].m_dwFrame/pAnimInfo->m_UNITTIME << ",";
			}
			else
			{
				SFile << " , ,";
			}
		}
		else
		{
			SFile << " , , , , , ,";
		}
	}
}

VOID GLSKILL::LoadCsv ( CStringArray &StrArray )
{
	//if( iLine )
	//{
		m_sBASIC.LoadCsv( StrArray );
		m_sLEARN.LoadCsv( StrArray );
		m_sEXT_DATA.LoadCsv( StrArray );
		m_sSPECIAL_SKILL.LoadCsv( StrArray );
	//}
	//else
	//{
		m_sAPPLY.LoadCsv( StrArray );
	//}
}

//	지속성 스킬 ( 쓰는곳 없음 ) 
bool GLSKILL::IsSkillFact ()
{
	switch ( m_sAPPLY.emBASIC_TYPE )
	{
	case SKILL::EMFOR_VARHP:
	case SKILL::EMFOR_VARMP:
	case SKILL::EMFOR_VARSP:
	case SKILL::EMFOR_DEFENSE:
	case SKILL::EMFOR_HITRATE:
	case SKILL::EMFOR_AVOIDRATE:
	case SKILL::EMFOR_VARAP:
	case SKILL::EMFOR_VARDAMAGE:
	case SKILL::EMFOR_VARDEFENSE:
	case SKILL::EMFOR_PA:
	case SKILL::EMFOR_SA:
	case SKILL::EMFOR_MA:
	case SKILL::EMFOR_HP_RATE:
	case SKILL::EMFOR_MP_RATE:
	case SKILL::EMFOR_SP_RATE:
	case SKILL::EMFOR_RESIST:
	case SKILL::EMFOR_SUMMON_ACTIVETIME:
	case SKILL::EMFOR_SUMMON_HIRE:
		return true;
	};

	if ( m_sAPPLY.vecADDON.size() != 0 )
	{
		return true;
	}

	for ( DWORD i = 0; i < m_sAPPLY.vecSPEC.size(); ++i )
	{
		switch ( m_sAPPLY.vecSPEC[i].emSPEC )
		{
		case EMSPECA_REFDAMAGE:
		case EMSPECA_NONBLOW:
		case EMSPECA_PIERCE:
		case EMSPECA_RANGE_PSY_LONG:
		case EMSPECA_MOVEVELO:
		case EMSPECA_INVISIBLE:
		case EMSPECA_RECVISIBLE:
		case EMSPECA_ATTACKVELO:
		case EMSPECA_SKILLDELAY:
		case EMSPECA_PSY_DAMAGE_REDUCE:
		case EMSPECA_MAGIC_DAMAGE_REDUCE:
		case EMSPECA_PSY_DAMAGE_REFLECTION:
		case EMSPECA_MAGIC_DAMAGE_REFLECTION:
		case EMSPECA_DEFENSE_SKILL_ACTIVE:
		case EMSPECA_DURATION_DAMAGE:
		case EMSPECA_CURSE:
		case EMSPECA_TARRANGE_ALL:
		case EMSPECA_APPLYRANGE_ALL:
		case EMSPECA_STUN:
		case EMSPECA_NON_DRUG:
		case EMSPECA_NON_SKILL:
		case EMSPECA_IGNORED_DAMAGE:
        case EMSPECA_VEHICLE_GETOFF: // 하차효과 봉주
        case EMSPECA_ENDURE_VARY:    // 지속시간 증감 봉주
        case EMSPECA_LINK_HP:        // 고리효과 봉주
		case EMSPECA_TAUNT:
		case EMSPECA_TURN_STATE_BATTLE:
		case EMSPECA_STIGMA:
		case EMSPECA_RELEASE_PET:
		case EMSPECA_DOMINATION:
		case EMSPECA_HALLUCINATE:
		case EMSPECA_RELEASE_ENDURE:
		case EMSPECA_VIEW_RANGE_RATE:
        case EMSPECA_MOVEVELO_LIMIT:
			return true;
		};
	}
	
	return false;
}

bool GLSKILL::CheckSpecialaAddon( EMSPEC_ADDON eAddon )	const
{
	for ( DWORD i = 0; i < m_sAPPLY.vecSPEC.size(); ++i )
	{
		if ( m_sAPPLY.vecSPEC[i].emSPEC == eAddon )
			return true;
	}
	return false;
} //GLSKILL::CheckSpecialaAddon

bool GLSKILL::IsRebirth()	const
{
	return CheckSpecialaAddon( EMSPECA_REBIRTH );
}

//
//mjeon.CaptureTheField
//
// 이로운 주문인가? (Heal도 포함)
//
bool GLSKILL::IsBuff()	const
{
	return (m_sBASIC.emACTION_TYPE == SKILL::EMACTION_TYPE_BUFF);
}
bool GLSKILL::IsLimit() const
{
	return (m_sBASIC.emACTION_TYPE == SKILL::EMACTION_TYPE_LIMIT);
}
bool GLSKILL::IsHarm() const
{
	if ( IsBuff() )
		return false;

	if( CheckSpecialaAddon( EMSPECA_REBIRTH )	||
		CheckSpecialaAddon( EMSPECA_TELEPORT )	||
		CheckSpecialaAddon( EMSPECA_SWAPPOS ) )
	{
		return false;
	} //if

	return true;
}

const char* GLSKILL::GetName()
{
    if (GLOBAL_PARAM::bShowContentID)
        return m_sBASIC.szNAME;

	char* szName = NULL;
	
	if (GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		szName = const_cast< char* >( CGameTextMan::GetInstance().GetText( m_sBASIC.szNAME, 0, CGameTextMan::EM_SKILL_TEXT ).GetString() );// _tcscpy( szName , CGameTextMan::GetInstance().GetText( m_sBASIC.szNAME, 0, CGameTextMan::EM_SKILL_TEXT ).GetString() );
	else 
		szName = const_cast< char* >( GLStringTable::GetInstance().GetString( m_sBASIC.szNAME, GLStringTable::SKILL ) );// _tcscpy( szName , GLStringTable::GetInstance().GetString( m_sBASIC.szNAME, GLStringTable::SKILL ) );
	
	if (szName == NULL || strcmp( szName, "Skip" ) == 0 )
        return m_sBASIC.szNAME;
    else
	    return szName;
}

const std::string GLSKILL::GetNameStr() const
{
    if (GLOBAL_PARAM::bShowContentID)
    {
        static std::string TempStr;
		TempStr = m_sBASIC.szNAME;
        return TempStr;
    }

//	std::string RetStr;
// 	if (GLUseFeatures::GetInstance().IsUsingPrivateDataIntegrate())
// 		RetStr = CGameTextMan::GetInstance().GetText( m_sBASIC.szNAME, 0, CGameTextMan::EM_SKILL_TEXT );
// 	else
// 		RetStr = GLStringTable::GetInstance().GetSkillString(m_sBASIC.szNAME);

	const std::string RetStr = GLUseFeatures::GetInstance().IsUsingDataIntegrate() ? 
		CGameTextMan::GetInstance().GetText( m_sBASIC.szNAME, 0, CGameTextMan::EM_SKILL_TEXT ).GetString():GLStringTable::GetInstance().GetSkillString(m_sBASIC.szNAME);

    if (!RetStr.empty() && strcmp( RetStr.c_str(), "Skip" ) != 0)
    {
        return RetStr;
    }
    else
    {
        static std::string TempStr(m_sBASIC.szNAME);
        return TempStr;
    }
}

const char* GLSKILL::GetDesc()
{
    if (GLOBAL_PARAM::bShowContentID)
        return m_sEXT_DATA.strCOMMENTS.c_str();

	// std::string szDesc;
	// char szDesc[255];
	char* szDesc = NULL;
	if (GLUseFeatures::GetInstance().IsUsingDataIntegrate())
		szDesc = const_cast< char* >( CGameTextMan::GetInstance().GetText( m_sEXT_DATA.strCOMMENTS, 0, CGameTextMan::EM_SKILL_TEXT ).GetString() );//_tcscpy( szDesc, CGameTextMan::GetInstance().GetText( m_sEXT_DATA.strCOMMENTS, 0, CGameTextMan::EM_SKILL_TEXT ).GetString() );
	else
		szDesc = const_cast< char* >( GLStringTable::GetInstance().GetString( m_sEXT_DATA.strCOMMENTS, GLStringTable::SKILL ) );// _tcscpy( szDesc, GLStringTable::GetInstance().GetString( m_sEXT_DATA.strCOMMENTS, GLStringTable::SKILL ) );

	if (szDesc == NULL || strcmp( szDesc, "Skip" ) == 0 || strcmp( szDesc, "skip" ) == 0 )
        return "";
    else
	    return szDesc;
}

namespace GLSKILL_UTIL
{
	template <typename T>
		const size_t INSERT_VA_LIST ( void* pVarArray[], T Data, size_t Offset )
	{
		memcpy( (pVarArray + Offset), &Data, sizeof(T) );
		return Offset + sizeof(T)/sizeof(void *);
	}

	template <>
		const size_t INSERT_VA_LIST ( void* pVarArray[], short Data, size_t Offset )
	{
		int Value = Data;
		memcpy( (pVarArray + Offset), &Value, sizeof(int) );
		return Offset + sizeof(int)/sizeof(void*);
	}

	template <>
		const size_t INSERT_VA_LIST ( void* pVarArray[], char Data, size_t Offset )
	{
		int Value = Data;
		memcpy( (pVarArray + Offset), &Value, sizeof(int) );
		return Offset + sizeof(int)/sizeof(void*);
	}

	template <>
		const size_t INSERT_VA_LIST ( void* pVarArray[], float Data, size_t Offset )
	{
		double Value = Data;
		memcpy( (pVarArray + Offset), &Value, sizeof(double) );
		return Offset + sizeof(double)/sizeof(void*);
	}
};

GLSKILL* GLSKILL::GetSecondSkill()
{
	if( m_sEXT_DATA.idMultiTargetSkill == NATIVEID_NULL() )
		return NULL;

	return GLSkillMan::GetInstance().GetData ( m_sEXT_DATA.idMultiTargetSkill.wMainID, 
											   m_sEXT_DATA.idMultiTargetSkill.wSubID );
} //GLSKILL::GetSecondSkill

void GLSKILL::GetDescFormatByLevel( CString& strDest, WORD wLevel, DWORD dwFormatFlag )
{
	if ( wLevel >= SKILL::MAX_LEVEL )
	{
		wLevel = SKILL::MAX_LEVEL - 1;
	}

	void*  VarList[1024];
	size_t VarCount    = 0;
	DOUBLE ValueDouble = 0.0f;
	char szDesc[255] = {0};
	
	 if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		_tcscpy( szDesc, CGameTextMan::GetInstance().GetText( m_sEXT_DATA.strCOMMENTS, 0, CGameTextMan::EM_SKILL_TEXT ) );
	 else
		_tcscpy( szDesc, GLStringTable::GetInstance().GetString(m_sEXT_DATA.strCOMMENTS, GLStringTable::SKILL) );

	if (!szDesc)
        return;

	// Note : Desc Format 기본 적용 값 읽음
	//
	if ( dwFormatFlag&DESC_FORMAT_BASIC )
	{
		FLOAT fValue = COMMENT::SKILL_TYPES_SCALE[m_sAPPLY.emBASIC_TYPE] * m_sAPPLY.sDATA_LVL[wLevel].fBASIC_VAR;
		VarCount = GLSKILL_UTIL::INSERT_VA_LIST( VarList, fValue, VarCount);
	}

	// Note : Desc Format blow 값 읽음
	//
	if ( dwFormatFlag&DESC_FORMAT_BLOW ) 
	{
		SKILL::SSTATE_BLOW* pBlow = &m_sAPPLY.sSTATE_BLOW[wLevel];

		if ( m_sAPPLY.emSTATE_BLOW != EMBLOW_NONE )
		{
			VarCount = GLSKILL_UTIL::INSERT_VA_LIST( VarList, pBlow->fRATE, VarCount);

			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
			{
				// 
				std::string strBLOW_VAR1 = CGameTextMan::GetInstance().GetCommentText("BLOW_VAR1", m_sAPPLY.emSTATE_BLOW).GetString();
				std::string strBLOW_VAR1_NONE = CGameTextMan::GetInstance().GetCommentText("BLOW_VAR1", EMBLOW_NONE).GetString();
				if ( strcmp( strBLOW_VAR1.c_str(), strBLOW_VAR1_NONE.c_str() ) )
				{
					FLOAT fValue = COMMENT::BLOW_VAR1_SCALE[m_sAPPLY.emSTATE_BLOW] * pBlow->fVAR1;
					VarCount = GLSKILL_UTIL::INSERT_VA_LIST( VarList, fValue, VarCount);
				}

				std::string strBLOW_VAR2 = CGameTextMan::GetInstance().GetCommentText("BLOW_VAR2", m_sAPPLY.emSTATE_BLOW).GetString();
				std::string strBLOW_VAR2_NONE = CGameTextMan::GetInstance().GetCommentText("BLOW_VAR2", EMBLOW_NONE).GetString();
				if ( strcmp( strBLOW_VAR2.c_str(), strBLOW_VAR2_NONE.c_str() ) )
				{
					FLOAT fValue = COMMENT::BLOW_VAR2_SCALE[m_sAPPLY.emSTATE_BLOW] * pBlow->fVAR2;
					VarCount = GLSKILL_UTIL::INSERT_VA_LIST( VarList, fValue, VarCount);
				}
			}
			else
			{
				if ( strcmp( COMMENT::BLOW_VAR1[m_sAPPLY.emSTATE_BLOW].c_str(), COMMENT::BLOW_VAR1[EMBLOW_NONE].c_str() ) )
				{
					FLOAT fValue = COMMENT::BLOW_VAR1_SCALE[m_sAPPLY.emSTATE_BLOW] * pBlow->fVAR1;
					VarCount = GLSKILL_UTIL::INSERT_VA_LIST( VarList, fValue, VarCount);
				}

				if ( strcmp( COMMENT::BLOW_VAR2[m_sAPPLY.emSTATE_BLOW].c_str(), COMMENT::BLOW_VAR2[EMBLOW_NONE].c_str() ) )
				{
					FLOAT fValue = COMMENT::BLOW_VAR2_SCALE[m_sAPPLY.emSTATE_BLOW] * pBlow->fVAR2;
					VarCount = GLSKILL_UTIL::INSERT_VA_LIST( VarList, fValue, VarCount);
				}
			}
		}
	}

	// Note : Desc Format Impact 값 읽음
	//
	if ( dwFormatFlag&DESC_FORMAT_IMPACT )
	{
		DWORD dwSize = (DWORD)m_sAPPLY.vecADDON.size();

		for ( DWORD i=0; i<dwSize; ++i )
		{
			SKILL::SIMPACT_ADDON* pAddon = &m_sAPPLY.vecADDON[i];

			if ( !pAddon )
				continue;

			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
			{				
				if ( strcmp( CGameTextMan::GetInstance().GetCommentText("IMPACT_ADDON", pAddon->emADDON).GetString(),
					CGameTextMan::GetInstance().GetCommentText("IMPACT_ADDON", EMIMPACTA_NONE).GetString() ) )
				{
					FLOAT fValue = COMMENT::IMPACT_ADDON_SCALE[pAddon->emADDON] * pAddon->fADDON_VAR[wLevel];
					VarCount = GLSKILL_UTIL::INSERT_VA_LIST( VarList, fValue, VarCount);	
				}
			}
			else
			{
				if ( strcmp( COMMENT::IMPACT_ADDON[pAddon->emADDON].c_str(), COMMENT::IMPACT_ADDON[EMIMPACTA_NONE].c_str() ) )
				{
					FLOAT fValue = COMMENT::IMPACT_ADDON_SCALE[pAddon->emADDON] * pAddon->fADDON_VAR[wLevel];
					VarCount = GLSKILL_UTIL::INSERT_VA_LIST( VarList, fValue, VarCount);	
				}
			}
		}
	}

	// Note : Desc Format Spec 값 읽음
	//
	if ( dwFormatFlag&DESC_FORMAT_SPEC )
	{
		DWORD dwSize = (DWORD)m_sAPPLY.vecSPEC.size();

		for ( DWORD i=0; i<dwSize; ++i )
		{
			SKILL::SSPEC_ADDON* pSpec = &m_sAPPLY.vecSPEC[i];

			if ( !pSpec )
				continue;

			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
			{
				if ( strcmp( 
					CGameTextMan::GetInstance().GetCommentText("SPEC_ADDON_VAR1", pSpec->emSPEC).GetString()
					, CGameTextMan::GetInstance().GetCommentText("SPEC_ADDON_VAR1",EMSPECA_NULL).GetString() ) )
				{
					FLOAT fValue = COMMENT::SPEC_ADDON_VAR1_SCALE[pSpec->emSPEC]* pSpec->sSPEC[wLevel].fVAR1;

					switch( pSpec->emSPEC )
					{
					case EMSPECA_ATTACKVELO :		// 공격속도
						{
							fValue *= -1.0f;
						}
						break;

					default :
						break;
					}

					VarCount = GLSKILL_UTIL::INSERT_VA_LIST( VarList, fValue, VarCount);
				}
				if ( strcmp( 
					CGameTextMan::GetInstance().GetCommentText("SPEC_ADDON_VAR2", pSpec->emSPEC).GetString(),
					CGameTextMan::GetInstance().GetCommentText("SPEC_ADDON_VAR2", EMSPECA_NULL).GetString()) )
				{
					FLOAT fValue = COMMENT::SPEC_ADDON_VAR2_SCALE[pSpec->emSPEC] * pSpec->sSPEC[wLevel].fVAR2;
					VarCount = GLSKILL_UTIL::INSERT_VA_LIST( VarList, fValue, VarCount);
				}
			}
			else
			{
				if ( strcmp( COMMENT::SPEC_ADDON_VAR1[pSpec->emSPEC].c_str(), COMMENT::SPEC_ADDON_VAR1[EMSPECA_NULL].c_str() ) )
				{
					FLOAT fValue = COMMENT::SPEC_ADDON_VAR1_SCALE[pSpec->emSPEC] * pSpec->sSPEC[wLevel].fVAR1;

					switch( pSpec->emSPEC )
					{
					case EMSPECA_ATTACKVELO :		// 공격속도
						{
							fValue *= -1.0f;
						}
						break;

					default :
						break;
					}

					VarCount = GLSKILL_UTIL::INSERT_VA_LIST( VarList, fValue, VarCount);
				}
				if ( strcmp( COMMENT::SPEC_ADDON_VAR2[pSpec->emSPEC].c_str(), COMMENT::SPEC_ADDON_VAR2[EMSPECA_NULL].c_str() ) )
				{
					FLOAT fValue = COMMENT::SPEC_ADDON_VAR2_SCALE[pSpec->emSPEC] * pSpec->sSPEC[wLevel].fVAR2;
					VarCount = GLSKILL_UTIL::INSERT_VA_LIST( VarList, fValue, VarCount);
				}
			}
		}
	}

	// Note : 포멧스트링 명령을 수행한다.
	//
	if ( VarCount )
	{
		va_list arg_ptr;
		va_start(arg_ptr, *(VarList-1));

		strDest.FormatV(szDesc, arg_ptr);

		va_end(arg_ptr);
	}
	else
	{
		strDest = szDesc;
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
GLSkillMan::GLSkillMan () 
	: m_pd3dDevice(NULL)
	, m_bModify(false)
{
	memset(m_szFileName, 0, sizeof(char) * (MAX_PATH));
}

GLSkillMan::~GLSkillMan ()
{
}

HRESULT GLSkillMan::OneTimeSceneInit ()
{		
	for ( int i=0; i<EMSKILLCLASS_NSIZE; i++ )
	for ( int j=0; j<MAX_CLASSSKILL; j++ )
	{
		m_pSkills[i][j] = NULL;
	}		

	return S_OK;
}

HRESULT GLSkillMan::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;

	return S_OK;
}

HRESULT GLSkillMan::DeleteDeviceObjects ()
{
	return S_OK;
}

HRESULT GLSkillMan::FinalCleanup ()
{
	for (int i=0; i<EMSKILLCLASS_NSIZE; ++i)
	    for (int j=0; j<MAX_CLASSSKILL; ++j)
		    SAFE_DELETE(m_pSkills[i][j]);
	return S_OK;
}

GLSKILL* GLSkillMan::GetData(WORD wClass, WORD Index)
{
	if (wClass>=EMSKILLCLASS_NSIZE || Index>=MAX_CLASSSKILL)
        return NULL;
    else
	    return m_pSkills[wClass][Index];
}

const std::string GLSkillMan::GetSkillName(DWORD SkillId) const
{
    return GetSkillName(SNATIVEID(SkillId));
}

const std::string GLSkillMan::GetSkillName(const SNATIVEID& SkillId) const
{
   return GetSkillName(SkillId.wMainID, SkillId.wSubID);
}

const std::string GLSkillMan::GetSkillName(WORD MainId, WORD SubId) const
{
    if (MainId >= EMSKILLCLASS_NSIZE || SubId >= MAX_CLASSSKILL)
    {
        static std::string TempStr1("");
        return TempStr1;
    }

    const GLSKILL* pSkillData = m_pSkills[MainId][SubId];
    if (pSkillData)
    {
        return pSkillData->GetNameStr();
    }
    else
    {
        static std::string TempStr2("");
        return TempStr2;
    }
}

void GLSkillMan::SetData ( WORD wMID, WORD Index, const PGLSKILL pSkillData, bool binner )
{
	GASSERT(wMID<EMSKILLCLASS_NSIZE);
	GASSERT(Index<MAX_CLASSSKILL);

	if ( wMID>=EMSKILLCLASS_NSIZE )
		return;
	if ( Index>=MAX_CLASSSKILL )
		return;

	SAFE_DELETE(m_pSkills[wMID][Index]);

	m_pSkills[wMID][Index] = pSkillData;

	switch ( static_cast<EMSKILLCLASS>(wMID) )
	{
	case EMSKILL_FIGHTER_01:
	case EMSKILL_FIGHTER_02:
	case EMSKILL_FIGHTER_03:
	case EMSKILL_FIGHTER_04:
		pSkillData->m_sLEARN.dwCLASS = GLCC_FIGHTER;
		break;

	case EMSKILL_ARMS_01:
	case EMSKILL_ARMS_02:
	case EMSKILL_ARMS_03:
	case EMSKILL_ARMS_04:
		pSkillData->m_sLEARN.dwCLASS = GLCC_ARMS;
		break;

	case EMSKILL_ARCHER_01:
	case EMSKILL_ARCHER_02:
	case EMSKILL_ARCHER_03:
	case EMSKILL_ARCHER_04:
		pSkillData->m_sLEARN.dwCLASS = GLCC_ARCHER;
		break;

	case EMSKILL_SPIRIT_01:
	case EMSKILL_SPIRIT_02:
	case EMSKILL_SPIRIT_03:
	case EMSKILL_SPIRIT_04:
		pSkillData->m_sLEARN.dwCLASS = GLCC_SPIRIT;
		break;

	case EMSKILL_EXTREME_01:
	case EMSKILL_EXTREME_02:
	case EMSKILL_EXTREME_03:
	case EMSKILL_EXTREME_04:
		pSkillData->m_sLEARN.dwCLASS = GLCC_EXTREME;
		break;

	case EMSKILL_SCIENTIST_01:
	case EMSKILL_SCIENTIST_02:
	case EMSKILL_SCIENTIST_03:
	case EMSKILL_SCIENTIST_04:
		pSkillData->m_sLEARN.dwCLASS = GLCC_SCIENTIST;
		break;

    case EMSKILL_ASSASSIN_01:
    case EMSKILL_ASSASSIN_02:
    case EMSKILL_ASSASSIN_03:
    case EMSKILL_ASSASSIN_04:
        pSkillData->m_sLEARN.dwCLASS = GLCC_ASSASSIN;
        break;

	case EMSKILL_TRICKER_01:
	case EMSKILL_TRICKER_02:
	case EMSKILL_TRICKER_03:
	case EMSKILL_TRICKER_04:
		pSkillData->m_sLEARN.dwCLASS = GLCC_TRICKER;
		break;
    case EMSKILL_ETC_01:
    case EMSKILL_ETC_02:
    case EMSKILL_ETC_03:
    case EMSKILL_ETC_04:
        pSkillData->m_sLEARN.dwCLASS = GLCC_ETC;
        break;
	case EMSKILL_ACTOR_01:
	case EMSKILL_ACTOR_02:
	case EMSKILL_ACTOR_03:
	case EMSKILL_ACTOR_04:
	case EMSKILL_ACTOR_05:
	case EMSKILL_ACTOR_06:
	case EMSKILL_ACTOR_07:
	case EMSKILL_ACTOR_08:
		pSkillData->m_sLEARN.dwCLASS = GLCC_ACTOR;
		break;

	default:
		pSkillData->m_sLEARN.dwCLASS = GLCC_NONE;
		break;
	};

	if ( !binner )
		m_bModify = true;
}

HRESULT GLSkillMan::LoadFile(const std::string& FileName, bool bPastLoad)
{
	StringCchCopy(m_szFileName, MAX_PATH, FileName.c_str());

    std::string FullPath(GLOGIC::GetPath());
	FullPath.append(FileName);

	std::auto_ptr<sc::BaseStream> pBStream(
        GLOGIC::openfile_basestream(
            GLOGIC::UseLogicZipFile(),
            GLogicData::GetInstance().GetGlogicZipFile(),
            FullPath.c_str(),
            FileName,
            false,
            GLOGIC::UseLogicPackFile()));

	if (!pBStream.get())
    {
        sc::writeLogError(
            sc::string::format(
                "GLSkillMan::LoadFile %1% %2%",
                FullPath,
                FileName));
		return E_FAIL;
    }

	sc::BaseStream& SFile = *pBStream;

	DWORD dwFILEVER;
	char szFILETYPE[_MAX_FNAME];
	SFile.GetFileType( szFILETYPE, _MAX_FNAME, dwFILEVER );
	SFile.SetEncode ( true );

	DWORD dwSkillNum;
	SFile >> dwSkillNum;

	for (DWORD i=0; i<dwSkillNum; ++i)
	{
        //---------------------------------------------------//
        NSLoadingDirect::FrameMoveRender( FALSE );
        //---------------------------------------------------//

		GLSKILL* pSkill = new GLSKILL;
		if (pSkill->LoadFile(SFile, bPastLoad) == E_FAIL)
        {
            sc::writeLogError(
                sc::string::format(
                    "GLSkillMan::LoadFile Skill Load %1% %2% %3%",
                    dwSkillNum,
                    FullPath,
                    FileName));
			return E_FAIL;
        }
		SetData(pSkill->m_sBASIC.sNATIVEID.wMainID, pSkill->m_sBASIC.sNATIVEID.wSubID, pSkill, true);
	}

	return S_OK;
}

HRESULT GLSkillMan::SaveFile ( const char* szFile )
{
	StringCchCopy(m_szFileName,MAX_PATH,szFile);

	char szFullPath[MAX_PATH] = {0};
	StringCchCopy ( szFullPath, MAX_PATH, GLOGIC::GetPath() );
	StringCchCat ( szFullPath, MAX_PATH, m_szFileName );

	sc::SerialFile SFile;

	SFile.SetFileType ( _FILEHEAD, VERSION );
	if ( !SFile.OpenFile ( FOT_WRITE, szFullPath ) )	return E_FAIL;
	SFile.SetEncode ( true );

	DWORD dwSkillNum = 0;
	for ( int i=0; i<EMSKILLCLASS_NSIZE; i++ )
	{
		for ( int j=0; j<MAX_CLASSSKILL; j++ )
		{
			if ( m_pSkills[i][j] )
				dwSkillNum++;
		}
	}

	SFile << dwSkillNum;

	for ( int i=0; i<EMSKILLCLASS_NSIZE; i++ )
	{
		for ( int j=0; j<MAX_CLASSSKILL; j++ )
		{
			if ( m_pSkills[i][j] )	
			{
				m_pSkills[i][j]->SaveFile ( SFile );
			
				// 데이터 통합 이후에는 지우자;
				SNATIVEID sNID = m_pSkills[i][j]->GetId();
				CString strKey;
				strKey.Format("SN_%03d_%03d", sNID.Mid(), sNID.Sid());
				if ( CGameTextMan::GetInstance().FindData(strKey.GetString(), CGameTextMan::EM_SKILL_TEXT) == FALSE )
					CGameTextMan::GetInstance().InsertString(strKey.GetString(), strKey.GetString(), CGameTextMan::EM_SKILL_TEXT);

				strKey.Format("SD_%03d_%03d", sNID.Mid(), sNID.Sid());
				if ( CGameTextMan::GetInstance().FindData(strKey.GetString(), CGameTextMan::EM_SKILL_TEXT) == FALSE )
					CGameTextMan::GetInstance().InsertString(strKey.GetString(), strKey.GetString(), CGameTextMan::EM_SKILL_TEXT);
			}
		}
	}

	m_bModify = false;
	return S_OK;
}

WORD GLSkillMan::FindFreeSkillIndex ( WORD wClass )
{
	GASSERT(wClass<EMSKILLCLASS_NSIZE);

	for ( WORD i=0; i<MAX_CLASSSKILL; i++ )
	{
		if ( !m_pSkills[wClass][i] )			return i;
	}

	return 0xFFFF;
}

BOOL GLSkillMan::DeleteSkill ( WORD wClass, WORD Index )
{
	GASSERT(wClass<EMSKILLCLASS_NSIZE);
	GASSERT(Index<MAX_CLASSSKILL);

	SAFE_DELETE(m_pSkills[wClass][Index]);

	m_bModify = true;
	return TRUE;
}

void GLSkillMan::GetMaxSkill ( WORD& wClass, WORD& rIndex )
{
	wClass = EMSKILLCLASS_NSIZE;
	rIndex = MAX_CLASSSKILL;
}

const char* GLSkillMan::_LOGFILE = "_CrowData_Synce.txt";
const char* GLSkillMan::_STRINGTABLE = "SkillStrTable.txt";

static CString	GetAppPath ()
{
	CString strFullPath;
	CString strCommandLine;

	TCHAR szPath[MAX_PATH] = {0};
	GetModuleFileName(::AfxGetInstanceHandle(), szPath, MAX_PATH);
	strCommandLine = szPath;

	if ( !strCommandLine.IsEmpty() )
	{
		DWORD dwFind = strCommandLine.ReverseFind ( '\\' );
		if ( dwFind != -1 )
		{
			strFullPath = strCommandLine.Left ( dwFind );
			
			if ( !strFullPath.IsEmpty() )
			{
				if ( strFullPath.GetAt(0) == '"' )
					strFullPath = strFullPath.Right ( strFullPath.GetLength() - 1 );
			}
		}
	}

	return strFullPath;
}

bool GLSkillMan::ValidData ()
{
	for ( int i = 0; i < EMSKILLCLASS_NSIZE; i++ )
	{
		for ( int j = 0; j < MAX_CLASSSKILL; j++ )
		{
			if ( m_pSkills[i][j] )
				return true;
		}
	}

	return false;
}

HRESULT GLSkillMan::SyncUpdateData (const SUBPATH* pPath)
{
	//	Note : 데이터 동기화 점검을 위한 초기화.
	//
	CString strAppPath;
	char szAppPath[MAX_PATH] = {0};
	char szFullPath[MAX_PATH] = {0};
	strAppPath = GetAppPath ();
	StringCchCopy ( szAppPath, MAX_PATH, strAppPath.GetString () );

	//	Note : DxSkeletonManager 기본 폴더 지정.
	//
	StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
	StringCchCat ( szFullPath, MAX_PATH, pPath->ObjFileSkeleton().c_str()); //SUBPATH::OBJ_FILE_SKELETON );
	DxSkeletonManager::GetInstance().SetPath ( szFullPath );

	// 디버그샛의 초기화
	CDebugSet::OneTimeSceneInit(pPath, false);

	//	Note : Animation 기본 폴더 지정.
	//
	StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
	StringCchCat ( szFullPath, MAX_PATH, pPath->ObjFileAnimation().c_str()); //SUBPATH::OBJ_FILE_ANIMATION );
	DxSkinAnimationManager::GetInstance().SetPath ( szFullPath );

	//	Note : SkinObject 기본 폴더 지정.
	//
	StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
	StringCchCat ( szFullPath, MAX_PATH, pPath->ObjFileSkinObject().c_str()); //SUBPATH::OBJ_FILE_SKINOBJECT );
	DxSkinObject::SetPath ( szFullPath );
	DxSkinCharDataContainer::GetInstance().SetPath ( szFullPath );
	DxAttBoneDataContainer::GetInstance().SetPath ( szFullPath );
	DxSkinPieceContainer::GetInstance().SetPath ( szFullPath );
	DxAttBoneLink::SetPath ( szFullPath );

	//	Note : GLogic
	//
	StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
	StringCchCat ( szFullPath, MAX_PATH, pPath->GLogicPath().c_str()); // SUBPATH::GLOGIC_FILE );
	GLOGIC::SetPath ( szFullPath );

	GLCONST_CHAR::LOADFILE(GLogicData::GetInstance().GetDefaultCharClassFileName());

	GLCONST_CHAR::cCONSTCLASS[GLCI_FIGHTER_M].LoadAniSet ( GLCONST_CHAR::szCharSkin[GLCI_FIGHTER_M], true );
	GLCONST_CHAR::cCONSTCLASS[GLCI_ARMS_M].LoadAniSet ( GLCONST_CHAR::szCharSkin[GLCI_ARMS_M], true );
	GLCONST_CHAR::cCONSTCLASS[GLCI_ARCHER_W].LoadAniSet ( GLCONST_CHAR::szCharSkin[GLCI_ARCHER_W], true );
	GLCONST_CHAR::cCONSTCLASS[GLCI_SPIRIT_W].LoadAniSet ( GLCONST_CHAR::szCharSkin[GLCI_SPIRIT_W], true );
	GLCONST_CHAR::cCONSTCLASS[GLCI_EXTREME_M].LoadAniSet ( GLCONST_CHAR::szCharSkin[GLCI_EXTREME_M], true );
	GLCONST_CHAR::cCONSTCLASS[GLCI_EXTREME_W].LoadAniSet ( GLCONST_CHAR::szCharSkin[GLCI_EXTREME_W], true );
	GLCONST_CHAR::cCONSTCLASS[GLCI_FIGHTER_W].LoadAniSet ( GLCONST_CHAR::szCharSkin[GLCI_FIGHTER_W], true );
	GLCONST_CHAR::cCONSTCLASS[GLCI_ARMS_W].LoadAniSet ( GLCONST_CHAR::szCharSkin[GLCI_ARMS_W], true );
	GLCONST_CHAR::cCONSTCLASS[GLCI_ARCHER_M].LoadAniSet ( GLCONST_CHAR::szCharSkin[GLCI_ARCHER_M], true );
	GLCONST_CHAR::cCONSTCLASS[GLCI_SPIRIT_M].LoadAniSet ( GLCONST_CHAR::szCharSkin[GLCI_SPIRIT_M], true );
	GLCONST_CHAR::cCONSTCLASS[GLCI_SCIENTIST_M].LoadAniSet ( GLCONST_CHAR::szCharSkin[GLCI_SCIENTIST_M], true );
	GLCONST_CHAR::cCONSTCLASS[GLCI_SCIENTIST_W].LoadAniSet ( GLCONST_CHAR::szCharSkin[GLCI_SCIENTIST_W], true );
    GLCONST_CHAR::cCONSTCLASS[GLCI_ASSASSIN_M].LoadAniSet ( GLCONST_CHAR::szCharSkin[GLCI_ASSASSIN_M], true );
    GLCONST_CHAR::cCONSTCLASS[GLCI_ASSASSIN_W].LoadAniSet ( GLCONST_CHAR::szCharSkin[GLCI_ASSASSIN_W], true );
	GLCONST_CHAR::cCONSTCLASS[GLCI_TRICKER_M].LoadAniSet ( GLCONST_CHAR::szCharSkin[GLCI_TRICKER_M], true );
	GLCONST_CHAR::cCONSTCLASS[GLCI_TRICKER_W].LoadAniSet ( GLCONST_CHAR::szCharSkin[GLCI_TRICKER_W], true );
    GLCONST_CHAR::cCONSTCLASS[GLCI_ETC_M].LoadAniSet ( GLCONST_CHAR::szCharSkin[GLCI_ETC_M], true );
    GLCONST_CHAR::cCONSTCLASS[GLCI_ETC_W].LoadAniSet ( GLCONST_CHAR::szCharSkin[GLCI_ETC_W], true );
	GLCONST_CHAR::cCONSTCLASS[GLCI_ACTOR_M].LoadAniSet ( GLCONST_CHAR::szCharSkin[GLCI_ACTOR_M], true );
	GLCONST_CHAR::cCONSTCLASS[GLCI_ACTOR_W].LoadAniSet ( GLCONST_CHAR::szCharSkin[GLCI_ACTOR_W], true );

	int nCHAR_FLAGS[GLCI_NUM_ACTOR] =
	{
		GLCC_FIGHTER_M,
		GLCC_ARMS_M,
		GLCC_ARCHER_W,
		GLCC_SPIRIT_W,
		GLCC_EXTREME_M,
		GLCC_EXTREME_W,
		GLCC_FIGHTER_W,
		GLCC_ARMS_W,
		GLCC_ARCHER_M,
		GLCC_SPIRIT_M,
		GLCC_SCIENTIST_M,
		GLCC_SCIENTIST_W,
        GLCC_ASSASSIN_M,
        GLCC_ASSASSIN_W,
		GLCC_TRICKER_M,
		GLCC_TRICKER_W,
        GLCC_ETC_M,
        GLCC_ETC_W,
		GLCC_ACTOR_M,
		GLCC_ACTOR_W,
	};

	CDebugSet::ClearFile ( _LOGFILE );

	for ( int i = 0; i < EMSKILLCLASS_NSIZE; i++ )
	{
		for ( int j = 0; j < MAX_CLASSSKILL; j++ )
		{
			if ( !m_pSkills[i][j] )
				continue;

			GLSKILL &sSKILL = *m_pSkills[i][j];

			for ( int k=0; k<SKILL::MAX_LEVEL; ++k )
			{
				if ( sSKILL.m_sAPPLY.sDATA_LVL[k].wAPPLYNUM==0 )
				{
					CDebugSet::ToFile ( _LOGFILE, "[%03d][%03d] 레벨(%d) 적용횟수가 0 입니다.", i, j, k );
				}

				if ( sSKILL.m_sAPPLY.sDATA_LVL[k].wTARNUM==0 )
				{
					CDebugSet::ToFile ( _LOGFILE, "[%03d][%03d] 레벨(%d) 타겟갯수가 0 입니다.", i, j, k );
				}
			}

			for ( int nClass=0; nClass<GLCI_NUM_ACTOR; ++nClass )
			{
				if ( sSKILL.m_sLEARN.dwCLASS & nCHAR_FLAGS[nClass] )
				{
					GLCONST_CHARCLASS &cCHARCLASS = GLCONST_CHAR::cCONSTCLASS[nClass];

					VECANIATTACK &sANILIST = cCHARCLASS.m_ANIMATION[sSKILL.m_sEXT_DATA.emANIMTYPE][sSKILL.m_sEXT_DATA.emANISTYPE];

					if ( sANILIST.empty() )
					{
						CDebugSet::ToFile ( _LOGFILE, "[%03d][%03d][%s] 지정된 '스킬모션'이 존제하지 않음 .", i, j, sSKILL.GetName() );
						continue;
					}

					SANIATTACK &sANI = sANILIST[0];

					if ( sANI.m_wDivCount==0 )
					{
						CDebugSet::ToFile ( _LOGFILE, "[%03d][%03d][%s] '스킬모션'에 '타격지점'이 없음.", i, j, sSKILL.GetName() );
					}
				}
			}
		}
	}

	CDebugSet::ToFile ( _LOGFILE, "---------------------sync-completion--------------------------------" );	

	return S_OK;
}

HRESULT GLSkillMan::SyncStringTable()
{
	char	szFullPathFileName[MAX_PATH] = "";
	StringCchCopy ( szFullPathFileName, MAX_PATH, GLOGIC::GetPath() );
	StringCchCat ( szFullPathFileName, MAX_PATH, _STRINGTABLE );

	//DeleteFile ( szFullPathFileName );

	CString strBuffer1, strBuffer;
	std::string strTemp;
	std::string::size_type idx;

	strBuffer = "// File : Skill String Table\r\n";
	strBuffer += "// Note : 키(ID)와 내용은 반드시 탭으로 구분되어야 합니다.\r\n//\r\n";
		
	for ( int i = 0; i < EMSKILLCLASS_NSIZE; i++ )
	{
		for ( int j = 0; j < MAX_CLASSSKILL; j++ )
		{
			if ( !m_pSkills[i][j] )		continue;
			const GLSKILL &sSKILL = *m_pSkills[i][j];

			if( sSKILL.m_sBASIC.szNAME[0] != _T('\0') )
			{
				strBuffer1.Format( _T("SN_%03d_%03d\t%s\r\n"), i, j, sSKILL.m_sBASIC.szNAME );
				strBuffer += strBuffer1;
			}

			if( !sSKILL.m_sEXT_DATA.strCOMMENTS.empty() )
			{
				strTemp = sSKILL.m_sEXT_DATA.strCOMMENTS;
				idx = strTemp.find( "\r\n" );
				while ( idx != std::string::npos )
				{
					strTemp.replace( idx, 2, " " );
					idx = strTemp.find( "\r\n" );
				}

				strBuffer1.Format( _T("SD_%03d_%03d\t%s\r\n"), i, j, strTemp.c_str() );
				strBuffer += strBuffer1;
			}
		}
	}

	CFile file;
	file.Open( _T(szFullPathFileName), CFile::modeCreate|CFile::modeWrite ); // 파일 열기
	file.Write( strBuffer.GetString(), strBuffer.GetLength()-2 ); // 파일 쓰기
	file.Close();

	//delete [] szEncode;
	return S_OK;
}

HRESULT GLSkillMan::SaveCsvFile ( CWnd* pWnd )
{
	CFileDialog dlg( FALSE, ".csv", NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		"csv file (*.csv)|*.csv|", pWnd );

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
	if ( dlg.DoModal() != IDOK )
		return E_FAIL;

	std::fstream streamFILE;
	streamFILE.open ( dlg.GetPathName().GetString(), std::ios_base::out );

    // Csv Head
    GLSKILL::SaveCsvHead( streamFILE );

	for( int i=0; i<EMSKILLCLASS_NSIZE; ++i )
	{
		for( int j=0; j<MAX_CLASSSKILL; ++j )
		{
			if( !m_pSkills[i][j] )
				continue;

			// Csv Data
			GLSKILL *pSKILL = m_pSkills[i][j];
			pSKILL->SaveCsv( streamFILE );
		}
	}

	streamFILE.close();

	return S_OK;
}

HRESULT GLSkillMan::LoadCsvFile ( CWnd* pWnd )
{
	CFileDialog dlg( TRUE, ".csv", NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		"csv file (*.csv)|*.csv|", pWnd );

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
	if ( dlg.DoModal() != IDOK )
		return E_FAIL;

	sc::CStringFile StrFile( 3072000 );
	if( !StrFile.Open ( dlg.GetPathName().GetString() ) )
		return E_FAIL;

    for (int i=0; i<EMSKILLCLASS_NSIZE; ++i)
    {
        for (int j=0; j<MAX_CLASSSKILL; ++j)
        {
            if (m_pSkills[i][j])
            {
                delete m_pSkills[i][j];
                m_pSkills[i][j] = NULL;
            }
        }
    }

	STRUTIL::ClearSeparator();
	STRUTIL::RegisterSeparator( "," );

	CString strLine;
	CStringArray StrArray;

	// Csv Head
    int iHead = 0;
    StrFile.GetNextLine( strLine );
    STRUTIL::StringSeparate( strLine, StrArray );
    iHead = (int)StrArray.GetCount();

    int iLine = 0;
	char szError[ 256 ];

	int iMID = EMSKILLCLASS_NSIZE;
	int iSID = MAX_CLASSSKILL;

	while( StrFile.GetNextLine( strLine ) )
	{
        iLine++;

        STRUTIL::StringSeparate( strLine, StrArray );

		int iCount = (int)StrArray.GetCount();
        if( iCount < iHead )
		{
			StringCchPrintf( szError, 256, "%d Line = include blank column, next item load failed", iLine );
			MessageBox( pWnd->GetSafeHwnd(), _T(szError), _T("Fail"), MB_OK );
			return E_FAIL;
		}

		iMID = atoi( StrArray[0] );
		iSID = atoi( StrArray[1] );

		if( iMID >= EMSKILLCLASS_NSIZE || iSID >= MAX_CLASSSKILL )
			continue;

		if( !m_pSkills[ iMID ][ iSID ] )
			m_pSkills[ iMID ][ iSID ] = new GLSKILL;

		// Csv Data
		GLSKILL *pSKILL = m_pSkills[ iMID ][ iSID ];
		pSKILL->LoadCsv( StrArray );
	}

	return S_OK;
}

std::string GLSkillMan::SaveSqlFile()
{
	if ( !m_pSkills )
		return std::string( "Skill data null" );

	std::string Path( sc::getAppPath() );
	Path.append( "\\SkillList.sql" );    

	sc::TxtFile SqlFile( Path, true, sc::TxtFile::ANSI );

	SqlFile.write( "DELETE SkillList" );
	SqlFile.write( "GO" );

	for ( int i = 0; i < EMSKILLCLASS_NSIZE; ++i )
	{
		for ( int j = 0; j < MAX_CLASSSKILL; ++j )
		{
			if ( !m_pSkills[i][j] )
				continue;

			GLSKILL *pSKILL = m_pSkills[i][j];
			std::string strSkillName( pSKILL->GetName() );
			sc::string::replaceForDb( strSkillName );
			SqlFile.write(
				sc::string::format(
				"INSERT INTO SkillList( SkillMain, SkillSub, SkillName, SkillGrade ) VALUES( %1%, %2%, '%3%', %4% )",
				pSKILL->GetId().wMainID,
				pSKILL->GetId().wSubID,
				strSkillName,
				pSKILL->m_sBASIC.dwGRADE ) );
		}
	}
	SqlFile.write("GO");
	std::string FileName = SqlFile.FileName();
	return FileName;
}