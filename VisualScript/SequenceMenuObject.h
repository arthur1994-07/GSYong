/**
 * \date	2011/01/25
 * \author	Jun-Hyuk Choi
 */
#pragma once

class CSequenceMenuObject : public CMenu
{
public:
    enum
    {
        EBreakAllLinks = 1,
        EEnumProperty,
    };

public:
    struct SSelect
    {
        size_t nIndex;
    };

public:
    CSequenceMenuObject();
    virtual ~CSequenceMenuObject();

public:
    SSelect Open( const ic::Int2& sPos, CWnd* pParent );
};