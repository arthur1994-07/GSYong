#ifndef PVESystemQuery_H
#define PVESystemQuery_H

/**
	���̺� ���� ����
	Primary Key �� ĳ���� ID �� �� ID �� ������ ���ڿ��ӿ� ���� �Ѵ�.

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
	���ڵ� �˻� ����

	strSelectMainQuery : ���� ������(MapInfoID)�� ���� �� ������ ��� ����(1���� ���� �� ����)
	strSelectSubQuery : ĳ����ID(ChaNum)�� ���� �� ������ ��� ����(�������� ���� �� ����)
 */
static const TSTRING strSelectMainQuery =	_T("SELECT MapInfoID, ChaNum, MapID, InCount, GenCount, Level, GenTime ")
										    _T("FROM MapInfo WHERE MapInfoID = \"%1%\";");
static const TSTRING strSelectSubQuery =	_T("SELECT MapInfoID, ChaNum, MapID, InCount, GenCount, Level, GenTime ")
                                            _T("FROM MapInfo WHERE ChaNum = %1%;");

/**
	���ڵ� ���� ����

 */
static const TSTRING strInsertQuery =	    _T("INSERT INTO MapInfo(MapInfoID, ChaNum, MapID, InCount, GenCount, Level, GenTime)")
										    _T("VALUES(\"%1%\", %2%, %3%, %4%, %5%, %6%, %7%);");

/**
	���ڵ� ������Ʈ(����) ����

 */
static const TSTRING strUpdateQuery =	    _T("UPDATE MapInfo ")
										    _T("SET MapInfoID = \"%1%\", ChaNum = %2%, MapID = %3%, InCount = %4%, GenCount = %5%, Level = %6%, GenTime = %7% ")
										    _T("WHERE MapInfoID = \"%1%\";");

#endif
