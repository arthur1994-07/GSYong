#pragma once

#include "BasicTextBoxExLink.h"

/**
	�������� �ؽ�Ʈ �ڽ��� ũ��(������)�� ����Ǹ�
    ������ ��ũ�� ������ �ؽ�Ʈ �ڽ��� ���� �Ѵ�.
	
 */
class CDynamicBasicTextBoxExLink
    : public CBasicTextBoxExLink
{
public:
	CDynamicBasicTextBoxExLink(EngineDeviceMan* pEngineDevice);

    /**
    	������ ��ũ ��Ʈ�� �߰�(AddStringLink)�� ��ũ�� ������ ���� ��ĥ ���
        �� ���� �������� �ѱ�� ó���� �ϴµ� �� �Լ��� ��ũ�� ��ĥ ��� ��ũ�� �״��
        ���� �Ǹ鼭 ���ڿ��� ��� ó���� ���� �Ѵ�.

    	\param nIndex ���ڸ� �߰��� ������Ʈ(STEXTPART) �ε���
    	\param strText �߰��� ���ڿ�
    	\param vecLinkTextData ��ũ ������ ���� ����ü, ��ũ�� ���� ���ڿ� ��ġ ���� ������ ���� �Ѵ�.
    	\param TextColor ���ڿ� ����
     */
    int     AddStringLink( int nIndex, const CString& strText,  const VEC_LINK_TEXT_DATA& vecLinkTextData, const D3DCOLOR& TextColor );

    /**
    	�ؽ�Ʈ �ڽ� ũ�⸦ �������� �����ϸ鼭
        ���� �������� �ؽ�Ʈ�� ���� �ؽ�Ʈ ũ�⸦ �� ���� �� �ְ� �����μ�
        ��ũ���� �ϸ� �������� �ؽ�Ʈ�� ������ �ٲ�� �ٽ� �ؽ�Ʈ�� �� ���� ���־�� �Ѵ�.

    	\param nCurLine ���� �������� ���� ����
     */
    void    SetCurLine( int nCurLine );

    /**
    	���� ��Ʈ�ѵ��� ���� �Ѵ�.
        �������� �ؽ�Ʈ �ڽ� ũ�Ⱑ ���� �Ǹ� ȣ�� �ؾ� �Ѵ�.
        ���������� �ؽ�Ʈ �ڽ� ũ�⿡ �°� �������� �ؽ�Ʈ�� �� ���� �Ѵ�.

    	\param rcParentOldPos 
    	\param rcParentNewPos 
     */
    void    AlignSubControl ( const UIRECT& rcParentOldPos, const UIRECT& rcParentNewPos );

protected:

    /**
    	���� �ؽ�Ʈ �ڽ��� ���̸� ������� �������� ���� ������ ��� �Ѵ�.        
        
        \return �������� ���� ����
     */
    float   ReCalcVisibleLine();

    /**
        ���� �ؽ�Ʈ �ڽ� â�� ����� �°�
        �������� �ؽ�Ʈ�鸸 �����(©���ִ� �ؽ�Ʈ�� ��� �ٽ� �����.) �Ѵ�.
        �������� �ؽ�Ʈ �ڽ��� ���� �Ǵ� ��� ����� �� �ִ�.

     */
    void    ReCalcTextPart ();
    void    ReCalcTextPart( DEQUETEXTLINE& sDequeLines );

    /**
    	�ؽ�Ʈ ��Ʈ�� �ٽ� ����� �ش�.
        ���� �ؽ�Ʈ �ڽ��� ����� �������� �ٽ� ����� �ܿ� ���� �Ѵ�.

    	\param sDequeLines �ٽ� ����� ���� ��ü
        \param strText �ٽ� ����� ���ڿ�
        \param nStartLine �ٽ� ����� ���� ���� �ε���
        \param nEndLine �ٽ� ����� �� ���� �ε���
    	\return �ٽ� �����ؼ� ���� ���� ��
     */
    size_t  ReCreateTextPart( DEQUETEXTLINE& sDequeLines, CString& strText, size_t nStartLine, size_t nEndLine );    

    /**
    	�ؽ�Ʈ�� �ܾ �ٽ� ����� �ش�.
        �ܼ��� �ܾ ����� ���� �ƴ϶� ���� �������� ����� �߰� �����Ϳ� ����
        ©�� �ܾ ������ �ϹǷ� ���� �ϴ�.

    	\param dequeLine ���� ���ڿ� ���ε�, �̰��� ����� �����͸� ������ �ִ�.
        \param nLine ���� �˻� �ϰ� �ִ� ����
        \param nWord ���� �˻� �ϰ� �ִ� �ܾ�
        \param nChar ���� �˻� �ϰ� �ִ� ����
        \param sTextLine ���� �߰� �ϰ� �ִ� ����
        \param strLine ���� ���� ������ ���ڿ�
     */
    void    ReCreateTextWord( DEQUETEXTLINE& dequeLine, size_t& nLine, size_t& nWord, size_t& nChar, STEXTLINE& sTextLine, CString& strLine );

    /**
    	���� ���ԵǾ� �ִ� �ܾ��� ������ ���� ���� �ܾ�� �����ϴ� ������ �����Ѵ�.
        ���� �÷� ������ �߰� ������(������ ��ũ ����) �� �����ϴ� ������ ���� �Ѵ�.

    	\param sOrg ���� ���� �Ǿ� �ִ� �ܾ�
    	\param sCopy ���� ���� �ܾ�
    	\param sTextLine ���� ���� �ܾ ���� ����
     */
    void    ReSetWord( STEXTWORD& sOrg, STEXTWORD& sCopy, STEXTLINE& sTextLine );

    /**
        �������� �ؽ�Ʈ �ڽ��� ���ϰ� �Ǹ鼭
        ������ Bottom ���� �Ǿ� ���� ��� ����ؼ� �������� ������ ���� ��������
        ������ �ٲ� ��� ������ ���־�� �Ѵ�. �̸� ���� �Ѵ�.

     */
    void    ReCalcRenderPos();
};