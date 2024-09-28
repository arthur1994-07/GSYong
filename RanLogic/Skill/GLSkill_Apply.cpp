#include "../pch.h"
#include "./GLSkillDef.h"
//#include "./GLSkill_ApplyData.h"
#include "./GLSkill_Apply.h"

namespace SKILL
{
	SAPPLY::SAPPLY()
		: emBASIC_TYPE(EMFOR_HP)
		, emELEMENT(EMELEMENT_SPIRIT)
		, dwCUREFLAG(NULL)
		, emSTATE_BLOW(EMBLOW_NONE)
		, nEffectFrame( 0 )
	{
	}

	void SAPPLY::Assign ( SAPPLY_100 &sOldData )
	{
		//	기본 적용값 설정.
		emBASIC_TYPE			= sOldData.emBASIC_TYPE;

		if ( sOldData.emELEMENT == EMELEMENT_OLDMAX ) emELEMENT = EMELEMENT_ARM;
		else emELEMENT				= sOldData.emELEMENT;
		dwCUREFLAG				= sOldData.dwCUREFLAG;

		for ( int i=0; i<MAX_LEVEL_BEFORE; ++i )	sDATA_LVL[i].Assign ( sOldData.sDATA_LVL[i] );

		//	상태이상 설정.
		emSTATE_BLOW			= sOldData.emSTATE_BLOW;
		memcpy ( sSTATE_BLOW, sOldData.sSTATE_BLOW, sizeof(SKILL::SSTATE_BLOW)*MAX_LEVEL_BEFORE );

		//	부가 효과 설정.
		SIMPACT_ADDON sAddon;
		sAddon.emADDON					= sOldData.emADDON;
		if ( sAddon.emADDON != EMIMPACTA_NONE )
		{
			memcpy ( sAddon.fADDON_VAR, sOldData.fADDON_VAR, sizeof(float)*MAX_LEVEL_BEFORE );
			vecADDON.push_back( sAddon );
		}

		//	특수기능 설정.
		SSPEC_ADDON sSpec;		
		sSpec.emSPEC					= sOldData.emSPEC;
		if ( sSpec.emSPEC != EMSPECA_NULL )
		{
			for ( int i = 0; i < MAX_LEVEL_BEFORE; ++i )
			{
				sSpec.sSPEC[i].Assign( sOldData.sSPEC[i] );
			}
			vecSPEC.push_back( sSpec );
		}


	}

	void SAPPLY::Assign ( SAPPLY_101 &sOldData )
	{
		//	기본 적용값 설정.
		emBASIC_TYPE			= sOldData.emBASIC_TYPE;
		if ( sOldData.emELEMENT == EMELEMENT_OLDMAX ) emELEMENT = EMELEMENT_ARM;
		else emELEMENT				= sOldData.emELEMENT;
		dwCUREFLAG				= sOldData.dwCUREFLAG;

		for ( int i=0; i<MAX_LEVEL_BEFORE; ++i )	sDATA_LVL[i].Assign ( sOldData.sDATA_LVL[i] );


		//	상태이상 설정.
		emSTATE_BLOW			= sOldData.emSTATE_BLOW;
		memcpy ( sSTATE_BLOW, sOldData.sSTATE_BLOW, sizeof(SKILL::SSTATE_BLOW)*MAX_LEVEL_BEFORE );

		//	부가 효과 설정.
		SIMPACT_ADDON sAddon;
		sAddon.emADDON					= sOldData.emADDON;
		if ( sAddon.emADDON != EMIMPACTA_NONE )
		{
			memcpy ( sAddon.fADDON_VAR, sOldData.fADDON_VAR, sizeof(float)*MAX_LEVEL_BEFORE );
			vecADDON.push_back( sAddon );
		}

		//	특수기능 설정.
		SSPEC_ADDON sSpec;		
		sSpec.emSPEC					= sOldData.emSPEC;
		if ( sSpec.emSPEC != EMSPECA_NULL )
		{
			for ( int i = 0; i < MAX_LEVEL_BEFORE; ++i )
			{
				sSpec.sSPEC[i].Assign( sOldData.sSPEC[i] );
			}		
			vecSPEC.push_back( sSpec );
		}
	}

	void SAPPLY::Assign ( SAPPLY_102 &sOldData )
	{
		//	기본 적용값 설정.
		emBASIC_TYPE			= sOldData.emBASIC_TYPE;
		emELEMENT				= sOldData.emELEMENT;
		dwCUREFLAG				= sOldData.dwCUREFLAG;

		for ( int i=0; i<MAX_LEVEL_BEFORE; ++i )	sDATA_LVL[i].Assign ( sOldData.sDATA_LVL[i] );

		//	상태이상 설정.
		emSTATE_BLOW			= sOldData.emSTATE_BLOW;
		memcpy ( sSTATE_BLOW, sOldData.sSTATE_BLOW, sizeof(SKILL::SSTATE_BLOW)*MAX_LEVEL_BEFORE );

		//	부가 효과 설정.
		SIMPACT_ADDON sAddon;
		sAddon.emADDON					= sOldData.emADDON;
		if ( sAddon.emADDON != EMIMPACTA_NONE )
		{
			memcpy ( sAddon.fADDON_VAR, sOldData.fADDON_VAR, sizeof(float)*MAX_LEVEL_BEFORE );
			vecADDON.push_back( sAddon );
		}

		//	특수기능 설정.
		SSPEC_ADDON sSpec;		
		sSpec.emSPEC					= sOldData.emSPEC;
		if ( sSpec.emSPEC != EMSPECA_NULL )
		{
			for ( int i = 0; i < MAX_LEVEL_BEFORE; ++i )
			{
				sSpec.sSPEC[i].Assign( sOldData.sSPEC[i] );
			}		
			vecSPEC.push_back( sSpec );
		}		
	}

