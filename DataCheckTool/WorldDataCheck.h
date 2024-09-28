#pragma once

class CWorldDataCheck
{
public:
	CWorldDataCheck(void);
	~CWorldDataCheck(void);

	BOOL LoadFile ( const char *szFile );


private:
	const static char FILEMARK[128];
};
