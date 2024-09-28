#pragma once

#define	USERANGE					0x00000001	// ���� üũ
#define	USESPHERE					0x00000002	// �� üũ
#define	USERANGEHIGH				0x00000004	// ���� ȸ�� üũ
#define	USEROTATEL					0x00000008	// ȸ�� üũ
#define	USECOLLISION				0x00000020	// ��ƼŬ �浹 üũ
#define	USEATTRACTION				0x00000040	// �ڷ�	
#define	USECENTER					0x00000080	// ���̱�
#define	USEDIRECTION				0x00000100	// ������ ������ ����..
#define	USETEXSCALE					0x00000200	// �ؽ��� ���̱�
#define	USETEXROTATE				0x00000400	// �ؽ��� ȸ��
#define	USEMESH						0x00000800	// �޽� ��� üũ
#define	USEPARENTROTATEWORLD		0x00001000	// �θ��� ȸ�� ��Ʈ���� ���� ��	-_-; �ٸ� ��� ������?	���� ����Ʈ  ��
#define	USEPARENTMOVE				0x00002000	// �θ��� �̵� ��Ʈ���� ���� ��	-_-; �ٸ� ��� ������?
#define	USEPARENTROTATE_FOR_PARTICLE	0x00008000	// �θ��� �̵� ��Ʈ���� ���� ��	-_-; �ٸ� ��� ������?
#define	USESEQUENCE					0x00010000	// ������ ȿ�� ����
#define	USESEQUENCELOOP				0x00020000	// ������ ȿ�� ���� ����
#define	USEBILLBOARDALL				0x00040000	// ��� ��ƼŬ�� ������ ���� ��Ű��
#define	USEBILLBOARDUP				0x00080000	// ������ ���� �� ���� ����� ����.!!
#define	USETEXTURE					0x00100000	// USEMESH, UESTEXTURE, USEMESH+UESTEXTURE 3���� ��츦 ���� �� �ִ�.
#define	USEBEZIER_P					0x00200000	// �߰� ��ƼŬ..( �ð��� �ʹ� ª�� ��, �̵� �� �� ����Ѵ�. )
#define	USEGROUND					0x00400000	// ���� ȿ��
#define	USEFROMTO					0x00800000	// ��𿡼� ���� ��ǥ�� �̵��Ѵ�.
#define	USEGROUNDATTACH				0x01000000	// ���鿡 ��ƼŬ�� �޶� �ٴ´�.
#define	USEHEIGHTNO					0x02000000	// ���̰��� ��ȭ�� ������ �Ѵ�.
#define USENORMAL2					0x04000000	// ���
#define USEGROUNDTEX				0x08000000	// ���� TEX ���
#define USERANDOMPLAY				0x10000000	// ������ ȿ���� ����
#define USERANDOMDIRECT				0x20000000	// ������ ȿ���� ���� ���� ���� ����
#define USENEWEFF					0x40000000	// ���ο� ȿ���� ����
#define USENEWEFF_END				0x80000000	// ���ο� ȿ���� ����.. �θ�ȿ���� ���� ������Ų��. < ������ ó������ ���� >
#define	USENEWEFF_END_STOP			0x00000010	// ���ο� ȿ���� ����.. �θ�ȿ���� ���� ������Ų��. �θ�� �������� �����.

// Note : ���� ���
#define	USEGOTOCENTER				0x00004000	// ���� �������� �̵� ��Ŵ


enum PARTICLE_FLAG_EX
{
	USE_EX_RANGE_SCREEN	= 0x00000001,		// ȭ�� �տ��� ��ƼŬ���� �����ǵ��� �Ѵ�.
};

#define STRING_NUM_256		256
