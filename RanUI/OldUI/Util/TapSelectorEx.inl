#pragma once

template<typename _BUTTONCONTROL>
CTapSelectorEx<_BUTTONCONTROL>::CTapSelectorEx ()
: m_TapSelectedID(0)
, m_TapSelectedControl(NULL)
{
}

template<typename _BUTTONCONTROL>
CTapSelectorEx<_BUTTONCONTROL>::~CTapSelectorEx()
{
    if ( !m_TapButtonMap.empty() )
    {
        m_TapButtonMap.clear();
    }

    m_TapSelectedID = 0;
    m_TapSelectedControl = NULL;
}

template<typename _BUTTONCONTROL>
const bool CTapSelectorEx<_BUTTONCONTROL>::RegisterTapButton ( _BUTTONCONTROL* pControl )
{
    if ( !pControl )
    {
        return true;
    }

    UIGUID cID = pControl->GetWndID();
	GASSERT( cID < FORCE_ID  );

    BUTTONMAP_ITER Finditer = m_TapButtonMap.find( cID );
    if ( Finditer != m_TapButtonMap.end() )
    {
        return false;
    }

    m_TapButtonMap.insert( std::make_pair(cID, pControl) );

    if ( !m_TapSelectedControl )
    {
        TapSelect (cID);
    }
    else
    {
        TapDisable ( pControl );
    }

    return true;
}

template<typename _BUTTONCONTROL>
void CTapSelectorEx<_BUTTONCONTROL>::TapSelectorUIMessage ( const UIGUID ControlID, const DWORD dwMsg, EMTAPSELECTORRESULT& emOutResult )
{
    emOutResult = EMTAPSELECTORRESULT_NONE;

	if ( CHECK_MOUSE_IN(dwMsg) )
	{
		if ( TapOver(ControlID) == true )
		{
			EventOverTap(ControlID);
		}
	}
    if ( CHECK_KEYFOCUSED( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
    {
        if ( !TapSelect ( ControlID ) )
        {
            return;
        }

        emOutResult = EMTAPSELECTORRESULT_SELECT;
    }
}

template<typename _BUTTONCONTROL>
const bool CTapSelectorEx<_BUTTONCONTROL>::TapOver(const UIGUID ControlID)
{
	BUTTONMAP_ITER Finditer = m_TapButtonMap.find( ControlID );
	return Finditer == m_TapButtonMap.end() ? false : true;
}

template<typename _BUTTONCONTROL>
const bool CTapSelectorEx<_BUTTONCONTROL>::TapSelect ( const UIGUID ControlID )
{
    BUTTONMAP_ITER Finditer = m_TapButtonMap.find( ControlID );
    if ( Finditer == m_TapButtonMap.end() )
    {
        return false;
    }

    if ( m_TapSelectedControl == Finditer->second )
    {
        return false;
    }

    if ( m_TapSelectedControl )
    {
        TapDisable ( m_TapSelectedControl );
    }

    TapEnable ( Finditer->second );

    m_TapSelectedID       = ControlID;
    m_TapSelectedControl  = Finditer->second;

	EventSelectedTap(ControlID);

    return true;
}
