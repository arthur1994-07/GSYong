#pragma once

#include "../../SigmaCore/Database/Sqlite/SQLite.h"

// 문제 발생 -> 현재 필드서버의 캐릭터들 sqlite 에 저장 -> 서버 자동 종료 -> 서버 재시작 -> sqlite 의 캐릭터들 실제 게임데이터베이스에 저장
// 필드서버에 ado not enough storage is available to complete this operation 에러가 발생하면
// 현재 필드서버의 데이터베이스 관련 모든 작업이 되지 않는다.
// 캐릭터 저장이 안되며, 저장을 시도하면 바이너리 데이터가 손상이 되는 경우가 많다.
// 데이터베이스 작업을 처리하다가 ado E_OUT_OF_MEMORY 에러가 발생하면(DbExecuter) 더 이상 Execute 를 진행하지 않고
// dbaction 이름이 CharacterSave 라면 그안에 있는 캐릭터 정보를 sqlite 에 저장한다.
// 필드서버가 강제로 멈추게 되고, 필드서버가 종료관련 작업을 처리한다. 종료 처리 중에 캐릭터 저장작업이 있는데
// 그 작업이 dbaction CharacterSave 이다. 이것은 DbExecuter 로 들어온다. 들어오면 sqlite 로 저장이 된다.
// 저장과정은 다음과 같다.
// SCHARDATA2 를 serialize 한다. 그 결과물을 string 으로 받아서 압축을 한다.(zlib) 압축한 문자열을 base64 encode 한다. 그 문자열을 sqlite text 자료형 칼럼에 insert 한다.
// 압축하지 않으면 크기가 너무 크고, base64 encode 를 하지않으면 sqlite text 칼럼 특성상 /0 문자열 문제로 정상적으로 나중에 읽어올 수가 없다.
// 저장된 데이터는 서버가 시작될 때 테이블을 읽어서 데이터가 있다면
// 저장 반대 순서로 base64 decode -> 압축 해제 -> serialize 되어 있는 문자열 SCHARDATA2 에 쓰기 SCHARDATA2 로 db 에 SaveCharacter 호출하기 순으로 게임디비에 저장을 한다.

namespace db
{
	class IDbManager;
}

class CharacterBackup
{
public:
	enum eCommonError
	{
		eSuccess = 0, 
		eChaIsNull, 
		eExecute, 
		eOpen, 
		ePrepare, 
		eBind, 
		eStep, 
		eStringEmpty, 
		eWrongItem, 
		eNullTablePointer, 
		eDBManNull, 
		eCompress, 
		eDecompress, 
	};

public:
	CharacterBackup( db::IDbManager* pDBMan );
	~CharacterBackup();

public:
	int SaveCharacterInSqlite( SCHARDATA2* pCharacter );
	int SaveCharacterInGameDatabase();
	std::vector< DWORD >& GetSavedUserNum() { return m_SavedUserNum; }

protected:
	int open();
	void close();
	int executeSQL( const std::string& strQuery );

protected:
	db::IDbManager* m_pAdoMan;
	SQLite::Database m_Character;
	sqlite3_stmt* m_stmtForCharacterInsert;
	sqlite3_stmt* m_stmtForCharacterUpdate;
	std::vector< DWORD > m_SavedUserNum;

};
