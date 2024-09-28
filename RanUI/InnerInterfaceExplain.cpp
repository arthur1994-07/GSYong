#include "stdafx.h"
#include "./InnerInterface.h"

#include "./OldUI/Tutorial/ExplainInterface.h"

//-----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
//-----------------------------------------------------------------------------


IExplainInterface* CInnerInterface::GetExplainInterface()
{ 
    return m_pExplainInterface;
}

BOOL CInnerInterface::ExplainIsSlotBoxVisibleAtInven() const 
{ 
    return m_pExplainInterface->IsSlotBoxVisibleAtInven();
}

void CInnerInterface::ExplainSetSlotBoxVisibleAtInven(BOOL State)
{ 
    m_pExplainInterface->SetSlotBoxVisibleAtInven(State);
}

BOOL CInnerInterface::ExplainIsSlotBoxVisible(unsigned int Index) const
{
    return m_pExplainInterface->IsSlotBoxVisible(Index);
}

void CInnerInterface::ExplainSetSlotBoxVisible(unsigned int Index, BOOL Visible)
{
    m_pExplainInterface->SetSlotBoxVisible(Index, Visible);
}
