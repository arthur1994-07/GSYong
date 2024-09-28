#ifndef _SC_ADO_CONNECTION_STRING_H_
#define _SC_ADO_CONNECTION_STRING_H_

#pragma once

#include <string>
#include <map>
#include "../../Util/Lock.h"

namespace sc {
    namespace db {

        class CjConnectionStrings : public DefaultLock
        {
        private:
            CjConnectionStrings();
            static	CjConnectionStrings* m_pInstance;

            typedef std::map<int, std::string>	MAP_CONNS;
            typedef MAP_CONNS::iterator         MAP_CONNS_ITER;
            typedef MAP_CONNS::const_iterator   MAP_CONNS_CITER;
            typedef MAP_CONNS::value_type       MAP_CONNS_VALUE;
            
            MAP_CONNS m_mConnStrings;

        public:
	        static CjConnectionStrings* GetInstance();
	        static void ReleaseInstance();

	        BOOL AddConnString(
                int nKey,
                const CString& csIP,
                const CString& csDBName,
                const CString& csUserID,
                const CString& csUserPass);
	        
            BOOL AddConnString(
                int nKey,
                const std::string& strIP,
                const std::string& strDBName,
                const std::string& strUserID,
                const std::string& strUserPass);

            BOOL GetConnString(int nKey, OUT std::string& csConnString);

	        /*	required to be implemented
			        DeleteConnString(...);
			        ReplaceConnString(...);
	        */
	        void Reset();
	        virtual	~CjConnectionStrings();
        }; // class CjConnectionStrings

    } // namespace db
} // namespace sc

#endif // _SC_ADO_CONNECTION_STRING_H_