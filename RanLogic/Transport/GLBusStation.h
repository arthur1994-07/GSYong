#pragma once

#include "./GLBusData.h"

class GLBusStation : public GLBusData
{
protected:
	std::string				m_strPATH;

protected:
	void insert( SSTATION &sSTATION );

public:
	void SetPath(const std::string& strPATH) { m_strPATH = strPATH; }
	std::string GetPath() const	{ return m_strPATH; }

public:
	virtual bool LOAD(const std::string& strFILE, bool bToolMode) override;

	virtual SSTATION* GetStation ( DWORD dwID );

protected:
	GLBusStation(void);

public:
	virtual ~GLBusStation(void);

public:
	static GLBusStation& GetInstance ();
};
