#include "PVESystem.h"
#include "../../RanLogic/PVE/PVEDB.h"

using namespace TriggerSystem;

namespace PVESystem
{
    typedef std::map<DWORD, SPVEInDunID> PVEInDunMemberContainer;
    typedef PVEInDunMemberContainer::iterator PVEInDunMemberContainerIt;
    typedef PVEInDunMemberContainer::const_iterator PVEInDunMemberContainerCIt;

    static SQLite::Database         s_db;       //@ PVE �δ� DB
    static SServerAllMapInfoMessage s_cache;    //@ PVE �δ� ���� 
    static PVEInDunContainer        s_indun;    //@ PVE �δ�
    static PVEInDunMemberContainer  s_member;   //@ PVE �δ� ���

    /**
        DB ���� ���� ó���� �Ѱ����� ������.
        ���� ������ �̰������� �ϸ� �ȴ�.

     */
    static void OutputDataBaseError(const TCHAR* pContents)
    {
        std::string strMessage( sc::string::format(_T("%1% ErrorCode : %2%"), pContents, s_db.GetLastError()) );
        sc::ErrorMessageBox(strMessage, false);
    }

    /**
        DB ������ �ٽ� ���� �Ѵ�.

     */
    static void ResetDB()
    {
        ResetPVEDBFile();

        if (s_db.IsOpen())
        {
            s_db.Close();
        }

		_tmkdir(_T("PVEDB"));

        // ":memory:" �� �޸� DB�� �ǹ� �Ѵ�.
        if (s_db.Open(GetPVEDBFile()) != SQLITE_OK)
        {
            OutputDataBaseError(_T("Cann't Open Database Error"));
            return;
        }

        // ���̺� ����
        if (s_db.ExecuteSQL(strCreateQuery) != SQLITE_OK)
        {
            OutputDataBaseError(_T("Create DatabaseTable Error"));
            return;
        }
    }

} // end namespace