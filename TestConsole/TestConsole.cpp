// TestConsole.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//  - 

#include "stdafx.h"
#include <boost/crc.hpp>
#include <boost/tr1/memory.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/time_facet.hpp>
//#include <boost/locale.hpp>
#include <vector>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/regex.hpp>
#include <WinSock2.h>
#include "../=CryptoPP/sha.h"
#include "../=CryptoPP/iterhash.h"

#ifndef TSTRING
#define TSTRING std::string
#endif

#include "../SigmaCore/SigmaCore.h"
#include "../SigmaCore/Encrypt/RandomRC5Encrypt.h"
#include "../SigmaCore/Log/LogMan.h"
#include "../SigmaCore/String/StringUtil.h"
#include "../SigmaCore/String/StringUtils.h"
#include "../SigmaCore/String/StringFormat.h"
#include "../SigmaCore/File/AutoTxtFile.h"
#include "../SigmaCore/Math/Random.h"
#include "../SigmaCore/Util/MinMessageBox.h"

#include "../SigmaCore/Util/SystemInfo.h"
#include "../SigmaCore/Util/DateTime.h"

#include "../EngineLib/Common/SUBPATH.h"

#include "../RanLogic/WorldBattle/WorldBattleCountry.h"
#include "../RanLogic/GLogicFile.h"

#include "../SigmaCore/File/AutoTxtFile.h"
#include "../SigmaCore/Math/Random.h"
#include "../RanLogicServer/Util/GLItemLimit.h"
#include "../RanLogicServer/Util/IpAddressChecker.h"
#include "../RanLogicServer/Database/ODBC/s_COdbcManager.h"
#include "../RanLogicServer/Database/ODBC/s_COdbcSupervisor.h"
#include "../RanLogicServer/Database/ODBC/OdbcStmt.h"
#include "../RanLogicServer/PrivateMarket/GLPrivateMarketSearchMan.h"

#include "../RanLogic/Item/GLItemMan.h"

#include "LogicTest.h"
#include "BoostFormatTest.h"
#include "./CharTest.h"
#include "TestConsole.h"

#include "../SigmaCore/DebugInclude.h"
//#include "../=VLD/include/vld.h"

// 유일한 응용 프로그램 개체입니다.

CWinApp theApp;

using namespace std;

void AppPath();
void BadFormatTest();
void SharedPtrTest();
void UintTest();
void BoostSplitTest();
void Sha256();
void CheckSsnHead();
void Crc32();
void ThaiCheck();
void GLItemLimitTest();
void IpAddressCheck();
void OdbcPoolTest();
void AutoTxtFileTest();
void ContainerEraseTest();
void AutoTxtFileTest();
void RandomRC5_Test();
void ContainerEraseTest();
void BoostLocaleTest();
void ParseTest();
void MultiIndexTest();
bool SQLiteTest();

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

/*	// MFC를 초기화합니다. 초기화하지 못한 경우 오류를 인쇄합니다.
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: 오류 코드를 필요에 따라 수정합니다.
		_tprintf(_T("심각한 오류: MFC를 초기화하지 못했습니다.\n"));
		nRetCode = 1;
	}
	else
	{
		// TODO: 응용 프로그램의 동작은 여기에서 코딩합니다.
        sc::initialize();

        //AppPath();
        //BadFormatTest();
        //SharedPtrTest();
        //UintTest();
        //BoostSplitTest();
        //Sha256();
        //CheckSsnHead();
        //Crc32();
        //ThaiCheck();
        //GLItemLimitTest();
        //IpAddressCheck();
        //OdbcPoolTest();
        //AutoTxtFileTest();
        //ContainerEraseTest();
        /*
        sc::RandomInt RandomGen;
        for (int i=0; i<1000; ++i)
        {
            __int64 X = RandomGen.Int64();
            if (X <= 0)
                std::cout << X << std::endl;
            if (X >= (LLONG_MAX-100000000))
                std::cout << X << std::endl;
        }
        */
        //OdbcPoolTest();
        //AutoTxtFileTest();
        //ContainerEraseTest();
        /*
        sc::RandomInt RandomGen;
        for (int i=0; i<1000; ++i)
        {
            __int64 X = RandomGen.Int64();
            if (X <= 0)
                std::cout << X << std::endl;
            if (X >= (LLONG_MAX-100000000))
                std::cout << X << std::endl;
        }
        //OdbcPoolTest();
        
        LogicTest* pLogicTest = new LogicTest;
        pLogicTest->Init();
        pLogicTest->DoTest();
        delete pLogicTest;
        */
        /*
        __time64_t Time = 323;
        sc::time::ValidTime(Time);
        sc::time::AdjustTime(Time);
        */
        
		/*
        for (int i=0; i<10000; ++i)
            RandomRC5_Test();
		*/

        //BoostLocaleTest();

        //ParseTest();

       // MultiIndexTest();

        //sc::finalize();
        //ContainerEraseTest();
	//}

    //sc::LogMan::releaseInstance();*/

	SQLiteTest();

	return nRetCode;
}

