
#include "stdafx.h"

#include "RnMapHelper.h"

#include "../../../RanLogic/Crow/GLCrowDataMan.h"
#include "../../../RanLogic/Crow/GLCrowData.h"
#include "../../../RanLogic/Quest/GLQuestMan.h"
#include "../../../RanLogic/Quest/GLQuest.h"


#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../RanLogicClient/Char/GLCharClient.h"

#include "../../../enginelib/GUInterface/UIDataType.h"
#include "../../../enginelib/GUInterface/UIControl.h"



namespace RnMapPrivate
{
	void GetQuestFromMonsterID(GLGaeaClient* pGaeaClient,DWORD monsterID,std::vector<DWORD>* pQuestIdVec )
	{
		const GLQuestPlay::QUESTMOB_MAP& mapQuestMob = pGaeaClient->GetCharacter()->m_cQuestPlay.GetQuestMobProc();
		GLQuestPlay::QUESTMOB_MAP::const_iterator it = mapQuestMob.find(monsterID);
		if( it != mapQuestMob.end() )
		{
			DWORD questID = it->second; 
			pQuestIdVec->push_back(questID);
		}
	}

	MONSTER_TYPE GetMonsterStrongType(GLGaeaClient* pGaeaClient,SCROWDATA* pCrowData)
	{
		MONSTER_TYPE monsterIconType = MONSTER_TYPE_VERY_WEAK;

		const int LEVEL_DIFF_STRONG = 30;
		int diff = pGaeaClient->GetCharacter()->GetLevel() - pCrowData->m_wLevel;
		if( diff > LEVEL_DIFF_STRONG )
			monsterIconType = MONSTER_TYPE_VERY_WEAK;
		else if( diff <= LEVEL_DIFF_STRONG - 1  )
			monsterIconType = MONSTER_TYPE_WEAK;
		else if( diff < -29 )
			monsterIconType = MONSTER_TYPE_NORMAL;
		else if( diff > -30 )
			monsterIconType = MONSTER_TYPE_STRONG;
		else
			GASSERT(false);
		
		return monsterIconType;
	}

	MONSTER_TYPE GetMonsterIconType(GLGaeaClient* pGaeaClient,SCROWDATA* pCrowData)
	{
		MONSTER_TYPE monsterIconType = MONSTER_TYPE_QUEST;
		std::vector<DWORD> questIdVec;
		GetQuestFromMonsterID(pGaeaClient,pCrowData->sNativeID.Id(),&questIdVec);
		if(questIdVec.empty())	
		{
			if( pCrowData->IsBoss() )
			{
				monsterIconType = MONSTER_TYPE_BOSS;
			}
			else
			{
				monsterIconType = GetMonsterStrongType(pGaeaClient,pCrowData);
			}
		}

		return monsterIconType;
	}

	void ChangeTexture(CUIControl* pControl,MONSTER_TYPE monsterType)
	{

		float width  = 128.f;//(float)pControl->GetTextureSizeWidth();
		float height = 128.f;//(float)pControl->GetTextureSizeHeight();

		float fMicroX = -0.25f/width;
		float fMicroY = -0.25f/height;

		UIRECT rect;
		if( monsterType == MONSTER_TYPE_QUEST)
			rect = UIRECT( 28/width-fMicroX,12/height-fMicroY, 14/width,14/height );
		else if( monsterType == MONSTER_TYPE_BOSS)
			rect = UIRECT( 56/width-fMicroX,12/height-fMicroY, 14/width,14/height);
		else if( monsterType == MONSTER_TYPE_VERY_WEAK )
			rect = UIRECT( 0/width-fMicroX,12/height-fMicroY,  14/width,14/height);
		else if( monsterType == MONSTER_TYPE_WEAK)
			rect = UIRECT( 14/width-fMicroX,12/height-fMicroY, 14/width,14/height);
		else if( monsterType == MONSTER_TYPE_NORMAL)
			rect = UIRECT( 14/width-fMicroX,12/height-fMicroY, 14/width,14/height);
		else if( monsterType == MONSTER_TYPE_STRONG)
			rect = UIRECT( 42/width-fMicroX,12/height-fMicroY, 14/width,14/height );
		else
			GASSERT(false);

		pControl->SetTexturePos(rect);
	}
}