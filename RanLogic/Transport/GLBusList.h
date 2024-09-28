#pragma once

#include "./GLBusData.h"

class GLBusList : public GLBusData
{
public:
	GLBusList(void);
	virtual ~GLBusList(void);

protected:
	std::vector<SVILLAGE> m_vecVILLAGE;

protected:
	void insert(DWORD dwSTATIONID, SSTATION &sSTATION);

public:
	virtual bool LOAD(const std::string& strFILE, bool bToolMode) override;
	virtual SSTATION* GetStation(DWORD dwID) override;

	DWORD GetVillageNum();
	SVILLAGE* GetVillage(DWORD dwID);	
};
