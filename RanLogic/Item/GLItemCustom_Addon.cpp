#include "../pch.h"
#include "../RandomAddon/RandomAddon.h"
#include "../RandomAddon/RandomAddonDef.h"
 
#include "../../SigmaCore/Math/Random.h"

#include "./GLItemDef.h"
#include "./GLItem.h"
#include "./GLItem_RandomGenOp_Utility.h"
#include "./GLItemCustom_Addon.h"

SITEMCUSTOM_ADDON::SITEMCUSTOM_ADDON(void) 
: m_sADDON_0(EMITEM_ADDON::EMADD_NOTINIT)	// �� �ʱ�ȭ �������� �ǹ�;
, m_sADDON_1(EMITEM_ADDON::EMADD_NOTINIT)
, m_sADDON_2(EMITEM_ADDON::EMADD_NOTINIT)
, m_sADDON_3(EMITEM_ADDON::EMADD_NOTINIT)
, m_sADDON_4(EMITEM_ADDON::EMADD_NOTINIT)
, m_sADDON_5(EMITEM_ADDON::EMADD_NOTINIT)
{
}

SITEMCUSTOM_ADDON::~SITEMCUSTOM_ADDON(void) 
{
}

bool SITEMCUSTOM_ADDON::IsInit(void) const
{
	if ( m_sADDON[0].emTYPE == EMITEM_ADDON::EMADD_NOTINIT
		&& m_sADDON[1].emTYPE == EMITEM_ADDON::EMADD_NOTINIT
		&& m_sADDON[2].emTYPE == EMITEM_ADDON::EMADD_NOTINIT
		&& m_sADDON[3].emTYPE == EMITEM_ADDON::EMADD_NOTINIT
		&& m_sADDON[4].emTYPE == EMITEM_ADDON::EMADD_NOTINIT
		&& m_sADDON[5].emTYPE == EMITEM_ADDON::EMADD_NOTINIT )
		return false;

	return true;
}

bool SITEMCUSTOM_ADDON::IsEmpty(void) const
{
	if (   ( (m_sADDON[0].emTYPE == EMITEM_ADDON::EMADD_NOTINIT ) ||  (m_sADDON[0].emTYPE == EMITEM_ADDON::EMADD_NONE) )  
		&& ( (m_sADDON[1].emTYPE == EMITEM_ADDON::EMADD_NOTINIT ) ||  (m_sADDON[1].emTYPE == EMITEM_ADDON::EMADD_NONE) )
		&& ( (m_sADDON[2].emTYPE == EMITEM_ADDON::EMADD_NOTINIT ) ||  (m_sADDON[2].emTYPE == EMITEM_ADDON::EMADD_NONE) )
		&& ( (m_sADDON[3].emTYPE == EMITEM_ADDON::EMADD_NOTINIT ) ||  (m_sADDON[3].emTYPE == EMITEM_ADDON::EMADD_NONE) )
		&& ( (m_sADDON[4].emTYPE == EMITEM_ADDON::EMADD_NOTINIT ) ||  (m_sADDON[4].emTYPE == EMITEM_ADDON::EMADD_NONE) )
		&& ( (m_sADDON[5].emTYPE == EMITEM_ADDON::EMADD_NOTINIT ) ||  (m_sADDON[5].emTYPE == EMITEM_ADDON::EMADD_NONE) ) )
		return true;
	
	return false;
}

void SITEMCUSTOM_ADDON::ResetAddon(void)
{
	for ( INT i(0); (i < SITEMCUSTOM_ADDON::ADDON_SIZE); ++i )
		m_sADDON[i] = ITEM::SADDON();
}

DWORD SITEMCUSTOM_ADDON::GetOptType(DWORD dwIndex) const
{
	return m_sADDON[dwIndex].emTYPE;
}

const UINT SITEMCUSTOM_ADDON::GetOptIndex ( const BYTE _nOptionType ) const
{
	for ( UINT i = 0; i < SITEMCUSTOM_ADDON::ADDON_SIZE; ++i )
	{
		if ( _nOptionType == m_sADDON[i].emTYPE )
			return i;
	}

	return SITEMCUSTOM_ADDON::ADDON_SIZE;
}

void SITEMCUSTOM_ADDON::DefaultInitialize( const SNATIVEID& sNID )
{
	DefaultInitialize(GLItemMan::GetInstance().GetItem(sNID));
}

void SITEMCUSTOM_ADDON::DefaultInitialize( const SITEM* pItem )
{
	if ( !pItem )
		return;

	const INT nMax(min(SITEMCUSTOM_ADDON::ADDON_SIZE, ITEM::SSUIT::ADDON_SIZE ));
	for ( INT i = 0; i < nMax; ++i )
	{
		m_sADDON[i] = pItem->sSuitOp.sADDON[i];
	}

	//this->Generate_RandomAddon(RandomAddon::GENERATE_GEN|RandomAddon::GENERATE_ONLYRANDOM, pItem);
}

