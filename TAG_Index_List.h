// �ڵ带 ������ �� �ְԲ� �ϴ� �ױ� ����Ʈ

// .h ������ ����� ������ ������ Ȱ���ϱ� ������


// ����)
// ��� #R = Recursive

//////////////////////////////////////////////////////////////////////////
/*  ���� �ӽ� ����Ʈ                                                    */

namespace EngineLib
{
#define �����ǰ�� EngineLib

    enum ���ҽ�_�޴���
    {
	    TAG_UpdateResourceManager,
	    TAG_LoadResourceManager,
	    TAG_ClassTextureManager,
	    TAG_CreateResourceManager,
	    TAG_StartResourceThreadManager,
    }

	enum �ִϸ��̼�_������
	{
		TAG_ProcessAnimationThread,
		TAG_CalculateAnimationThread,
	}
}
	
namespace RanLogic
{
    enum ������_���
    {
	    TAG_InitClientTriggerSystem,
	    TAG_PVEClientStartStage,
    }
}

namespace RanLogicClient
{
    enum ������_���
    {
	    TAG_PVEClientStartStage,
    }
}

namespace MatchingSystem
{

	enum ��ʸ�Ʈ
	{
		TOURNAMENT_UNIQ_DISABLE, // ��ʸ�Ʈ�϶� �����ϰ� �ȵǴ±�ɵ� 
	};
}