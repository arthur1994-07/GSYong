#include "stdafx.h"
#include "./OldUI/Interface/PetHungry.h"
#include "./OldUI/Pet/PetWindow.h"
#include "./OldUI/Pet/PetskinMixImage.h"
#include "./InnerInterface.h"

//-----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
//-----------------------------------------------------------------------------


void CInnerInterface::PetWindowSetSkillCombo()
{
    m_pPetWindow->SetSkillCombo();
}

void CInnerInterface::SetPetHungry()
{
    m_pPetHungry->SetPetHungry();
}

void CInnerInterface::SetPetHungryItem( SNATIVEID sPetItemID )
{
    m_pPetHungry->SetPetItem( sPetItemID );
}

void CInnerInterface::PetSkinMixImageSetItemData(WORD wPosX, WORD wPosY, SNATIVEID sItemData, WORD wHoldPosX, WORD wHoldPosY)
{
    m_pPetSkinMixImage->SetItemData(wPosX, wPosY, sItemData, wHoldPosX, wHoldPosY);
}

void CInnerInterface::PetSatiationUpdate( int Satiation )
{
    m_pPetHungry->PetSatiationUpdate( Satiation );
}