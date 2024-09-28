#ifndef PVEDB_H
#define PVEDB_H

/**
    DB 갱신을 위한 정보를 가진다.
    우리는 DB 를 일갱신 시킨다.

 */
struct SPVEDBFile 
{
    TSTRING strFile;
    int nYear;
    int nMonth;
    int nDay;
	int nHour;	
};

/**
    DB 파일명을 처리 한다.
    역시나 한곳에서 파일명을 처리함으로써 유연하게 대처할 수 있다.

 */
const TCHAR* GetPVEDBFile();

/**
    DB 파일이 존재하는지 체크 한다.
    우리는 이를 통해 초기화를 체크 할 수 있다.

 */
bool IsPVEDBFile();

/**
	DB 파일을 초기화 한다.

 */
void ResetPVEDBFile();

#endif