	void SAPPLY::Assign ( SAPPLY_103 &sOldData )
	{
		//	기본 적용값 설정.
		emBASIC_TYPE			= sOldData.emBASIC_TYPE;
		emELEMENT				= sOldData.emELEMENT;
		dwCUREFLAG				= sOldData.dwCUREFLAG;

		for ( int i=0; i<MAX_LEVEL_BEFORE; ++i )	sDATA_LVL[i].Assign ( sOldData.sDATA_LVL[i] );

		//	상태이상 설정.
		emSTATE_BLOW			= sOldData.emSTATE_BLOW;
		memcpy ( sSTATE_BLOW, sOldData.sSTATE_BLOW, sizeof(SKILL::SSTATE_BLOW)*MAX_LEVEL_BEFORE );

		//	부가 효과 설정.
		SIMPACT_ADDON sAddon;
		sAddon.emADDON					= sOldData.emADDON;
		if ( sAddon.emADDON != EMIMPACTA_NONE )
		{
			memcpy ( sAddon.fADDON_VAR, sOldData.fADDON_VAR, sizeof(float)*MAX_LEVEL_BEFORE );
			vecADDON.push_back( sAddon );
		}

		//	특수기능 설정.
		SSPEC_ADDON sSpec;		
		sSpec.emSPEC					= sOldData.emSPEC;
		if ( sSpec.emSPEC != EMSPECA_NULL )
		{
			//memcpy ( sSpec.sSPEC, sOldData.sSPEC, sizeof(SKILL::SSPEC)*MAX_LEVEL_BEFORE );
			for ( int i = 0; i < MAX_LEVEL_BEFORE; ++i )
			{
				sSpec.sSPEC[i].Assign( sOldData.sSPEC[i] );
			}
			vecSPEC.push_back( sSpec );
		}
	}

	void SAPPLY::Assign ( SAPPLY_104 &sOldData )
	{
		//	기본 적용값 설정.
		emBASIC_TYPE			= sOldData.emBASIC_TYPE;
		emELEMENT				= sOldData.emELEMENT;
		dwCUREFLAG				= sOldData.dwCUREFLAG;

		for ( int i=0; i<MAX_LEVEL_BEFORE; ++i )	sDATA_LVL[i].Assign ( sOldData.sDATA_LVL[i] );


		//	상태이상 설정.
		emSTATE_BLOW			= sOldData.emSTATE_BLOW;
		memcpy ( sSTATE_BLOW, sOldData.sSTATE_BLOW, sizeof(SKILL::SSTATE_BLOW)*MAX_LEVEL_BEFORE );

		//	부가 효과 설정.
		SIMPACT_ADDON sAddon;
		sAddon.emADDON					= sOldData.emADDON;
		if ( sAddon.emADDON != EMIMPACTA_NONE )
		{
			memcpy ( sAddon.fADDON_VAR, sOldData.fADDON_VAR, sizeof(float)*MAX_LEVEL_BEFORE );
			vecADDON.push_back( sAddon );
		}

		//	특수기능 설정.
		SSPEC_ADDON sSpec;		
		sSpec.emSPEC					= sOldData.emSPEC;
		if ( sSpec.emSPEC != EMSPECA_NULL )
		{
			//memcpy ( sSpec.sSPEC, sOldData.sSPEC, sizeof(SKILL::SSPEC)*MAX_LEVEL_BEFORE );
			for ( int i = 0; i < MAX_LEVEL_BEFORE; ++i )
			{
				sSpec.sSPEC[i].Assign( sOldData.sSPEC[i] );
			}
			vecSPEC.push_back( sSpec );
		}
	}

	void SAPPLY::Assign ( SAPPLY_105 &sOldData )
	{
		//	기본 적용값 설정.
		emBASIC_TYPE			= sOldData.emBASIC_TYPE;
		emELEMENT				= sOldData.emELEMENT;
		dwCUREFLAG				= sOldData.dwCUREFLAG;

		for ( int i=0; i<MAX_LEVEL_BEFORE; ++i )	sDATA_LVL[i].Assign ( sOldData.sDATA_LVL[i] );

		//	상태이상 설정.
		emSTATE_BLOW			= sOldData.emSTATE_BLOW;
		memcpy ( sSTATE_BLOW, sOldData.sSTATE_BLOW, sizeof(SKILL::SSTATE_BLOW)*MAX_LEVEL_BEFORE );

		//	부가 효과 설정.
		SIMPACT_ADDON sAddon;
		sAddon.emADDON					= sOldData.emADDON;
		if ( sAddon.emADDON != EMIMPACTA_NONE )
		{
			memcpy ( sAddon.fADDON_VAR, sOldData.fADDON_VAR, sizeof(float)*MAX_LEVEL_BEFORE );
			vecADDON.push_back( sAddon );
		}

		//	특수기능 설정.
		SSPEC_ADDON sSpec;		
		sSpec.emSPEC					= sOldData.emSPEC;
		if ( sSpec.emSPEC != EMSPECA_NULL )
		{
			//memcpy ( sSpec.sSPEC, sOldData.sSPEC, sizeof(SKILL::SSPEC)*MAX_LEVEL_BEFORE );
			for ( int i = 0; i < MAX_LEVEL_BEFORE; ++i )
			{
				sSpec.sSPEC[i].Assign( sOldData.sSPEC[i] );
			}
			vecSPEC.push_back( sSpec );
		}
	}

