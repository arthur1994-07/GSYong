#include "pch.h"

#include "NSGUI.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace NSGUI
{
    // 인터페이스 사용시 캐릭터의 움직임을 제어
    BOOL m_bCharMoveBlock(FALSE);
	BOOL m_bCharMoveAllBlock(FALSE);


    BOOL IsCharMoveBlock()
    { 
        return m_bCharMoveBlock;
    }

	void SetCharMoveBlock()	
    { 
        m_bCharMoveBlock = TRUE; 
    }

	void ResetCharMoveBlock()	
    { 
        m_bCharMoveBlock = FALSE;
    }

	BOOL IsCharMoveAllBlock()
	{ 
		return m_bCharMoveAllBlock;
	}

	void SetCharMoveAllBlock()	
	{ 
		m_bCharMoveAllBlock = TRUE; 
	}

	void ResetCharMoveAllBlock()	
	{ 
		m_bCharMoveAllBlock = FALSE;
	}
}