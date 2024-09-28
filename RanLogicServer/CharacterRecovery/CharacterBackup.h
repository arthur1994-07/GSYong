#pragma once

#include "../../SigmaCore/Database/Sqlite/SQLite.h"

// ���� �߻� -> ���� �ʵ弭���� ĳ���͵� sqlite �� ���� -> ���� �ڵ� ���� -> ���� ����� -> sqlite �� ĳ���͵� ���� ���ӵ����ͺ��̽��� ����
// �ʵ弭���� ado not enough storage is available to complete this operation ������ �߻��ϸ�
// ���� �ʵ弭���� �����ͺ��̽� ���� ��� �۾��� ���� �ʴ´�.
// ĳ���� ������ �ȵǸ�, ������ �õ��ϸ� ���̳ʸ� �����Ͱ� �ջ��� �Ǵ� ��찡 ����.
// �����ͺ��̽� �۾��� ó���ϴٰ� ado E_OUT_OF_MEMORY ������ �߻��ϸ�(DbExecuter) �� �̻� Execute �� �������� �ʰ�
// dbaction �̸��� CharacterSave ��� �׾ȿ� �ִ� ĳ���� ������ sqlite �� �����Ѵ�.
// �ʵ弭���� ������ ���߰� �ǰ�, �ʵ弭���� ������� �۾��� ó���Ѵ�. ���� ó�� �߿� ĳ���� �����۾��� �ִµ�
// �� �۾��� dbaction CharacterSave �̴�. �̰��� DbExecuter �� ���´�. ������ sqlite �� ������ �ȴ�.
// ��������� ������ ����.
// SCHARDATA2 �� serialize �Ѵ�. �� ������� string ���� �޾Ƽ� ������ �Ѵ�.(zlib) ������ ���ڿ��� base64 encode �Ѵ�. �� ���ڿ��� sqlite text �ڷ��� Į���� insert �Ѵ�.
// �������� ������ ũ�Ⱑ �ʹ� ũ��, base64 encode �� ���������� sqlite text Į�� Ư���� /0 ���ڿ� ������ ���������� ���߿� �о�� ���� ����.
// ����� �����ʹ� ������ ���۵� �� ���̺��� �о �����Ͱ� �ִٸ�
// ���� �ݴ� ������ base64 decode -> ���� ���� -> serialize �Ǿ� �ִ� ���ڿ� SCHARDATA2 �� ���� SCHARDATA2 �� db �� SaveCharacter ȣ���ϱ� ������ ���ӵ�� ������ �Ѵ�.

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