	void SAPPLY::LOAD_106( sc::BaseStream &SFile )
	{
		DWORD dwSize, dwTemp;

		dwTemp;

		SFile >> dwTemp;
		emBASIC_TYPE = ( SKILL::EMTYPES )dwTemp;
		SFile >> dwTemp;
		emELEMENT = ( EMELEMENT ) dwTemp;
		SFile >> dwCUREFLAG;

		SKILL::CDATA_LVL_102 sOldDATA_LVL[MAX_LEVEL_BEFORE];

		SFile.ReadBuffer ( &sOldDATA_LVL, sizeof(SKILL::CDATA_LVL_102)* MAX_LEVEL_BEFORE );	
		for ( int i=0; i<MAX_LEVEL_BEFORE; ++i )	sDATA_LVL[i].Assign ( sOldDATA_LVL[i] );

		SFile >> dwTemp;
		emSTATE_BLOW = ( EMSTATE_BLOW ) dwTemp;
		SFile.ReadBuffer ( &sSTATE_BLOW, sizeof(SKILL::SSTATE_BLOW)*MAX_LEVEL_BEFORE );


		vecADDON.clear();
		SFile >> dwSize;
		for ( DWORD i = 0; i < dwSize; ++i )
		{
			SIMPACT_ADDON_100	sOldAddon;
			SIMPACT_ADDON		sAddon;

			SFile.ReadBuffer ( &sOldAddon, sizeof(SKILL::SIMPACT_ADDON_100) );
			sAddon.Assign( sOldAddon );
			vecADDON.push_back( sAddon );
		}

		vecSPEC.clear();
		SFile >> dwSize;
		for ( DWORD i = 0; i < dwSize; ++i )
		{
			SSPEC_ADDON_100	sOldSpec;
			SSPEC_ADDON		sSpec;

			SFile.ReadBuffer ( &sOldSpec, sizeof(SKILL::SSPEC_ADDON_100) );
			sSpec.Assign( sOldSpec );
			vecSPEC.push_back( sSpec );
		}
	}

	void SAPPLY::LOAD_107 ( sc::BaseStream &SFile )
	{
		DWORD dwSize, dwTemp;

		dwTemp;

		SFile >> dwTemp;
		emBASIC_TYPE = ( SKILL::EMTYPES )dwTemp;
		SFile >> dwTemp;
		emELEMENT = ( EMELEMENT ) dwTemp;
		SFile >> dwCUREFLAG;

		for ( DWORD i=0; i<MAX_LEVEL_BEFORE; ++i )
		{
			SKILL::CDATA_LVL_103 sData103;
			SFile.ReadBuffer ( &sData103, sizeof(SKILL::CDATA_LVL_103) );	
			sDATA_LVL[i].Assign(sData103);
		}

		SFile >> dwTemp;
		emSTATE_BLOW = ( EMSTATE_BLOW ) dwTemp;
		SFile.ReadBuffer ( &sSTATE_BLOW, sizeof(SKILL::SSTATE_BLOW)*MAX_LEVEL_BEFORE );


		vecADDON.clear();
		SFile >> dwSize;
		for ( DWORD i = 0; i < dwSize; ++i )
		{
			SIMPACT_ADDON_100	sOldAddon;
			SIMPACT_ADDON		sAddon;

			SFile.ReadBuffer ( &sOldAddon, sizeof(SKILL::SIMPACT_ADDON_100) );
			sAddon.Assign( sOldAddon );
			vecADDON.push_back( sAddon );
		}

		vecSPEC.clear();
		SFile >> dwSize;
		for ( DWORD i = 0; i < dwSize; ++i )
		{
			SSPEC_ADDON_100	sOldSpec;
			SSPEC_ADDON		sSpec;

			SFile.ReadBuffer ( &sOldSpec, sizeof(SKILL::SSPEC_ADDON_100) );
			sSpec.Assign( sOldSpec );
			vecSPEC.push_back( sSpec );
		}
	}

