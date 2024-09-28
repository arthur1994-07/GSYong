#include "stdafx.h"
#include "./OldUI/Char/RnCharacterWindowInterface.h"
#include "./InnerInterface.h"


//-----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
//-----------------------------------------------------------------------------


void CInnerInterface::CharacterSetPhoneNumber(const CString& PhoneNumber)
{
	// ╬х╬╡юс
	GASSERT(FALSE);
//    m_pCharacterWindow->SetPhoneNumber(PhoneNumber);
}

void CInnerInterface::UpdateActivityBadge()
{
    if ( m_pCharacterWindow && UiIsVisibleGroup( CHARACTER_WINDOW ) )
    {
        m_pCharacterWindow->UpdateTitleData();
    }
}

void CInnerInterface::SetStorageItemToVehicleTapWindow(const std::vector<DWORD>& vehicleIDVec)
{
	m_pCharacterWindow->SetStorageItemToVehicleTapWindow(vehicleIDVec);
}

void CInnerInterface::SetVisibleCharacterAdditionalWindow( bool bVisible )
{
	m_pCharacterWindow->SetVisibleCharacterAdditionalWindow(bVisible);
}
