#ifndef S_CBIT_H_
#define S_CBIT_H_

#pragma once

#define ROT_RIGHT	1
#define ROT_LEFT	2
#define MAX_ROTATE	6

namespace sc
{
///////////////////////////////////////////////////////////////////////////////
/// \ingroup NetServerLib
///
/// \par requirements
/// win98 or later\n
/// win2k or later\n
/// MFC\n
///
/// \version 1.0
/// first version
///
/// \date 2002.05.30
///
/// \author jgkim
///
/// \par license
/// Copyright 2002-2003 (c) Mincoms. All rights reserved.   
///
/// \todo 
///
/// \bug 
///
	class CBit
	{
	public :
		CBit();
		~CBit();

	protected:		
		/**
		* 스트림에서 해당 포지션의 비트값을 가져온다.
		* \param *bits 비트를 가져올 스트림
		* \param pos 비트를 가져올 위치 
		* \return 가져온 비트값 1 or 0
		*/
		int	 bit_get(const char *bits, int pos);

		/**
		* 스트림에서 해당 포지션의 비트값을 세팅한다.
		* \param *bits 비트를 세팅할 스트림
		* \param pos 비트를 세팅할 위치
		* \param state 세팅할 비트값 1 or 0
		*/
		void bit_set(char *bits, int pos, int state);
		void bit_xor(const char *bits1, 
				const char *bits2, 
				char *bitsx, 
				int size);
		void bit_rot_left(char *bits, int size, int count);
		void bit_rot_right(char *bits, int size, int count);
		void byte_xor(char *bits1, int sizeByte);

	public:
		void buf_encode(char *cbuffer,
			int nSize,
			int nDirection,
			int nAmount);
		void buf_decode(char *cbuffer,
			int nSize,
			int nDirection,
			int nAmount);

	public:
		void _buf_encode(char *cbuffer,
			int nSize,
			int nDirection,
			int nAmount);

		void _buf_decode(char *cbuffer,
			int nSize,
			int nDirection,
			int nAmount);
	};
}

