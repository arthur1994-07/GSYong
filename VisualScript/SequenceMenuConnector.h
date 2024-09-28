/**
 * \date	2011/01/25
 * \author	Jun-Hyuk Choi
 */
#pragma once

class CSequenceMenuConnector : public CMenu
{
public:
    enum
    {
        EBreakLinkToMax = 100,

        EBreakAllLinks = 1,
        EBreakLinkTo,
        EBreakLinkToEnd = EBreakLinkTo + EBreakLinkToMax,
        ESetActivateDelay,
        ESetActiveEvent,
    };

public:
    struct SSelect
    {
        size_t nIndex;
    };

protected:
    CMenu m_cMenuBreakLinkTo;

public:
    CSequenceMenuConnector();
    virtual ~CSequenceMenuConnector();

public:
    SSelect Open( const ic::Int2& sPos, CWnd* pParent );
};