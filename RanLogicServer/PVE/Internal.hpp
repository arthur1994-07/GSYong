#include "PVESystem.h"
#include "../../RanLogic/PVE/PVEDB.h"

using namespace TriggerSystem;

namespace PVESystem
{
    typedef std::map<DWORD, SPVEInDunID> PVEInDunMemberContainer;
    typedef PVEInDunMemberContainer::iterator PVEInDunMemberContainerIt;
    typedef PVEInDunMemberContainer::const_iterator PVEInDunMemberContainerCIt;

    static SQLite::Database         s_db;       //@ PVE 인던 DB
    static SServerAllMapInfoMessage s_cache;    //@ PVE 인던 정보 
    static PVEInDunContainer        s_indun;    //@ PVE 인던
    static PVEInDunMemberContainer  s_member;   //@ PVE 인던 멤버

    /**
        DB 오류 관련 처리를 한곳으로 모은다.
        향후 수정시 이곳에서만 하면 된다.

     */
    static void OutputDataBaseError(const TCHAR* pContents)
    {
        std::string strMessage( sc::string::format(_T("%1% ErrorCode : %2%"), pContents, s_db.GetLastError()) );
        sc::ErrorMessageBox(strMessage, false);
    }

    /**
        DB 파일을 다시 생성 한다.

     */
    static void ResetDB()
    {
        ResetPVEDBFile();

        if (s_db.IsOpen())
        {
            s_db.Close();
        }

		_tmkdir(_T("PVEDB"));

        // ":memory:" 는 메모리 DB를 의미 한다.
        if (s_db.Open(GetPVEDBFile()) != SQLITE_OK)
        {
            OutputDataBaseError(_T("Cann't Open Database Error"));
            return;
        }

        // 테이블 생성
        if (s_db.ExecuteSQL(strCreateQuery) != SQLITE_OK)
        {
            OutputDataBaseError(_T("Create DatabaseTable Error"));
            return;
        }
    }

} // end namespace