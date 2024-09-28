
#pragma once

#include "UIDataType.h"

#include <list>
#include <map>

//----------------------------------------------------------//

class CUIControl;

class CUIDockingMan
{

public:
    struct SDOCKINGINFO
    {
        typedef std::list<CUIControl*> UICONTROL_LIST;

        CUIControl*     pCONTROL;
        UIRECT          rcRECT;
        SDOCKINGINFO*   pPARENT;
        BOOL            ISPARENT;
        UICONTROL_LIST  CHILDLIST;

        SDOCKINGINFO (CUIControl* _pCONTROL, const BOOL _ISPARENT);
        const UIRECT GetGlobalPos ();
        void         SetGlobalPos ( D3DXVECTOR2& vPos );
    };

public:
    typedef std::map<UIGUID,SDOCKINGINFO> UIDOCKINGINFO_MAP;
    typedef UIDOCKINGINFO_MAP::iterator   UIDOCKINGINFO_MAP_ITER;

public:
    static const FLOAT DEFAULT_ATTACH_DISTANCE;

public:
    CUIDockingMan ();
    ~CUIDockingMan ();

public:
    void                RemoveAll();

public:
    void                Update ( float fElapsedTime );

public:
    const BOOL          RegisterControl  ( CUIControl* pControl, const BOOL bParent=FALSE );

public:
    void                SetDragControl   ( CUIControl* pControl );
    void                ResetDragControl () { m_pDragControl = NULL; }

public:
    const FLOAT         GetWindowWidth  ();
    const FLOAT         GetWindowHeight ();

protected:
    const BOOL          AttachCheck     ( const UIRECT& rcSrc, const UIRECT& rcDest, D3DXVECTOR2& vInOutMove );
    const BOOL          CollisionCheck  ( const UIRECT& rcSrc, const UIRECT& rcDest );

private:
    UIDOCKINGINFO_MAP   m_DockingMap;

private:
    SDOCKINGINFO*       m_pDragControl;
    UIRECT              m_rcDragReal;
    UIRECT              m_rcDragVirtual;

    FLOAT               m_fAttachDistance;
};