	void SAPPLY::LOAD_108 ( sc::BaseStream &SFile )
	{
		DWORD dwSize, dwTemp;

		dwTemp;

		SFile >> dwTemp;
		emBASIC_TYPE = ( SKILL::EMTYPES )dwTemp;
		SFile >> dwTemp;
		emELEMENT = ( EMELEMENT ) dwTemp;
		SFile >> dwCUREFLAG;

		SFile.ReadBuffer ( &sDATA_LVL, sizeof(SKILL::CDATA_LVL)* MAX_LEVEL_BEFORE );	

		SFile >> dwTemp;
		emSTATE_BLOW = ( EMSTATE_BLOW ) dwTemp;
		SFile.ReadBuffer ( &sSTATE_BLOW, sizeof(SKILL::SSTATE_BLOW)*MAX_LEVEL_BEFORE );


		vecADDON.clear();
		SFile >> dwSize;
		for ( DWORD i = 0; i < dwSize; ++i )
		{
			SIMPACT_ADDON_100	sOldAddon;
			SIMPACT_ADDON		sAddon;

			SFile.ReadBuffer ( &sOldAddon, sizeof(SKILL::SIMPACT_ADDON_100) );
			sAddon.Assign( sOldAddon );
			vecADDON.push_back( sAddon );
		}

		vecSPEC.clear();
		SFile >> dwSize;
		for ( DWORD i = 0; i < dwSize; ++i )
		{
			SSPEC_ADDON_100	sOldSpec;
			SSPEC_ADDON		sSpec;

			SFile.ReadBuffer ( &sOldSpec, sizeof(SKILL::SSPEC_ADDON_100) );
			sSpec.Assign( sOldSpec );
			vecSPEC.push_back( sSpec );
		}
	}

	void SAPPLY::LOAD_109 ( sc::BaseStream &SFile )
	{
		DWORD dwSize, dwTemp;

		dwTemp;

		SFile >> dwTemp;
		emBASIC_TYPE = ( SKILL::EMTYPES )dwTemp;
		SFile >> dwTemp;
		emELEMENT = ( EMELEMENT ) dwTemp;
		SFile >> dwCUREFLAG;

		SFile.ReadBuffer ( &sDATA_LVL, sizeof(SKILL::CDATA_LVL)* MAX_LEVEL_BEFORE );	

		SFile >> dwTemp;
		emSTATE_BLOW = ( EMSTATE_BLOW ) dwTemp;
		SFile.ReadBuffer ( &sSTATE_BLOW, sizeof(SKILL::SSTATE_BLOW)*MAX_LEVEL_BEFORE );


		vecADDON.clear();
		SFile >> dwSize;
		for ( DWORD i = 0; i < dwSize; ++i )
		{
			SIMPACT_ADDON_101 sOldAddon;
			SIMPACT_ADDON sAddon;
			SFile.ReadBuffer ( &sOldAddon, sizeof(SKILL::SIMPACT_ADDON_101) );
			sAddon.Assign(sOldAddon);
			vecADDON.push_back( sAddon );
		}

		vecSPEC.clear();
		SFile >> dwSize;
		for ( DWORD i = 0; i < dwSize; ++i )
		{
			SSPEC_ADDON sSpec;
			SFile.ReadBuffer ( &sSpec, sizeof(SKILL::SSPEC_ADDON) );			
			vecSPEC.push_back( sSpec );
		}
	}

	void SAPPLY::LOAD_110 ( sc::BaseStream &SFile )
	{
		DWORD dwSize, dwTemp;

		dwTemp;

		SFile >> dwTemp;
		emBASIC_TYPE = ( SKILL::EMTYPES )dwTemp;
		SFile >> dwTemp;
		emELEMENT = ( EMELEMENT ) dwTemp;
		SFile >> dwCUREFLAG;

		SFile.ReadBuffer ( &sDATA_LVL, sizeof(SKILL::CDATA_LVL)* MAX_LEVEL_BEFORE );	

		SFile >> dwTemp;
		emSTATE_BLOW = ( EMSTATE_BLOW ) dwTemp;
		SFile.ReadBuffer ( &sSTATE_BLOW, sizeof(SKILL::SSTATE_BLOW)*MAX_LEVEL_BEFORE );


		vecADDON.clear();
		SFile >> dwSize;
		for ( DWORD i = 0; i < dwSize; ++i )
		{
			SIMPACT_ADDON_101 sOldAddon;
			SIMPACT_ADDON sAddon;
			SFile.ReadBuffer ( &sOldAddon, sizeof(SKILL::SIMPACT_ADDON_101) );
			sAddon.Assign(sOldAddon);
			vecADDON.push_back( sAddon );
		}

		vecSPEC.clear();
		SFile >> dwSize;
		for ( DWORD i = 0; i < dwSize; ++i )
		{
			SSPEC_ADDON_101	sOldSpec;
			SSPEC_ADDON		sSpec;

			SFile.ReadBuffer ( &sOldSpec, sizeof(SKILL::SSPEC_ADDON_101) );
			sSpec.Assign( sOldSpec );	
			vecSPEC.push_back( sSpec );
		}
		SFile >> nEffectFrame;
	} //SAPPLY::LOAD_110