void BoostLocaleTest()
{
    /*
    time_t now=time(0);
    cout << "Today is "<< boost::locale::as::date << now << " and tomorrow is " << now+24*3600 << endl;
    cout << "Current time is "<< boost::locale::as::time << now << endl;
    cout << "The current weekday is "<< boost::locale::as::ftime("%A") << now << endl;
    */
    //std::string TimeZone = boost::locale::time_zone::global(); 
    CTime t(2012, 6, 5, 10, 44, 59); 
    // 10:15 PM March 19, 1999
    CString s = t.Format("%x %X");
    std::cout << s.GetString() << std::endl;

    s = t.Format("%z, %Z");
    std::cout << s.GetString() << std::endl;

    s = t.Format("%#x");
    std::cout << s.GetString() << std::endl;

    s = t.Format("%#c");
    std::cout << s.GetString() << std::endl;

    s = t.Format("%c");
    std::cout << s.GetString() << std::endl;

    COleDateTime t1(1999, 3, 19, 22, 15, 0);
    CString str = t1.Format(LOCALE_NOUSEROVERRIDE, LANG_KOREAN);
    std::cout << str.GetString() << std::endl;
}

void RandomRC5_Test()
{
    std::string PlainText("03ac674216f3e15c761ee1a5e255f067953623c8b388b4459e13f978d7c846f4"); // 1234 -> Sha256
    
    // Enc
    sc::RandomRC5 EncA(std::string(""));
    std::string EncText = EncA.GetEncrypt(PlainText);
    std::string KeyText = EncA.GetKey();

    // Dec
    sc::RandomRC5 DecA(KeyText);
    std::string DecText = DecA.GetDecrypt(EncText);

    std::cout << "Key       " << KeyText << std::endl;
    std::cout << "PlainText " << PlainText << std::endl;
    std::cout << "EncText   " << EncText << std::endl;
    std::cout << "DecText   " << DecText << std::endl;

    if (PlainText != DecText)
        assert(0);
}

void IpAddressCheck()
{    
    WSADATA wsaData;
    int iResult;
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        return;
    }

    std::string IpAddress;
    IpAddressChecker IpCheck;

    bool Allow = false;
    
    // private ip
    IpAddress = "10.0.0.0";
    if (!IpCheck.IsAllowIp(IpAddress))
        std::cout << "Not allow Ip " << IpAddress << std::endl;

    IpAddress = "10.0.0.255";    
    if (!IpCheck.IsAllowIp(IpAddress))
        std::cout << "Not allow Ip " << IpAddress << std::endl;

    for (int i=0; i<110; ++i)
        IpCheck.AddConnection("192.168.255.255");

    for (int i=0; i<110; ++i)
        IpCheck.AddConnection("103.4.175.75");

    IpAddress = "192.168.255.255";
    if (!IpCheck.IsAllowIp(IpAddress))
        std::cout << "Not allow Ip " << IpAddress << std::endl;

    IpAddress = "103.4.175.75";
    if (!IpCheck.IsAllowIp(IpAddress))
        std::cout << "Not allow Ip " << IpAddress << std::endl;

    for (int i=0; i<100; ++i)
        IpCheck.DelConnection(IpAddress);

    if (!IpCheck.IsAllowIp(IpAddress))
        std::cout << "Not allow Ip " << IpAddress << std::endl;
    else
        std::cout << "allow Ip " << IpAddress << std::endl;

    IpCheck.AddAllowIp(IpAddress);
    for (int i=0; i<110; ++i)
        IpCheck.AddConnection(IpAddress);

    if (!IpCheck.IsAllowIp(IpAddress))
        std::cout << "Not allow Ip " << IpAddress << std::endl;
    else
        std::cout << "allow Ip " << IpAddress << std::endl;
}

void GLItemLimitTest()
{
    GLItemLimit* pLimit = new GLItemLimit(NULL, NULL, NULL);
    pLimit->TxtLogToDB();

    for (int i=0; i<5; ++i)
    {
        __int64 GenNum = 0;
        pLimit->RegItemGen(GenNum, SNATIVEID(1,1), EMGEN_DEFAULT);
        std::cout << "GenNum:" << GenNum << std::endl;
    }

    for (int i=0; i<5; ++i)
    {
        __int64 GenNum = 0;
        pLimit->RegItemGen(GenNum, SNATIVEID(1,1), EMGEN_POINTSHOP);
        std::cout << "GenNum:" << GenNum << std::endl;
    }   
    
    delete pLimit;
}

void ThaiCheck()
{
    /*
    bool bCheck = false;
    bCheck = STRUTIL::CheckThaiString("สถานะของเมล");
    bCheck = sc::string::checkThaiName(L"สถานะของเมล");
    bCheck = STRUTIL::CheckThaiString("1สถานะของเมล");
    bCheck = STRUTIL::CheckThaiString("abcสถานะของเมล");
    bCheck = STRUTIL::CheckThaiString("abc สถานะของเมล");
    bCheck = STRUTIL::CheckThaiString("한글 สถานะของเมล");
    */
}

