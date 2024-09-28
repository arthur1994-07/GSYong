#include "pch.h"
#include "../../SigmaCore/gassert.h"
#include "UIDebugSet.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace	NS_UIDEBUGSET
{
	#ifdef	__UI_DEGUG__
	BOOL	m_bInBlock = FALSE;
	#endif	//	__UI_DEGUG__

	void  BlockBegin ()
	{
	#ifdef	__UI_DEGUG__
		if ( m_bInBlock )
		{
			GASSERT ( 0 && "�̹� �ٸ� ������ ���� �ɸ��� �ֽ��ϴ�." );
			return ;
		}

		m_bInBlock = TRUE;
	#endif	//	__UI_DEGUG__
	}

	BOOL  IsInBlock ()
	{
	#ifdef	__UI_DEGUG__
		return m_bInBlock;
	#endif	//	__UI_DEGUG__
		return FALSE;
	}

	void  BlockEnd ()
	{
	#ifdef	__UI_DEGUG__
		m_bInBlock = FALSE;
	#endif	//	__UI_DEGUG__
	}
}