	void SAPPLY::LOAD_111 ( sc::BaseStream &SFile )
	{
		DWORD dwSize, dwTemp;

		dwTemp;

		SFile >> dwTemp;
		emBASIC_TYPE = ( SKILL::EMTYPES )dwTemp;
		SFile >> dwTemp;
		emELEMENT = ( EMELEMENT ) dwTemp;
		SFile >> dwCUREFLAG;

		SFile.ReadBuffer ( &sDATA_LVL, sizeof(SKILL::CDATA_LVL)* MAX_LEVEL_BEFORE );	

		SFile >> dwTemp;
		emSTATE_BLOW = ( EMSTATE_BLOW ) dwTemp;
		SFile.ReadBuffer ( &sSTATE_BLOW, sizeof(SKILL::SSTATE_BLOW)*MAX_LEVEL_BEFORE );


		vecADDON.clear();
		SFile >> dwSize;
		for ( DWORD i = 0; i < dwSize; ++i )
		{
			SIMPACT_ADDON_101 sOldAddon;
			SIMPACT_ADDON sAddon;
			SFile.ReadBuffer ( &sOldAddon, sizeof(SKILL::SIMPACT_ADDON_101) );
			sAddon.Assign(sOldAddon);
			vecADDON.push_back( sAddon );
		}

		vecSPEC.clear();
		SFile >> dwSize;
		for ( DWORD i = 0; i < dwSize; ++i )
		{
			SSPEC_ADDON_102	sOldSpec;
			SSPEC_ADDON		sSpec;

			SFile.ReadBuffer ( &sOldSpec, sizeof(SKILL::SSPEC_ADDON_102) );
			sSpec.Assign( sOldSpec );	
			vecSPEC.push_back( sSpec );
		}

		SFile >> nEffectFrame;
	} //SAPPLY::LOAD_110

	void SAPPLY::LOAD_112 ( sc::BaseStream &SFile )
	{
		DWORD dwSize, dwTemp;

		dwTemp;

		SFile >> dwTemp;
		emBASIC_TYPE = ( SKILL::EMTYPES )dwTemp;
		SFile >> dwTemp;
		emELEMENT = ( EMELEMENT ) dwTemp;
		SFile >> dwCUREFLAG;

		SFile.ReadBuffer ( &sDATA_LVL, sizeof(SKILL::CDATA_LVL)* MAX_LEVEL_BEFORE );	

		SFile >> dwTemp;
		emSTATE_BLOW = ( EMSTATE_BLOW ) dwTemp;
		SFile.ReadBuffer ( &sSTATE_BLOW, sizeof(SKILL::SSTATE_BLOW)*MAX_LEVEL_BEFORE );


		vecADDON.clear();
		SFile >> dwSize;
		for ( DWORD i = 0; i < dwSize; ++i )
		{
			SIMPACT_ADDON_101 sOldAddon;
			SIMPACT_ADDON sAddon;
			SFile.ReadBuffer ( &sOldAddon, sizeof(SKILL::SIMPACT_ADDON_101) );
			sAddon.Assign(sOldAddon);
			vecADDON.push_back( sAddon );
		}

		vecSPEC.clear();
		SFile >> dwSize;

		bool bCA = false;
		for ( DWORD i = 0; i < dwSize; ++i )
		{
			SSPEC_ADDON_102	sOldSpec;
			SSPEC_ADDON		sSpec;

			SFile.ReadBuffer ( &sOldSpec, sizeof(SKILL::SSPEC_ADDON_102) );
			sSpec.Assign( sOldSpec );	
			vecSPEC.push_back( sSpec );

			if( sSpec.emSPEC == EMSPECA_COUNTATTACK ) bCA = true;
		}

		SFile >> nEffectFrame;

		if( bCA ) {
			SKILL::SSPEC_CA_ADDON_100 sAdon;
			SFile.ReadBuffer ( &sAdon, sizeof(SKILL::SSPEC_CA_ADDON_100) );
			sCAData.Assign( sAdon );
		}
	}

	void SAPPLY::LOAD_113 ( sc::BaseStream &SFile )
	{
		DWORD dwSize, dwTemp;

		dwTemp;

		SFile >> dwTemp;
		emBASIC_TYPE = ( SKILL::EMTYPES )dwTemp;
		SFile >> dwTemp;
		emELEMENT = ( EMELEMENT ) dwTemp;
		SFile >> dwCUREFLAG;

		SFile.ReadBuffer ( &sDATA_LVL, sizeof(SKILL::CDATA_LVL)* MAX_LEVEL );	

		SFile >> dwTemp;
		emSTATE_BLOW = ( EMSTATE_BLOW ) dwTemp;
		SFile.ReadBuffer ( &sSTATE_BLOW, sizeof(SKILL::SSTATE_BLOW)*MAX_LEVEL );


		vecADDON.clear();
		SFile >> dwSize;
		for ( DWORD i = 0; i < dwSize; ++i )
		{
			SIMPACT_ADDON sAddon;
			SFile.ReadBuffer ( &sAddon, sizeof(SKILL::SIMPACT_ADDON) );
			vecADDON.push_back( sAddon );
		}

		vecSPEC.clear();
		SFile >> dwSize;

		bool bCA = false;
		for ( DWORD i = 0; i < dwSize; ++i )
		{
			SSPEC_ADDON sSpec;
			SFile.ReadBuffer ( &sSpec, sizeof(SKILL::SSPEC_ADDON) );			
			vecSPEC.push_back( sSpec );

			if( sSpec.emSPEC == EMSPECA_COUNTATTACK ) bCA = true;
		}

		SFile >> nEffectFrame;

		if( bCA ) {
			SFile.ReadBuffer ( &sCAData, sizeof(SKILL::SSPEC_CA_ADDON) );
		}
	}