void Crc32()
{
    struct MSG_BASE
    {
        DWORD Size;
        DWORD Type;
    };

    struct MSG1 : public MSG_BASE
    {
        DWORD Value1;
        bool Value2;
        BYTE Value3;
        WORD Value4;
        DWORD Value5;
        unsigned int CrcResult;
    };

    MSG1 msg1;
    msg1.Size = sizeof(MSG1);
    msg1.Type = 111;
    msg1.Value1 = 1;
    msg1.Value2 = 2;
    msg1.Value3 = 3;
    msg1.Value4 = 4;
    msg1.Value5 = 5;

    unsigned int CrcResult;
    boost::crc_32_type computer;
    computer.process_bytes(&msg1, msg1.Size-sizeof(CrcResult));
    msg1.CrcResult = computer.checksum();


    unsigned int CrcResult2;
    boost::crc_32_type computer2;
    computer2.process_bytes(&msg1, msg1.Size-sizeof(CrcResult));
    CrcResult2 = computer2.checksum();

    LONGLONG Value = 1036042624;
    unsigned short crc1 = sc::string::getCrc16(&Value, sizeof(LONGLONG));
    
    //unsigned short crc1 = sc::string::getCrc16();


    return;
}

void CheckSsnHead()
{
    std::cout << "Age:" << sc::string::SsnHeadToAge("951010") << std::endl;
    std::cout << "Age:" << sc::string::SsnHeadToAge("730112") << std::endl;
    std::cout << "Age:" << sc::string::SsnHeadToAge("731022") << std::endl;
    std::cout << "Age:" << sc::string::SsnHeadToAge("891022") << std::endl;
    std::cout << "Age:" << sc::string::SsnHeadToAge("491022") << std::endl;
    std::cout << "Age:" << sc::string::SsnHeadToAge("891339") << std::endl;
    std::cout << "Age:" << sc::string::SsnHeadToAge("891100") << std::endl;
    std::cout << "Age:" << sc::string::SsnHeadToAge("890231") << std::endl;
    std::cout << "Age:" << sc::string::SsnHeadToAge("89") << std::endl;
    std::cout << "Age:" << sc::string::SsnHeadToAge("8") << std::endl;
    std::cout << "Age:" << sc::string::SsnHeadToAge("000000") << std::endl;
    std::cout << "Age:" << sc::string::SsnHeadToAge("9999890231") << std::endl;
}

std::string getSHA256(const std::string& strSrc)
{
    CryptoPP::SHA256 Sha256;    
    BYTE* OUTPUT = new BYTE[Sha256.DigestSize()];
    Sha256.CalculateDigest(OUTPUT, (const byte*) strSrc.c_str(), strSrc.size());
    std::string strHash(sc::string::HexEncodeA(OUTPUT, Sha256.DigestSize()));
    delete [] OUTPUT;
    return strHash;
}

//! Hash 값이 제대로 되었있는지는 아래 URL 에서 확인가능하다.
//! http://www.xorbin.com/tools/sha256-hash-calculator
void Sha256()
{
    std::string Ori("123abc");
    std::string Hash = getSHA256(Ori);
    std::cout << Hash << std::endl;
}

void BoostSplitTest()
{
    std::vector<std::string> VecStr;
    std::string TestStr("VERSION		1	2	3		4");    
    std::string Seperator(" ,\t");
    boost::split(VecStr, TestStr, boost::algorithm::is_any_of(Seperator), boost::algorithm::token_compress_on);
//     for (std::vector<std::string>::iterator iter=VecStr.begin(); iter!=VecStr.end(); )
//     {
//         if ((*iter).empty())
//             iter = VecStr.erase(iter);
//         else
//             ++iter;
//     }

    for (size_t i=0; i<VecStr.size(); ++i)
        std::cout << i << ":" << VecStr[i] << std::endl;
}

void UintTest()
{
    DWORD a = -1;
    if (a < 0)
        std::cout << "Oops a = -1" << std::endl;
    else
        std::cout << "a never less than 0" << std::endl;

    a = UINT_MAX;
    if (a < 0)
        std::cout << "Oops a = -1" << std::endl;
    else
        std::cout << "a never less than 0" << std::endl;    

    a = UINT_MAX+1;
    if (a < 0)
        std::cout << "Oops a = -1" << std::endl;
    else
        std::cout << "a never less than 0" << std::endl;

    a = UINT_MAX+2;
    if (a < 0)
        std::cout << "Oops a = -1" << std::endl;
    else
        std::cout << "a never less than 0" << std::endl;    

}

