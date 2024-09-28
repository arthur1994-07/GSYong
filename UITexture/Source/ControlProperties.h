#pragma once

class CFrame;

class CControlProperties : public CXTPPropertyGrid
{
protected:
    CFrame* m_pFrame;

	std::string m_strEditID;

public:
    CControlProperties();
    virtual ~CControlProperties();

protected:
	virtual LRESULT WindowProc( UINT message, WPARAM wParam, LPARAM lParam );

public:
    void Refresh();

private:
	void OnChangedID();

public:
    virtual CFrame* GetFrame() { return m_pFrame; }
};