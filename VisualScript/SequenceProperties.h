/**
 * \date	2011/01/25
 * \author	Jun-Hyuk Choi
 */
#pragma once

class CFrame;

class CSequenceProperties : public CXTPPropertyGrid
{
public:
    typedef std::map< size_t, DWORD > PropertiesClassMap;
    typedef PropertiesClassMap::value_type PropertiesClassMapValue;
    typedef PropertiesClassMap::iterator PropertiesClassMapIter;
    typedef PropertiesClassMap::reverse_iterator PropertiesClassMapRIter;

protected:
    CFrame* m_pFrame;
    PropertiesClassMap m_mapPropertiesClass;

public:
    CSequenceProperties();
    virtual ~CSequenceProperties();

protected:
	virtual LRESULT WindowProc( UINT message, WPARAM wParam, LPARAM lParam );

public:
    VOID RefreshPropertiesControl();

protected:
    VOID AddClassCategory( size_t nIndex );

public:
    virtual CFrame* GetFrame() { return m_pFrame; }
};