void SharedPtrTest()
{
    class TestClass
    {
    public:
        TestClass() {}
        ~TestClass() {}
        std::string m_Str;

    public:
        std::string GetStr() const { return m_Str; }
    };

    std::tr1::shared_ptr<TestClass> spTest(new TestClass); // use spTest count 1
    std::cout << "User Count:" << spTest.use_count() << std::endl;

    std::tr1::shared_ptr<TestClass> spTest2 = spTest;      // use spTest count 2
    std::cout << "User Count:" << spTest.use_count() << std::endl;

    std::tr1::shared_ptr<TestClass> spTest3 = spTest2;     // use spTest count 3
    std::cout << "User Count:" << spTest.use_count() << std::endl;

    spTest3.reset();                                       // use spTest count 2
    std::cout << "User Count:" << spTest.use_count() << std::endl;

    std::tr1::shared_ptr<TestClass> spTest4(new TestClass); // use spTest4 count 1
    spTest3 = spTest4;    // use spTest4 count 2
    std::cout << "User Count test4:" << spTest4.use_count() << std::endl;

    spTest3 = spTest; // use spTest count 3 // use spTest4 count 1
    std::cout << "User Count4:" << spTest4.use_count() << std::endl;
    std::cout << "User Count:" << spTest.use_count() << std::endl;

    TestClass Test5;
    Test5.m_Str = "Hello World";
    *spTest3.get() = Test5;
    std::cout << spTest3->GetStr() << std::endl;

}

void BadFormatTest()
{
    int a=3;
    LONGLONG x=100000000000000;
    const char* szTest = "bad bad bad";
    std::string Result = sc::string::format("hello %1%", x);    
    std::cout << Result << std::endl;    
    return;
}

std::string getAppPath()
{ 
    CString strFullPath;
    CString strCommandLine = GetCommandLine();
    if (!strCommandLine.IsEmpty())
    {
        std::string CommandLine(strCommandLine.GetString());
        std::vector<std::string> SplitVector;
        boost::split(SplitVector, CommandLine, boost::algorithm::is_any_of(" "));
        size_t FindPos=0;
        for (size_t i=0; i<SplitVector.size(); ++i)
        {
            std::transform(SplitVector[i].begin(), SplitVector[i].end(), SplitVector[i].begin(), towlower);
            if (std::string::npos != SplitVector[i].find(".exe"))
            {
                FindPos = i;
                break;
            }
        }

        strCommandLine = "";
        for (size_t i=0; i<=FindPos; ++i)
        {
            strCommandLine.Append(SplitVector[i].c_str());
            strCommandLine.Append(" ");
        }

        DWORD dwFind = strCommandLine.ReverseFind('\\');
        if (dwFind != -1)
        {
            strFullPath = strCommandLine.Left(dwFind);

            if (!strFullPath.IsEmpty())
                if (strFullPath.GetAt(0) == '"')
                    strFullPath = strFullPath.Right(strFullPath.GetLength() - 1);
        }
    }

    if (strFullPath.GetLength() < 4)
    {
        char szTest[MAX_PATH] = {0};
        GetCurrentDirectoryA(MAX_PATH, szTest);
        std::string strTest(szTest);
        return strTest;
    }
    else
    {
        return std::string(strFullPath.GetString());
    }
}

void AppPath()
{
    std::string AppPath = getAppPath();
    AfxMessageBox(AppPath.c_str(), MB_OK);
}
COdbcSupervisor* pOdbc;

void NoOdbcPool()
{
    for (size_t i=0; i<10; ++i)
    {
        pOdbc->ExecuteSQL(std::string("SELECT @@VERSION"));
        pOdbc->ExecuteSQL(std::string("SELECT @@VERSION"));
        pOdbc->ExecuteSQL(std::string("SELECT @@VERSION"));
        pOdbc->ExecuteSQL(std::string("SELECT @@VERSION"));
        pOdbc->ExecuteSQL(std::string("SELECT @@VERSION"));
        Sleep(100);
    }

    std::cout << "thread end" << std::endl;
}

COdbcSupervisor* pOdbcPool;

void OdbcPool()
{    
    for (size_t i=0; i<10; ++i)
    {
        pOdbcPool->ExecuteSQL(std::string("SELECT @@VERSION"));
        pOdbcPool->ExecuteSQL(std::string("SELECT @@VERSION"));
        pOdbcPool->ExecuteSQL(std::string("SELECT @@VERSION"));
        pOdbcPool->ExecuteSQL(std::string("SELECT @@VERSION"));
        pOdbcPool->ExecuteSQL(std::string("SELECT @@VERSION"));
        Sleep(50);
    }    

    std::cout << "thread end" << std::endl;
}

void OdbcPoolTest()
{
/*
    pOdbc = new COdbcSupervisor("RanLog", "RanLog", "RanLog@@", "RanLogS3");

    boost::thread a1(NoOdbcPool);
    boost::thread a2(NoOdbcPool);
    boost::thread a3(NoOdbcPool);
    boost::thread a4(NoOdbcPool);

    a4.join();
    a3.join();
    a2.join();
    a1.join();
    delete pOdbc;
*/

    pOdbcPool = new COdbcSupervisor("RanLog", "RanLog", "RanLog@@", "RanLogS3", true);
 
    boost::thread b1(OdbcPool);
    boost::thread b2(OdbcPool);
    boost::thread b3(OdbcPool);
    boost::thread b4(OdbcPool);
 
    Sleep(5000);

    delete pOdbcPool;

    COdbcEnv::ReleaseInstance();
}