	void SAPPLY::SAVE ( sc::SerialFile &SFile )
	{
		SFile << emBASIC_TYPE;
		SFile << emELEMENT;
		SFile << dwCUREFLAG;

		SFile.WriteBuffer( &sDATA_LVL, sizeof(SKILL::CDATA_LVL)* MAX_LEVEL );

		SFile << emSTATE_BLOW;
		SFile.WriteBuffer ( &sSTATE_BLOW, sizeof(SKILL::SSTATE_BLOW)*MAX_LEVEL );

		SFile << static_cast<int>(vecADDON.size());
		for ( DWORD i = 0; i < vecADDON.size(); ++i )
		{
			SFile.WriteBuffer ( &vecADDON[i], sizeof(SKILL::SIMPACT_ADDON) );			
		}

		bool bCA = false;

		SFile << static_cast<int>(vecSPEC.size());
		for ( DWORD i = 0; i < vecSPEC.size(); ++i )
		{
			if( vecSPEC[i].emSPEC == EMSPECA_COUNTATTACK ) bCA = true;

			SFile.WriteBuffer ( &vecSPEC[i], sizeof(SKILL::SSPEC_ADDON) );
		}

		// version 110
		SFile << nEffectFrame;

		if( bCA ) {
			SFile.WriteBuffer ( &sCAData, sizeof(SKILL::SSPEC_CA_ADDON) );
		}
	}


	VOID SAPPLY::SaveCsvHead ( std::fstream &SFile )
	{
		SFile << "emBASIC_TYPE" << ",";
		SFile << "emELEMENT" << ",";
		SFile << "dwCUREFLAG" << ",";
		for( int i=0; i<MAX_LEVEL; ++i )
		{
			SFile << "sDATA_LVL " << i << " fDELAYTIME" << ",";

			SFile << "sDATA_LVL " << i << " fLIFE" << ",";
			SFile << "sDATA_LVL " << i << " wAPPLYRANGE" << ",";
			SFile << "sDATA_LVL " << i << " wAPPLYANGLE" << ",";
			SFile << "sDATA_LVL " << i << " wPIERCENUM" << ",";
			SFile << "sDATA_LVL " << i << " wTARNUM" << ",";

			SFile << "sDATA_LVL " << i << " fBASIC_VAR" << ",";

			SFile << "sDATA_LVL " << i << " wUSE_ARROWNUM" << ",";
			SFile << "sDATA_LVL " << i << " wUSE_CHARMNUM" << ",";
			SFile << "sDATA_LVL " << i << " wUSE_BULLETNUM" << ",";

			SFile << "sDATA_LVL " << i << " wUSE_EXP" << ",";
			SFile << "sDATA_LVL " << i << " wUSE_HP" << ",";
			SFile << "sDATA_LVL " << i << " wUSE_MP" << ",";
			SFile << "sDATA_LVL " << i << " wUSE_SP" << ",";
			SFile << "sDATA_LVL " << i << " wUSE_CP" << ",";
			SFile << "sDATA_LVL " << i << " nUSE_BATTERY" << ",";

			SFile << "sDATA_LVL " << i << " wUSE_HP_PTY" << ",";
			SFile << "sDATA_LVL " << i << " wUSE_MP_PTY" << ",";
			SFile << "sDATA_LVL " << i << " wUSE_SP_PTY" << ",";
		}

		SFile << "emSTATE_BLOW" << ",";
		for( int i=0; i<MAX_LEVEL; ++i )
		{
			SFile << "sSTATE_BLOW " << i << " fRATE" << ",";
			SFile << "sSTATE_BLOW " << i << " fVAR1" << ",";
			SFile << "sSTATE_BLOW " << i << " fVAR2" << ",";
		}

		for( int i = 0; i < MAX_ADDON; ++i )
		{
			SFile << "emADDON" << i+1 << ",";
			for( int j=0; j<MAX_LEVEL; ++j )
			{
				SFile << "fADDON_VAR" << i+1 << "_" << j+1 << ",";
				SFile << "fRate" << i+1 << "_" << j+1 << ",";
			}
		}

		for( int i = 0; i < MAX_ADDON; ++i )
		{
			SFile << "emSPEC" << i+1 <<",";
			for( int j=0; j<MAX_LEVEL; ++j )
			{
				SFile << "sSPEC" << i+1 << "_" << j+1 << " fVAR1" << ",";
				SFile << "sSPEC" << i+1 << "_" << j+1 << " fVAR2" << ",";
				SFile << "sSPEC" << i+1 << "_" << j+1 << " fRate" << ",";
				SFile << "sSPEC" << i+1 << "_" << j+1 << " fRate2" << ",";
				SFile << "sSPEC" << i+1 << "_" << j+1 << " dwFLAG" << ",";
				SFile << "sSPEC" << i+1 << "_" << j+1 << " Native MID"	<< ",";
				SFile << "sSPEC" << i+1 << "_" << j+1 << " Native SID"	<< ",";
				SFile << "sSPEC" << i+1 << "_" << j+1 << " Link MID"	<< ",";
				SFile << "sSPEC" << i+1 << "_" << j+1 << " Link SID"	<< ",";
			}
		}
		SFile << "nEffectFrame"<< ",";

		for( int i = 0; i < MAX_LEVEL; ++i )
		{
			SFile << "sSPEC_CA" << i+1 << "_" << "bDamageApply" << ",";
			SFile << "sSPEC_CA" << i+1 << "_" << "bAllApply" << ",";

			for( int j=0; j<MAX_ADDON; ++j )
			{
				SFile << "sSPEC_CA" << i+1 << "_" << j+1 << " emSPEC" << ",";
			}
		}
	}

