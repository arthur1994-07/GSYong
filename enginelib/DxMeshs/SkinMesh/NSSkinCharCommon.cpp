#include "pch.h"

#include "./DxSkinAniControlData.h"

#include "NSSkinCharCommon.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace NSSkinCharCommon
{
	BOOL LoadAnimationFromTool( const TCHAR* szNameCHF, 
								const TCHAR* szNameCFG, 
								const TCHAR* szNameXFile, 
								LPDIRECT3DDEVICEQ pd3dDevice, 
								bool bREPRESH, 
								DxSkinAniControlData& sSkinAniControlData,
								VEC_TSTRING& vecAniFile )
	{
		for( DWORD i=0; i<vecAniFile.size(); ++i )
		{
			if( vecAniFile[i] == szNameCFG )
			{
				return TRUE;
			}
		}

		DxSkinAnimationRes skinAniRes = DxSkinAnimationManager::GetInstance().LoadAnimContainerFromTool(szNameCFG, szNameXFile, sSkinAniControlData.GetSkeletonName(), bREPRESH );
		if ( !skinAniRes.IsValid() )	
		{
			return FALSE;
		}

		vecAniFile.push_back( szNameCFG );


		sSkinAniControlData.CleanUp();
		sSkinAniControlData.CreateSkinAni_EDIT( vecAniFile );

		return TRUE;
	}

	void ReleaseAnimationFromTool( LPDIRECT3DDEVICEQ pd3dDevice, 
									const TCHAR* szAnim,
									DxSkinAniControlData& sSkinAniControlData,
									VEC_TSTRING& vecAniFile )
	{
		// Áö¿ò.
		VEC_TSTRING_ITER iter = vecAniFile.begin();
		for( ; iter!=vecAniFile.end(); )
		{
			if( (*iter) == szAnim )
			{
				iter = vecAniFile.erase( iter );
				DxSkinAnimationManager::GetInstance().DeleteData(szAnim);
			}
			else
			{
				++iter;
			}
		}

		sSkinAniControlData.CleanUp();
		sSkinAniControlData.CreateSkinAni_EDIT( vecAniFile );
	}
};