void AutoTxtFileTest()
{
    std::string FileName = sc::getAppPath();
    FileName.append("\\Log\\TestLog.sql");
    sc::AutoTxtFile* pTxtFile = new sc::AutoTxtFile(FileName, 10000);
    for (size_t i=0; i<10000000; ++i)
    {
        std::string Log(            
                "INSERT INTO dbo.LogItemExchange(NIDMain,NIDSub,SGNum,SvrNum,FldNum,MakeType,MakeNum,ItemAmount,ItemFromFlag,ItemFrom,ItemToFlag,ItemTo,ExchangeFlag,ExchangeDate,Damage,Defense,Fire,Ice,Poison,Electric,Spirit,CostumeMID,CostumeSID,TradePrice) VALUES\
(NIDMain,NIDSub,SGNum,SvrNum,FldNum,MakeType,MakeNum,ItemAmount,ItemFromFlag,ItemFrom,ItemToFlag,ItemTo,ExchangeFlag,ExchangeDate,Damage,Defense,Fire,Ice,Poison,Electric,Spirit,CostumeMID,CostumeSID,TradePrice);");
        pTxtFile->write(Log);
    }
    delete pTxtFile;
}

typedef std::map<DWORD, DWORD> TEST_MAP;
typedef TEST_MAP::iterator                 TEST_MAP_ITER;
typedef TEST_MAP::const_iterator           TEST_MAP_CITER;
typedef TEST_MAP::value_type               TEST_MAP_VALUE;

TEST_MAP TempData;

void ContainerEraseTest2(DWORD Key);
void ContainerEraseTest()
{    
    TempData.insert(TEST_MAP_VALUE(1, 1));
    TempData.insert(TEST_MAP_VALUE(11, 1));
    TempData.insert(TEST_MAP_VALUE(20, 1));
    TempData.insert(TEST_MAP_VALUE(21, 1));
    TempData.insert(TEST_MAP_VALUE(30, 1));
    TempData.insert(TEST_MAP_VALUE(31, 1));
    TempData.insert(TEST_MAP_VALUE(40, 1));
    TempData.insert(TEST_MAP_VALUE(41, 1));
    TempData.insert(TEST_MAP_VALUE(50, 1));
    TempData.insert(TEST_MAP_VALUE(51, 1));

    for (TEST_MAP_ITER iter=TempData.begin(); iter!=TempData.end(); )
    {
        if (iter->first % 2)
        {
            TEST_MAP_ITER TempIter = iter++;
            ContainerEraseTest2(TempIter->first);
        }
        else
        {
            ++iter;
        }
    }
}

void ContainerEraseTest2(DWORD Key)
{
    TEST_MAP_ITER iter = TempData.find(Key);
    if (iter != TempData.end())
        TempData.erase(iter);
}

struct PARSE_RESULT
{
    bool m_Mark;
    std::string m_Data;

    PARSE_RESULT()
        : m_Mark(false)
    {
    }
};

void ParseBody(const std::string& OriginalStr, char StartMark, char EndMark, std::vector<PARSE_RESULT>& SplitStr) 
{
    bool bMark = false;
    std::string SubStr;
    string::const_iterator iterator1 = OriginalStr.begin();    
    while (iterator1 != OriginalStr.end()) 
    {
        if (*iterator1 == StartMark && bMark == false)
        {
            if (!SubStr.empty())
            {
                PARSE_RESULT Result;
                Result.m_Mark = false;
                Result.m_Data = SubStr;
                SplitStr.push_back(Result);
            }
            bMark = true;
            SubStr.clear();
            SubStr.append(1, *iterator1);
        }
        else if (*iterator1 == EndMark && bMark == true)
        {
            SubStr.append(1, *iterator1);
            if (!SubStr.empty())
            {
                PARSE_RESULT Result;
                Result.m_Mark = true;
                Result.m_Data = SubStr;
                SplitStr.push_back(Result);
            }
            SubStr.clear();
            bMark = false;
        }
        else
        {
            SubStr.append(1, *iterator1);
        }
        ++iterator1;
    }
    if (!SubStr.empty())
    {
        PARSE_RESULT Result;
        Result.m_Mark = bMark;
        Result.m_Data = SubStr;
        SplitStr.push_back(Result);        
    }    
}

void ParseTest()
{
    std::string Test ("a[bc] d[e] [f] ghijkl[mnopqrs]1245");
    std::string Test1("[bc] d[e] [f] ghijkl[mnopqrs]1245");
    std::string Test2("[bc]");
    std::string Test3("[bc] d[e] [f] ghijkl[mnopqrs]");
    std::string Test4("[bc] d[e] [f ghijkl[mnopqrs]");
    std::string Test5("[bc [de]] [f ghijkl[mnopqrs]");

    std::vector<PARSE_RESULT> SplitStr;
    ParseBody(Test,  '[', ']', SplitStr);
    ParseBody(Test1, '[', ']', SplitStr);
    ParseBody(Test2, '[', ']', SplitStr);
    ParseBody(Test3, '[', ']', SplitStr);
    ParseBody(Test4, '[', ']', SplitStr);
    ParseBody(Test5, '[', ']', SplitStr);

    return;
}

