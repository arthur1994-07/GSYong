/**
 * \date	2011/01/25
 * \author	Jun-Hyuk Choi
 */
#pragma once

class CSequenceMenuBackground : public CMenu
{
public:
    enum
    {
        ESequenceObjectMax = 10000,

        ENewComment = ESequenceObjectMax,
        ECreateNewSequence,
        EPasteHere,
    };

public:
    struct SSelect
    {
        size_t nIndex;
    };

protected:
    CMenu m_cMenuEvent;
    CMenu m_cMenuVariable;
    CMenu m_cMenuAction;

public:
    CSequenceMenuBackground();
    virtual ~CSequenceMenuBackground();

public:
    SSelect Open( const ic::Int2& sPos, CWnd* pParent );
};