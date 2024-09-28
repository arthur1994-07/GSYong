#ifndef PVESystemQuery_H
#define PVESystemQuery_H

/**
	테이블 생성 쿼리
	Primary Key 는 캐릭터 ID 와 맵 ID 를 조합한 문자열임에 주의 한다.

 */
static const TSTRING strCreateQuery =	    _T("CREATE TABLE IF NOT EXISTS MapInfo(")
										    _T("MapInfoID STRING NOT NULL, ")
										    _T("ChaNum INTEGER NOT NULL, ")
										    _T("MapID INTEGER NOT NULL, ")
										    _T("InCount INTEGER NOT NULL, ")
										    _T("GenCount INTEGER NOT NULL, ")
                                            _T("Level INTEGER NOT NULL, ")
										    _T("GenTime TIME NOT NULL, ")
										    _T("PRIMARY KEY(MapInfoID));")
										    _T("CREATE UNIQUE INDEX IF NOT EXISTS Idx_MapInfo_MapInfoID ON MapInfo(MapInfoID);")
                                            _T("CREATE INDEX IF NOT EXISTS Idx_MapInfo_ChaNum ON MapInfo(ChaNum);");

/**
	레코드 검색 쿼리

	strSelectMainQuery : 유일 구분자(MapInfoID)를 통해 맵 정보를 얻는 쿼리(1개만 얻을 수 있음)
	strSelectSubQuery : 캐릭터ID(ChaNum)를 통해 맵 정보를 얻는 쿼리(여러개를 얻을 수 있음)
 */
static const TSTRING strSelectMainQuery =	_T("SELECT MapInfoID, ChaNum, MapID, InCount, GenCount, Level, GenTime ")
										    _T("FROM MapInfo WHERE MapInfoID = \"%1%\";");
static const TSTRING strSelectSubQuery =	_T("SELECT MapInfoID, ChaNum, MapID, InCount, GenCount, Level, GenTime ")
                                            _T("FROM MapInfo WHERE ChaNum = %1%;");

/**
	레코드 삽입 쿼리

 */
static const TSTRING strInsertQuery =	    _T("INSERT INTO MapInfo(MapInfoID, ChaNum, MapID, InCount, GenCount, Level, GenTime)")
										    _T("VALUES(\"%1%\", %2%, %3%, %4%, %5%, %6%, %7%);");

/**
	레코드 업데이트(갱신) 쿼리

 */
static const TSTRING strUpdateQuery =	    _T("UPDATE MapInfo ")
										    _T("SET MapInfoID = \"%1%\", ChaNum = %2%, MapID = %3%, InCount = %4%, GenCount = %5%, Level = %6%, GenTime = %7% ")
										    _T("WHERE MapInfoID = \"%1%\";");

#endif