	VOID SAPPLY::SaveCsv ( std::fstream &SFile )
	{
		SFile << emBASIC_TYPE << ",";
		SFile << emELEMENT << ",";
		SFile << dwCUREFLAG << ",";
		for( DWORD i=0; i<MAX_LEVEL; ++i )
		{
			SFile << sDATA_LVL[i].fDELAYTIME << ",";

			SFile << sDATA_LVL[i].fLIFE << ",";
			SFile << sDATA_LVL[i].wAPPLYRANGE << ",";
			SFile << sDATA_LVL[i].wAPPLYANGLE << ",";
			SFile << sDATA_LVL[i].wPIERCENUM << ",";
			SFile << sDATA_LVL[i].wTARNUM << ",";

			SFile << sDATA_LVL[i].fBASIC_VAR << ",";

			SFile << sDATA_LVL[i].wUSE_ARROWNUM << ",";
			SFile << sDATA_LVL[i].wUSE_CHARMNUM << ",";
			SFile << sDATA_LVL[i].wUSE_BULLETNUM << ",";		

			SFile << sDATA_LVL[i].wUSE_EXP << ",";
			SFile << sDATA_LVL[i].wUSE_HP << ",";
			SFile << sDATA_LVL[i].wUSE_MP << ",";
			SFile << sDATA_LVL[i].wUSE_SP << ",";
			SFile << sDATA_LVL[i].wUSE_CP << ",";	
			SFile << sDATA_LVL[i].nUSE_BATTERY << ",";		

			SFile << sDATA_LVL[i].wUSE_HP_PTY << ",";
			SFile << sDATA_LVL[i].wUSE_MP_PTY << ",";
			SFile << sDATA_LVL[i].wUSE_SP_PTY << ",";
		}

		SFile << emSTATE_BLOW << ",";
		for( DWORD i=0; i<MAX_LEVEL; ++i )
		{
			SFile << sSTATE_BLOW[i].fRATE << ",";
			SFile << sSTATE_BLOW[i].fVAR1 << ",";
			SFile << sSTATE_BLOW[i].fVAR2 << ",";
		}


		for ( DWORD i = 0; i < MAX_ADDON; ++i )
		{
			SIMPACT_ADDON sAddon;
			if ( i < vecADDON.size() )	sAddon = vecADDON[i];

			SFile << sAddon.emADDON << ",";
			for( DWORD j=0; j<MAX_LEVEL; ++j )
			{
				SFile << sAddon.fADDON_VAR[j] << ",";
				SFile << sAddon.fRate[ j ] << ",";
			}
		}

		for ( DWORD i = 0; i < MAX_ADDON; ++i )
		{
			SSPEC_ADDON sAddon;
			if ( i < vecSPEC.size() )	sAddon = vecSPEC[i];

			SFile << sAddon.emSPEC << ",";
			for( DWORD j=0; j<MAX_LEVEL; ++j )
			{
				SFile << sAddon.sSPEC[j].fVAR1 << ",";
				SFile << sAddon.sSPEC[j].fVAR2 << ",";
				SFile << sAddon.sSPEC[j].fRate << ",";
				SFile << sAddon.sSPEC[j].fRate2 << ",";
				SFile << sAddon.sSPEC[j].dwFLAG << ",";
				SFile << sAddon.sSPEC[j].dwNativeID.wMainID << ",";
				SFile << sAddon.sSPEC[j].dwNativeID.wSubID << ",";
				SFile << sAddon.sSPEC[j].dwLinkID.wMainID << ",";
				SFile << sAddon.sSPEC[j].dwLinkID.wSubID << ",";
			}
		}
		SFile << nEffectFrame << ",";

		for( int i = 0; i < MAX_LEVEL; ++i )
		{
			SFile << sCAData.sSPEC_CA[i].bDamageApply << ",";
			SFile << sCAData.sSPEC_CA[i].bAllApply << ",";

			for( int j=0; j<MAX_ADDON; ++j )
			{
				SFile << sCAData.sSPEC_CA[i].emSPEC[j] << ",";
			}
		}
	}

