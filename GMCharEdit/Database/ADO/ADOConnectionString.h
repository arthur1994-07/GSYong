#include "../../../SigmaCore/Util/Lock.h"

#include <string>
#include <map>

class CjConnectionStrings : public sc::DefaultLock
{
public:
	static	CjConnectionStrings* GetInstance();
	static	void ReleaseInstance();

	BOOL	AddConnString(int nKey, CString &csIP, CString &csDBName, CString &csUserID, CString &csUserPass);
	BOOL	AddConnString(int nKey, std::string &strIP, std::string &strDBName, std::string &strUserID, std::string &strUserPass);

	BOOL	GetConnString(int nKey, OUT CString &csConnString);


	/*	required to be implemented

			DeleteConnString(...);
			ReplaceConnString(...);

	*/


	BOOL	Reset() { m_mConnStrings.clear(); }


	virtual	~CjConnectionStrings();

private:
	CjConnectionStrings();

	static	CjConnectionStrings *m_pInstance;

	typedef std::map<int, CString>			 MAP_CONNS;
	typedef std::map<int, CString>::iterator MAP_CONNS_ITER;

	MAP_CONNS m_mConnStrings;
};