void MultiIndexTest()
{
    CharContainer CharMap;

    CHARACTER* pChar = new CHARACTER(1, 2, 21);
    CharMap.insert(pChar);    
}

bool SQLiteTest()
{	
	/*private_market::GLPrivateMarketSearchMan* pSQL = new private_market::GLPrivateMarketSearchMan;

	DWORD dwChaDbNum1 = 201;
	std::string strChaName1( "꼴통" );
	SNATIVEID sMapID1( 1, 1 );
	WORD wChaSchool1 = 1;
	WORD wChaChannel1 = 2;
	DWORD dwFieldSlot1 = 5;

	DWORD dwChaDbNum2 = 203040;
	std::string strChaName2( "루시르" );
	SNATIVEID sMapID2( 10, 10 );
	WORD wChaSchool2 = 1;
	WORD wChaChannel2 = 1;
	DWORD dwFieldSlot2 = 4;

	DWORD dwChaDbNum3 = 12905;
	std::string strChaName3( "데지" );
	SNATIVEID sMapID3( 2, 10 );
	WORD wChaSchool3 = 2;
	WORD wChaChannel3 = 1;
	DWORD dwFieldSlot3 = 3;

	pSQL->InsertSeller( dwChaDbNum1, strChaName1.c_str(), sMapID1, wChaSchool1, wChaChannel1, dwFieldSlot1 );
	pSQL->InsertSeller( dwChaDbNum2, strChaName2.c_str(), sMapID2, wChaSchool2, wChaChannel2, dwFieldSlot2 );
	pSQL->InsertSeller( dwChaDbNum3, strChaName3.c_str(), sMapID3, wChaSchool3, wChaChannel3, dwFieldSlot3 );

	{
		WORD wSchool = 1;
		WORD wChannel = 0;
		DWORD dwFieldSlot = 5;
		DWORD dwChaNum = 10902390;
		UUID sItemGUID;
		sc::string::getUUID( sItemGUID );
		SITEMCUSTOM sItemcustom( SNATIVEID( 76, 3 ) );
		sItemcustom.GENERATE_RANDOM_OPT( true );
		LONGLONG llPrice = 45670;
		DWORD dwTurnNum = 1;

		pSQL->InsertSaleItem( dwChaDbNum1, sItemGUID, sItemcustom, dwTurnNum, llPrice );
	}

	{
		WORD wSchool = 1;
		WORD wChannel = 0;
		DWORD dwFieldSlot = 5;
		DWORD dwChaNum = 10902390;
		UUID sItemGUID;
		sc::string::getUUID( sItemGUID );
		SITEMCUSTOM sItemcustom( SNATIVEID( 1, 24 ) );	// 천기궁
		sItemcustom.GENERATE_RANDOM_OPT( true );
		LONGLONG llPrice = 10000;
		DWORD dwTurnNum = 1;

		pSQL->InsertSaleItem( dwChaDbNum1, sItemGUID, sItemcustom, dwTurnNum, llPrice );
	}

	{
		WORD wSchool = 1;
		WORD wChannel = 0;
		DWORD dwFieldSlot = 5;
		DWORD dwChaNum = 230;
		UUID sItemGUID;
		sc::string::getUUID( sItemGUID );
		SITEMCUSTOM sItemcustom( SNATIVEID( 1, 10 ) );	// 피룡도
		sItemcustom.GENERATE_RANDOM_OPT( true );
		LONGLONG llPrice = 8900;
		DWORD dwTurnNum = 1;

		pSQL->InsertSaleItem( dwChaDbNum1, sItemGUID, sItemcustom, dwTurnNum, llPrice );
	}

	{
		WORD wSchool = 1;
		WORD wChannel = 0;
		DWORD dwFieldSlot = 5;
		DWORD dwChaNum = 230;
		UUID sItemGUID;
		sc::string::getUUID( sItemGUID );
		SITEMCUSTOM sItemcustom( SNATIVEID( 1, 27 ) );	// 구리경락반지
		sItemcustom.GENERATE_RANDOM_OPT( true );
		LONGLONG llPrice = 5000;
		DWORD dwTurnNum = 1;

		pSQL->InsertSaleItem( dwChaDbNum2, sItemGUID, sItemcustom, dwTurnNum, llPrice );
	}

	{
		WORD wSchool = 1;
		WORD wChannel = 0;
		DWORD dwFieldSlot = 5;
		DWORD dwChaNum = 100;
		UUID sItemGUID;
		sc::string::getUUID( sItemGUID );
		SITEMCUSTOM sItemcustom( SNATIVEID( 1, 33 ) );	// 용명검
		sItemcustom.GENERATE_RANDOM_OPT( true );
		LONGLONG llPrice = 12350;
		DWORD dwTurnNum = 1;

		pSQL->InsertSaleItem( dwChaDbNum2, sItemGUID, sItemcustom, dwTurnNum, llPrice );
	}

	{
		WORD wSchool = 1;
		WORD wChannel = 0;
		DWORD dwFieldSlot = 5;
		DWORD dwChaNum = 110;
		UUID sItemGUID;
		sc::string::getUUID( sItemGUID );
		SITEMCUSTOM sItemcustom( SNATIVEID( 1, 7 ) );	// 수호 목장궁
		sItemcustom.GENERATE_RANDOM_OPT( true );
		LONGLONG llPrice = 900;
		DWORD dwTurnNum = 1;

		pSQL->InsertSaleItem( dwChaDbNum2, sItemGUID, sItemcustom, dwTurnNum, llPrice );
	}

	{
		WORD wSchool = 1;
		WORD wChannel = 0;
		DWORD dwFieldSlot = 5;
		DWORD dwChaNum = 304050;
		UUID sItemGUID;
		sc::string::getUUID( sItemGUID );
		SITEMCUSTOM sItemcustom( SNATIVEID( 32, 9 ) );	// 전투 교복장갑
		sItemcustom.GENERATE_RANDOM_OPT( true );
		LONGLONG llPrice = 89000;
		DWORD dwTurnNum = 1;

		pSQL->InsertSaleItem( dwChaDbNum3, sItemGUID, sItemcustom, dwTurnNum, llPrice );
	}

	{
		WORD wSchool = 1;
		WORD wChannel = 0;
		DWORD dwFieldSlot = 5;
		DWORD dwChaNum = 345;
		UUID sItemGUID;
		sc::string::getUUID( sItemGUID );
		SITEMCUSTOM sItemcustom( SNATIVEID( 32, 7 ) );	// 중급 트레이닝장갑
		sItemcustom.GENERATE_RANDOM_OPT( true );
		LONGLONG llPrice = 5600;
		DWORD dwTurnNum = 1;

		pSQL->InsertSaleItem( dwChaDbNum3, sItemGUID, sItemcustom, dwTurnNum, llPrice );
	}

	{
		WORD wSchool = 1;
		WORD wChannel = 0;
		DWORD dwFieldSlot = 5;
		DWORD dwChaNum = 345;
		UUID sItemGUID;
		sc::string::getUUID( sItemGUID );
		SITEMCUSTOM sItemcustom( SNATIVEID( 34, 2 ) );	// 중급 검도서클장갑
		sItemcustom.GENERATE_RANDOM_OPT( true );
		LONGLONG llPrice = 300;
		DWORD dwTurnNum = 1;

		pSQL->InsertSaleItem( dwChaDbNum3, sItemGUID, sItemcustom, dwTurnNum, llPrice );
	}

	{
		WORD wSchool = 1;
		WORD wChannel = 0;
		DWORD dwFieldSlot = 5;
		DWORD dwChaNum = 21390;
		UUID sItemGUID;
		sc::string::getUUID( sItemGUID );
		SITEMCUSTOM sItemcustom( SNATIVEID( 38, 4 ) );	// 중급 강화 하복(상의)
		sItemcustom.GENERATE_RANDOM_OPT( true );
		LONGLONG llPrice = 5000;
		DWORD dwTurnNum = 1;

		pSQL->InsertSaleItem( dwChaDbNum3, sItemGUID, sItemcustom, dwTurnNum, llPrice );
	}

	{
		WORD wSchool = 1;
		WORD wChannel = 0;
		DWORD dwFieldSlot = 5;
		DWORD dwChaNum = 21390;
		UUID sItemGUID;
		sc::string::getUUID( sItemGUID );
		SITEMCUSTOM sItemcustom( SNATIVEID( 356, 47 ) );
		sItemcustom.GENERATE_RANDOM_OPT( true );
		LONGLONG llPrice = 7001;
		DWORD dwTurnNum = 1;

		pSQL->InsertSaleItem( dwChaDbNum1, sItemGUID, sItemcustom, dwTurnNum, llPrice );
	}

	{
		WORD wSchool = 1;
		WORD wChannel = 0;
		DWORD dwFieldSlot = 5;
		DWORD dwChaNum = 21390;
		UUID sItemGUID;
		sc::string::getUUID( sItemGUID );
		SITEMCUSTOM sItemcustom( SNATIVEID( 356, 48 ) );
		sItemcustom.GENERATE_RANDOM_OPT( true );
		LONGLONG llPrice = 7002;
		DWORD dwTurnNum = 1;

		pSQL->InsertSaleItem( dwChaDbNum1, sItemGUID, sItemcustom, dwTurnNum, llPrice );
	}

	{
		WORD wSchool = 1;
		WORD wChannel = 0;
		DWORD dwFieldSlot = 5;
		DWORD dwChaNum = 21390;
		UUID sItemGUID;
		sc::string::getUUID( sItemGUID );
		SITEMCUSTOM sItemcustom( SNATIVEID( 356, 49 ) );
		sItemcustom.GENERATE_RANDOM_OPT( true );
		LONGLONG llPrice = 7003;
		DWORD dwTurnNum = 1;

		pSQL->InsertSaleItem( dwChaDbNum2, sItemGUID, sItemcustom, dwTurnNum, llPrice );
	}*/

	{
		/*
		int nType;
		int nWeaponType;
		int nArmorType;
		int nRequire;
		int nRank;

		std::vector< sSaleItemSearchOptionNumeric > vecBasic;
		std::vector< sSaleItemSearchOptionFloat > vecAddonVarRandom;

		int nRequireValue;

		bool bTimelimit;

		int nSortOption;
		int nSortMethod;
		*/

		using namespace private_market;


		sSaleItemSearchandSortBasic sOption1, sOption2;
		
		{
			int nChannel = 0;
			WORD wSchool = 0;
			sOption1.nType = eItemTypeAll;
			sOption1.nWeaponType = eItemWeaponTypeAll;
			sOption1.nArmorType = eItemArmorTypeAll;
			sOption1.nRequire = eItemRequireNone;
			sOption1.nRequireValue = 100;
			sOption1.nRank = eItemRankAll;
			sOption1.bTimelimit = false;
			sOption1.nSortOption = eItemSortOptionName;
			sOption1.nSortMethod = eItemSortMethodDesc;
			sOption1.nRequireClass = eItemRequireClassAll;
			sOption1.nRequireSex = eItemRequireSexAll;

			sSaleItemSearchOptionNumeric sn1;
			sn1.nType = eItemBasicDamage;
			sn1.nValue = 0;

			sSaleItemSearchOptionNumeric sn2;
			sn2.nType = eItemBasicHitrate;
			sn2.nValue = 0;

			sOption1.vecBasic.push_back( sn1 );
			sOption1.vecBasic.push_back( sn2 );

			sSaleItemSearchOptionFloat sf1;
			sf1.nType = eItemAddonRandomStatsDex;
			sf1.fValue = 0.0f;

			sOption1.vecAddonVarRandom.push_back( sf1 );

			sOption1.wPage = 2;
		}

		{
			int nChannel = 0;
			WORD wSchool = 0;
			sOption2.nType = eItemTypeAll;
			sOption2.nWeaponType = eItemWeaponTypeAll;
			sOption2.nArmorType = eItemArmorTypeAll;
			sOption2.nRequire = eItemRequireNone;
			sOption2.nRequireValue = 100;
			sOption2.nRank = eItemRankAll;
			sOption2.bTimelimit = false;
			sOption2.nSortOption = eItemSortOptionName;
			sOption2.nSortMethod = eItemSortMethodDesc;
			sOption2.nRequireClass = eItemRequireClassAll;
			sOption2.nRequireSex = eItemRequireSexAll;

			sSaleItemSearchOptionNumeric sn1;
			sn1.nType = eItemBasicDamage;
			sn1.nValue = 0;

			sSaleItemSearchOptionNumeric sn2;
			sn2.nType = eItemBasicHitrate;
			sn2.nValue = 0;

			sOption2.vecBasic.push_back( sn1 );
			sOption2.vecBasic.push_back( sn2 );

			sSaleItemSearchOptionFloat sf1;
			sf1.nType = eItemAddonRandomStatsDex;
			sf1.fValue = 0.0f;

			sSaleItemSearchOptionFloat sf2;
			sf2.nType = eItemAddonRandomStatsStr;
			sf2.fValue = 1.0f;

			sOption2.vecAddonVarRandom.push_back( sf1 );
			sOption2.vecAddonVarRandom.push_back( sf2 );

			sOption2.wPage = 4;
		}

		int nEQ = 0;
		if ( sOption1 == sOption2 )
		{
			nEQ = 1;
		}

		/*sSaleItemSearchandSortBasic sOption;
		int nChannel = 0;
		WORD wSchool = 0;
		sOption.nType = eItemTypeAll;
		sOption.nWeaponType = eItemWeaponTypeAll;
		sOption.nArmorType = eItemArmorTypeAll;
		sOption.nRequire = eItemRequireNone;
		sOption.nRequireValue = 100;
		sOption.nRank = eItemRankAll;
		sOption.bTimelimit = false;
		sOption.nSortOption = eItemSortOptionName;
		sOption.nSortMethod = eItemSortMethodDesc;
		sOption.nRequireClass = eItemRequireClassAll;
		sOption.nRequireSex = eItemRequireSexAll;

		sSaleItemSearchOptionNumeric sn1;
		sn1.nType = eItemBasicDamage;
		sn1.nValue = 0;

		sSaleItemSearchOptionNumeric sn2;
		sn2.nType = eItemBasicHitrate;
		sn2.nValue = 0;

		sOption.vecBasic.push_back( sn1 );
		sOption.vecBasic.push_back( sn2 );

		sSaleItemSearchOptionFloat sf1;
		sf1.nType = eItemAddonRandomStatsDex;
		sf1.fValue = 0.0f;

		sOption.vecAddonVarRandom.push_back( sf1 );*/
		
		/*std::vector< sSaleItem > vecSaleItems;
		pSQL->GetSaleItems( 1, nChannel, wSchool, sOption, vecSaleItems );*/

	}

	//delete pSQL;

	return true;
}