	VOID SAPPLY::LoadCsv ( CStringArray &StrArray )
	{
		emBASIC_TYPE = (SKILL::EMTYPES)atoi( StrArray[ iCsvCur++ ] );
		emELEMENT = (EMELEMENT)atoi( StrArray[ iCsvCur++ ] );
		dwCUREFLAG = (DWORD)atol( StrArray[ iCsvCur++ ] );
		for( DWORD i=0; i<MAX_LEVEL; ++i )
		{
			sDATA_LVL[i].fDELAYTIME = (float)atof( StrArray[ iCsvCur++ ] );

			sDATA_LVL[i].fLIFE = (float)atof( StrArray[ iCsvCur++ ] );
			sDATA_LVL[i].wAPPLYRANGE = (WORD)atoi( StrArray[ iCsvCur++ ] );
			sDATA_LVL[i].wAPPLYANGLE = (WORD)atoi( StrArray[ iCsvCur++ ] );
			sDATA_LVL[i].wPIERCENUM = (WORD)atoi( StrArray[ iCsvCur++ ] );
			sDATA_LVL[i].wTARNUM = (WORD)atoi( StrArray[ iCsvCur++ ] );

			sDATA_LVL[i].fBASIC_VAR = (float)atof( StrArray[ iCsvCur++ ] );

			sDATA_LVL[i].wUSE_ARROWNUM = (WORD)atoi( StrArray[ iCsvCur++ ] );
			sDATA_LVL[i].wUSE_CHARMNUM = (WORD)atoi( StrArray[ iCsvCur++ ] );
			sDATA_LVL[i].wUSE_BULLETNUM = (WORD)atoi( StrArray[ iCsvCur++ ] );			

			sDATA_LVL[i].wUSE_EXP = (WORD)atoi( StrArray[ iCsvCur++ ] );
			sDATA_LVL[i].wUSE_HP = (WORD)atoi( StrArray[ iCsvCur++ ] );
			sDATA_LVL[i].wUSE_MP = (WORD)atoi( StrArray[ iCsvCur++ ] );
			sDATA_LVL[i].wUSE_SP = (WORD)atoi( StrArray[ iCsvCur++ ] );
			sDATA_LVL[i].wUSE_CP = (WORD)atoi( StrArray[ iCsvCur++ ] );
			sDATA_LVL[i].nUSE_BATTERY = (WORD)atoi( StrArray[ iCsvCur++ ] );

			sDATA_LVL[i].wUSE_HP_PTY = (WORD)atoi( StrArray[ iCsvCur++ ] );
			sDATA_LVL[i].wUSE_MP_PTY = (WORD)atoi( StrArray[ iCsvCur++ ] );
			sDATA_LVL[i].wUSE_SP_PTY = (WORD)atoi( StrArray[ iCsvCur++ ] );
		}

		emSTATE_BLOW = (EMSTATE_BLOW)atoi( StrArray[ iCsvCur++ ] );
		for( DWORD i=0; i<MAX_LEVEL; ++i )
		{
			sSTATE_BLOW[i].fRATE = (float)atof( StrArray[ iCsvCur++ ] );
			sSTATE_BLOW[i].fVAR1 = (float)atof( StrArray[ iCsvCur++ ] );
			sSTATE_BLOW[i].fVAR2 = (float)atof( StrArray[ iCsvCur++ ] );
		}

		vecADDON.clear();
		for ( DWORD i = 0; i < MAX_ADDON; ++i )
		{
			SIMPACT_ADDON sAddon;
			sAddon.emADDON = (EMIMPACT_ADDON)atoi( StrArray[ iCsvCur++ ] );
			for( DWORD j=0; j<MAX_LEVEL; ++j )
			{
				sAddon.fADDON_VAR[j] = (float)atof( StrArray[ iCsvCur++ ] );
				sAddon.fRate[j] = (float)atof( StrArray[ iCsvCur++ ] );
			}

			if ( sAddon.emADDON != EMIMPACTA_NONE )	vecADDON.push_back( sAddon );
		}

		vecSPEC.clear();
		for ( DWORD i = 0; i < MAX_ADDON; ++i )
		{
			SSPEC_ADDON sAddon;
			sAddon.emSPEC = (EMSPEC_ADDON)atoi( StrArray[ iCsvCur++ ] );

			for( DWORD j=0; j<MAX_LEVEL; ++j )
			{
				sAddon.sSPEC[j].fVAR1 = (float)atof( StrArray[ iCsvCur++ ] );
				sAddon.sSPEC[j].fVAR2 = (float)atof( StrArray[ iCsvCur++ ] );
				sAddon.sSPEC[j].fRate = (float)atof( StrArray[ iCsvCur++ ] );
				sAddon.sSPEC[j].fRate2 = (float)atof( StrArray[ iCsvCur++ ] );
				sAddon.sSPEC[j].dwFLAG = (DWORD)atol( StrArray[ iCsvCur++ ] );
				sAddon.sSPEC[j].dwNativeID.wMainID = (WORD)atoi( StrArray[ iCsvCur++ ] );
				sAddon.sSPEC[j].dwNativeID.wSubID = (WORD)atoi( StrArray[ iCsvCur++ ] );
				sAddon.sSPEC[j].dwLinkID.wMainID = (WORD)atoi( StrArray[ iCsvCur++ ] );
				sAddon.sSPEC[j].dwLinkID.wSubID = (WORD)atoi( StrArray[ iCsvCur++ ] );
			}

			if ( sAddon.emSPEC != EMSPECA_NULL )	vecSPEC.push_back( sAddon );			
		}

		// fRunningEffTime = (float)atof( StrArray[ iCsvCur++ ] );
		nEffectFrame = (INT)atoi( StrArray[ iCsvCur++ ] );

		for( int i = 0; i < MAX_LEVEL; ++i )
		{
			sCAData.sSPEC_CA[i].bDamageApply  = (bool)atoi( StrArray[ iCsvCur++ ] );
			sCAData.sSPEC_CA[i].bAllApply     = (bool)atoi( StrArray[ iCsvCur++ ] );

			for( int j = 0; j <MAX_ADDON; ++j )
			{
				sCAData.sSPEC_CA[i].emSPEC[j] = (EMSPEC_ADDON)atoi( StrArray[ iCsvCur++ ] );
			}
		}
	}
};