#pragma once

#include "BasicTextBoxExLink.h"

/**
	동적으로 텍스트 박스의 크기(사이즈)가 변경되며
    아이템 링크가 가능한 텍스트 박스를 정의 한다.
	
 */
class CDynamicBasicTextBoxExLink
    : public CBasicTextBoxExLink
{
public:
	CDynamicBasicTextBoxExLink(EngineDeviceMan* pEngineDevice);

    /**
    	기존의 링크 스트링 추가(AddStringLink)는 링크가 라인의 끝에 걸칠 경우
        에 다음 라인으로 넘기는 처리를 하는데 이 함수는 링크가 걸칠 경우 링크는 그대로
        유지 되면서 문자열을 끊어서 처리를 수행 한다.

    	\param nIndex 문자를 추가할 문자파트(STEXTPART) 인덱스
    	\param strText 추가할 문자열
    	\param vecLinkTextData 링크 데이터 정보 구조체, 링크에 대한 문자열 위치 등의 정보가 존재 한다.
    	\param TextColor 문자열 색상
     */
    int     AddStringLink( int nIndex, const CString& strText,  const VEC_LINK_TEXT_DATA& vecLinkTextData, const D3DCOLOR& TextColor );

    /**
    	텍스트 박스 크기를 동적으로 수정하면서
        현재 보여지는 텍스트에 한해 텍스트 크기를 재 생성 해 주게 됨으로서
        스크롤을 하면 보여지는 텍스트의 영역이 바뀌어 다시 텍스트를 재 생성 해주어야 한다.

    	\param nCurLine 현재 보여지는 시작 라인
     */
    void    SetCurLine( int nCurLine );

    /**
    	서브 컨트롤들을 정렬 한다.
        동적으로 텍스트 박스 크기가 변경 되면 호출 해야 한다.
        내부적으로 텍스트 박스 크기에 맞게 보여지는 텍스트를 재 생성 한다.

    	\param rcParentOldPos 
    	\param rcParentNewPos 
     */
    void    AlignSubControl ( const UIRECT& rcParentOldPos, const UIRECT& rcParentNewPos );

protected:

    /**
    	현재 텍스트 박스의 넓이를 기반으로 보여지는 라인 정보를 계산 한다.        
        
        \return 보여지는 공간 높이
     */
    float   ReCalcVisibleLine();

    /**
        현재 텍스트 박스 창의 사이즈에 맞게
        보여지는 텍스트들만 재생산(짤려있는 텍스트를 모아 다시 만든다.) 한다.
        동적으로 텍스트 박스가 변경 되는 경우 사용할 수 있다.

     */
    void    ReCalcTextPart ();
    void    ReCalcTextPart( DEQUETEXTLINE& sDequeLines );

    /**
    	텍스트 파트를 다시 만들어 준다.
        현재 텍스트 박스의 사이즈를 바탕으로 다시 만들어 줌에 주의 한다.

    	\param sDequeLines 다시 계산할 라인 전체
        \param strText 다시 계산할 문자열
        \param nStartLine 다시 계산할 시작 라인 인덱스
        \param nEndLine 다시 계산할 끝 라인 인덱스
    	\return 다시 생선해서 만든 라인 수
     */
    size_t  ReCreateTextPart( DEQUETEXTLINE& sDequeLines, CString& strText, size_t nStartLine, size_t nEndLine );    

    /**
    	텍스트의 단어를 다시 만들어 준다.
        단순히 단어를 만드는 것이 아니라 이전 데이터의 색상과 추가 데이터에 의해
        짤라서 단어를 만들어야 하므로 복잡 하다.

    	\param dequeLine 예전 문자열 라인들, 이곳에 색상과 데이터를 가지고 있다.
        \param nLine 현재 검색 하고 있는 라인
        \param nWord 현재 검색 하고 있는 단어
        \param nChar 현재 검색 하고 있는 문자
        \param sTextLine 새로 추가 하고 있는 라인
        \param strLine 새로 만들 라인의 문자열
     */
    void    ReCreateTextWord( DEQUETEXTLINE& dequeLine, size_t& nLine, size_t& nWord, size_t& nChar, STEXTLINE& sTextLine, CString& strLine );

    /**
    	현재 삽입되어 있는 단어의 정보를 새로 만든 단어로 복사하는 역할을 수행한다.
        현재 컬러 정보와 추가 데이터(아이템 링크 정보) 를 복사하는 역할을 수행 한다.

    	\param sOrg 현재 삽입 되어 있는 단어
    	\param sCopy 새로 만들 단어
    	\param sTextLine 새로 만들 단어를 가질 라인
     */
    void    ReSetWord( STEXTWORD& sOrg, STEXTWORD& sCopy, STEXTLINE& sTextLine );

    /**
        동적으로 텍스트 박스가 변하게 되면서
        정렬이 Bottom 으로 되어 있을 경우 계산해서 보여지는 영역과 실제 보여지는
        영역이 바뀔 경우 재계산을 해주어야 한다. 이를 적용 한다.

     */
    void    ReCalcRenderPos();
};