// �����ɼǰ� ����� ��Ŀ�������� ���ư���;
bool SITEMCUSTOM_ADDON::Generate_RandomAddon( DWORD dwGenerateOp, const SITEM* const pItem, const ITEM_RANDOMOP_UTILL::FixOption& fixOption, SITEM* pItemStamp )
{
	RandomAddon::Entity sEntity;
	if ( !pItem || E_FAIL == RandomAddon::Manager::GetInstance().Find( pItem->sSuitOp.szRandomAddonScript, sEntity ) )
		return false;

	RandomAddon::GENERATE_OPTION emGenerateOp = static_cast<RandomAddon::GENERATE_OPTION>(dwGenerateOp);

	// �ɼ� �߻�Ȯ�� �˻�;
	if ( (emGenerateOp & RandomAddon::GENERATE_GEN) 
		&& !sc::Random::getInstance().IsSucceed(sEntity.GetGenerateRate()) )
		return false;

	// �߻����� Ƚ�� �̸� �ľǾ��Ѵ�, ���Ҽ� ������ŭ ���س���, �о���� �����ִ¸�ŭ �о�ִ´�;
	// ȿ���鿡�� ���ݴ� �پ���� �ڵ� ���������־ �״� �����ΰŰ���;

	const RandomAddon::VEC_BUNCH& vecGenerateBunch = emGenerateOp & RandomAddon::GENERATE_GEN ? sEntity.vecBunchDrop : sEntity.vecBunchRemodel;
	const INT nBunchSize(vecGenerateBunch.size());
	//	������ �ɼ��� ���� ������ �ο� �ϱ� ���ؼ� ������ ����;
	std::vector<DWORD> vecOrder(nBunchSize);	
	for ( DWORD i(0); i < nBunchSize; ++i )
		vecOrder[i] = i;

	// ���´�;
	std::random_shuffle(vecOrder.begin(), vecOrder.end());

	// ���� �� ������ ������ ������ ����������� ������ �ʱ�ȭ ��Ű�� �����Ƿ�, ������ ���� �� �ִ� ������ �����Ѵ�;
	/*
	const WORD wFixMaximum(pItemStamp == NULL ? 0 : pItemStamp->sSuitOp.wReModelNum);
	if ( emGenerateOp & RandomAddon::GENERATE_GEN == 0 )
		nGEN = (unsigned int)RESETOptVALUE(fixOption, wFixMaximum);
	*/

	// �������, �Լ� ���������� �ش簪�� �������� �ʱ�ȭ��Ų��;
	ITEM::SADDON sAddonResult[SITEMCUSTOM_ADDON::ADDON_SIZE];
	INT nGenCount(0);

	// ������ ���� �̸� ��Ƶд�;
	for ( INT i(0); i < SITEMCUSTOM_ADDON::ADDON_SIZE && m_sADDON[i].emTYPE != EMADD_NONE; ++i )
	{
		if ( fixOption == (BYTE)m_sADDON[i].emTYPE )
			sAddonResult[nGenCount++] = m_sADDON[i];
	}

	// �ɼ� �߻�;
	bool bFirstAddOption = false;
	for ( INT i(0); (i < nBunchSize) && (nGenCount < SITEMCUSTOM_ADDON::ADDON_SIZE); ++i )
	{
		//	���� ���� ���� ����;
		const DWORD dwOrderType(vecOrder[i]);
		const RandomAddon::Bunch& sGenerateBunch = vecGenerateBunch[dwOrderType];
		switch (sGenerateBunch.emAddon)
		{
		case EMADD_NONE:
		case EMADDEX_INC_NONE:
		case EMADDEX_INCR_NONE:
			continue;
		}

		// ������ �ɼ��̶�� ���� �ʿ� ����;
		if ( fixOption == (BYTE)sGenerateBunch.emAddon )
			continue;

		// �߻�Ȯ�� üũ;
		const float fNowRate(sc::Random::getInstance().GetFloatPercent());

		// ù��° �߻��ϴ� ���� ������ �������� �ϳ��� �����Ѵ�;
		if ( fNowRate < sGenerateBunch.fRateChoose || ( false == bFirstAddOption ) )
		{
			const RandomAddon::Value& sGenerateValue = sGenerateBunch.GetValue();
			const float fValue(sGenerateValue.GetValue(ITEM::bAddonValueInteger[sGenerateBunch.emAddon]));
			
			// �� ������ ����;
			ITEM::SADDON sAddon;
			sAddon.emTYPE = sGenerateBunch.emAddon;
			if ( ITEM::bAddonValueInteger[sGenerateBunch.emAddon] == true )
				sAddon.nVALUE = (INT)fValue;
			else
				sAddon.fVALUE = fValue;

			sAddonResult[nGenCount++] = sAddon;

			bFirstAddOption = true;
		}
	}

	// ���õ� ���� �ֵ���� ���� ������ ����;
	{
		if ( emGenerateOp & RandomAddon::GENERATE_ONLYRANDOM )
		{
			ResetAddon();
			for ( INT i(0), nInsert(0); (i < SITEMCUSTOM_ADDON::ADDON_SIZE) && (nInsert < nGenCount); ++i )
			{
				// �����̾��� ���� �ٸ� ������ �����Ѵ�;
				if ( pItem->sSuitOp.sADDON[i].emTYPE == EMADD_RANDOM )
				{
					m_sADDON[i] = sAddonResult[nInsert++];
				}
			}

			for ( INT i (0); i < SITEMCUSTOM_ADDON::ADDON_SIZE; ++i )
			{
				if ( m_sADDON[i].emTYPE == EMADD_RANDOM )
					m_sADDON[i].emTYPE = EMADD_NONE;
			}
		}
		else
		{
			ResetAddon();
			const INT nResultSize(min(SITEMCUSTOM_ADDON::ADDON_SIZE, nGenCount));
			for ( INT i(0); i < nResultSize; ++i )
			{
				// ������ ���̶�� ������ �ʿ� ����;
				//if ( fixOption == (BYTE)m_sADDON[i].emTYPE )
				//	continue;

				m_sADDON[i] = sAddonResult[i];
			}
		}
	}
	return true;
}