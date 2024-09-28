#pragma once

namespace sc {
    class SerialFile;
}


struct	SBGMDATA
{
static	WORD	VER;

	CString	strFileName;

public:
	BOOL	SaveSet ( sc::SerialFile& SFile );
	BOOL	LoadSet ( sc::SerialFile& SFile );

public:
	void	SetFile ( CString &strFile )
	{
		strFileName = strFile;
	}
	void	ClearFile ()
	{
		strFileName.Empty ();
	}
};