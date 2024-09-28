#pragma once

#include "../../SigmaCore/Util/GLSingletone.h"

namespace Tooltip
{

	class GLItemTooltipMan : public CSingletone< GLItemTooltipMan > 
	{
	protected:
		friend CSingletone;

	private:
		GLItemTooltipMan ();
		~GLItemTooltipMan ();

	public:
		void Clear ();
		void GenerateItemTooltip ();
		void NextLine ();
		void CutZero ( CString& cstr, int nCount );

	private:
		// ������ �⺻ ����;
		void MakeItemDescriptionBasicInfo ();
		void MakeItemDescriptionSchool ();				// �п� ����;
		void MakeItemDescriptionClass ();				// Ŭ���� ����;
		void MakeItemDescriptionSex ();					// ���� ����;

		// ������ �߰� ����;
		bool MakeItemDescriptionRequirement ();			// �䱸���� ����;
		bool MakeItemDescriptionType ();				// Ÿ�� ����;

		// ������ �⺻ �ɷ�ġ;
		bool MakeItemDescriptionAttack ();				// ���ݷ� ����;
		bool MakeItemDescriptionDefense ();				// ���� ����;
		bool MakeItemDescriptionEtc ();					// �׿� ����;

		// ������ �ΰ� �ɷ�ġ;
		bool MakeItemDescriptionAddValue ();			// ���� ��ġ;
		bool MakeItemDescriptionResist ();				// ����;
		bool MakeItemDescriptionBlow ();				// �����̻�;

		// ��Ʈ������;
		void MakeItemDescriptionSetItem ();
		bool MakeItemDescriptionSetItemName ( const SSETITEM *sSetItem );
		bool MakeItemDescriptionSetItemParts ( const SSETITEM *sSetItem );
		bool MakeItemDescriptionSetItemBonus ( const SSETITEM *sSetItem );

	public:
		virtual void SetItem ( SITEM* pItem ) { m_pItemData = pItem; }
		virtual const std::string& GetItemDescription () const { return m_strItemDescription; }
		virtual const std::string& GetItemDescriptionPiece () const { return m_strItemDescriptionPiece; }

	private:
		// ������ ����;
		SITEM* m_pItemData;

		// ��ü ���� �ؽ�Ʈ;
		std::string m_strItemDescription;

		// MakeItemDescription �Լ��� ȣ���� ��;
		// �� �Լ��� Description ������ �ӽ÷� �����Ѵ�;
		// �����ġ ������ �ؽ�Ʈȭ�ؼ� �˰�ʹٸ�;
		// MakeItemDescriptionAddValue();
		// std::string strTooltip = GetItemDescriptionPiece();
		std::string m_strItemDescriptionPiece;
	};

}