#endif // S_CBIT_H_
//													$f  dF   ,
//				.,,,...                            :$L ;$  ,dP
//			,!!!!!!!',cd$$$$$e,                 q  4$f,$$,z$"
//		,!!!!!!!',c$$$$$$$$$$$$c              `$o`$$kuC3$$ .zf
//		,!!!!!!!',c$$$$$$P**""**?$c              R$beeF?$$$$$"
//	;!!!!!!! c$$$$$$",eed$$F"?t"               "$$$$bd$$$"
//	;!!!!!!!.d$$$$$F j"j$3$bf""?b?e,             '$$$$$$P"
//	!!!!!!:  $$$$$$P J,f ,d$$b?$bde`$c          .$k<?????>'$b
//	!!!!!!!$$$$F".$$".u$$??P}"""^ ?$$c.       d$$$$$dd$$$$$>
//	`!!!!!  ?L e$$ $$$$$P'zee^"$$$$boc"$$,     R$$$$$$$$$$$$
//	'!!!   z$$$$$c,"$P'd$$F'zdbeereee$$$$$eu    "??$$$$PF"
//Mn      !!   d$$$$$$$$ee z$P",d$$$$$$c?$$$$$$$$C  '!!!::::!!!>
//MMM   ,cec, '^$$$$$$$$$c,",e$$C?$$$$$$bc?$$$$$$$k !!!!!!!!!!!!
//MMM'.$$$$$$$, ?$$$$$$$$P$$$$$$$bc?T$$$$$$d$$$$$$$ . -.`!!!!!!!
//nMM d$$$$$$$F  $$P???",e4$$$$$$$$bcc?$$$$$$$$$$$$ /~:.!!!!!!!!!
//n.  "$$$$$$$'   ::,"??e,.-.?$$$$$$$$$$$$$$$$$$$$F.C"m.`!!!!!!!'
//M":!:`"""""   :!!!!!!i:."?o. "? ?$$$??$$$"$$F"$P<$$$b/4.`4!!!
//: 4!!!!h     <!!!!!!!!!!:  .CL.F'.zeElu. : ?eb o$$$$$$o(#c'`
//'`~!!~.ud !!````'!''``zd$$$$$`d$$$CuuJ" !: 4$$$$$$$$$$c"$c
//." !~`z$$$":!!!~`..:i! d$$$$$$$`$$$$FCCJ" !!!: ?$$$$$$$$$b $L
//$$"z$$$$$":!!! :!!!!!'4$$$$$$$$`$$$$$" "" !!!!!:`$$$$$$$$$$ "
//?o$$$$$$F !!!!!!!!!!! 4$$$$$$$$;?$P": JL \.~!!!!i $$$$P?"l.u-
//$$$$$$$$F!!!!!!!!!!!!:'R$$$$$$$E.:! $.$$c3$%:`!!! .l==7Cuec^ <
//$$$$$$$$ !!!!!!!!!!!!!i ?$$$$P"<!!! ?$`$$$$N.     Rk`$$$$$$r\
//$$$$$$$$L`!!!!!!!!!!!!!!! .:::!!!''`   $$$$$$      $c"??"7u+? !
//$$$$$$$$$ !!!!!!!!!!!'` :!!''`         '""        .'?b"l.4d$ !!!:
//$$$$$$$$$b `!!!'''`.. '''`               ...:::!!!!!! $$$.?$b'!!!>
//$$$$$$$$$$$beee@$$$$$$$$        ..:::!!!!!!!!!!!!!!!! 3$$b ?$c`!!!
//$$$$$$$$$$$$$$$$$$$$$$$f .::!!!!!!!!!!!!!!!!!!!!!!!!! d$$$$`$$b !!!
//$$$$$$$$$$$"3$$$$$$$$$":!!!!!!!!!!!!!!!!!!!!!!!!!!!!! $$$$$,$$$k !!!
//$$$$$$$$$P,d$$$$$$$$F !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! 3$$$$$ $$$i`!!! 3>
//$$$$$$F$Fz$$$$$$$$$$ !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! d$$$$$h`$$$,!!! $&
//$$$$$FJFx$$$$$$$$$$ !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!'4$$$$$$$ 9$$$.'.$$$L
//$$$$$ F.$$$$$$$$F.r !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! $$$$$$$F  ?$$$e$$$$F
//$$$$F .$$$$$$$$"d$$bu,,.```''!!!!!!!!!!!!!!!!'''''`,d$$$$$$$F.! ?$$$$$$F
//$$$$F d$$$$$$$$ $$$$$$$$$$$bc,,,,,,,,,,,,,,,,ccd$$$$$$$$$$$$ !!f ?$$$$"
//"$$$e$$$$$$$$&'$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ !! $P"
//`"?$$$$$$$$$e`?$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ !!'
//	`?$$$$$$$$$$e `""**???$$$$$$$$$$$$$$$$$$$$$$$P***""..:!!'
//		"?$$$$$$$$k`!!!!!!;;;;;;::,,,...... ,,,,,;;!!!!!!!!!!
//			""???"" !!!!!!!!!!!!!!!!!!!!!!  !!!!!!!!!!!!!!!!!:
//					!!!!!!!!!!!!!!!!!!!'   !!!!!!!!!!!!!!!!!!
//					!!!!!!!!!!!!!!!!.   `!!!!!!!!!'''!!!!'
//					:i !!!!!!!!!!!!!!!!   .``''`.,uu,,.```.
//					!!!!!!!!!!!!!!!!!~`    d$$$$$$$$$$$$$$$$$>
//					c.`~~~~~!!!!!:..        $$$$$$$$$$$$$$$$$$
//					$$$$$eeeeeeeuuuee$      $$$$$$$$$$$$$$$$$"
//					$$$$$$$$$$$$$$$$$$       """"...........
//					?$$$$$$$$$$$$$$$P"       '!!!!!!!!!!!!!!
//					.."""""""""".:          !!!!!!!!!!!!!
//					:!!!!!!!!!!!!!!          `!!!!!!!!!!!'
//					i!!!!!!!!!!!!!'          !!!!!!!!!!!'::
//					:!!!!!!!!!!!!!           !!!!!!!!!!:!!:.
//					!!!!!!!!!!'!!!           . `!!!!!!!!!!!!`...
//					`!!!!!!!! -'``           !!!!!!!!!!!!!!!!!!!!!::
//					!!!!!!!!!i!!!:           `~~~~~```~~~~~~~~~~~~`
//					!!!!!!!!!!!!!!!.
//						```'!!!!!!!'``::..
//							`!!!!!!!!!!!'
//								`````
// SanTa Comming To Town!
// jgkim