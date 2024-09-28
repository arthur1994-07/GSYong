#include "stdafx.h"
#include "MiniDumpUtil.h"

extern "C" void * _ReturnAddress(void);
#pragma intrinsic(_ReturnAddress)

#define SNAPPROLOG(Cntx)                                               \
__asm PUSH  EBP                   /* Save EBP explictly.            */ \
__asm MOV   EBP , ESP             /* Move the stack.                */ \
__asm SUB   ESP , __LOCAL_SIZE    /* Space for the local variables. */ \
/* Copy over all the easy current registers values. */                 \
__asm MOV   Cntx.Eax , EAX                                             \
__asm MOV   Cntx.Ebx , EBX                                             \
__asm MOV   Cntx.Ecx , ECX                                             \
__asm MOV   Cntx.Edx , EDX                                             \
__asm MOV   Cntx.Edi , EDI                                             \
__asm MOV   Cntx.Esi , ESI                                             \
/* Zero put the whole EAX register and just copy the segments into  */ \
/* the lower word.  This avoids leaving the upper word uninitialized*/ \
/* as the context segment registers are really 32-bit values.       */ \
__asm XOR   EAX , EAX                                                  \
__asm MOV   AX , GS                                                    \
__asm MOV   Cntx.SegGs , EAX                                           \
__asm MOV   AX , FS                                                    \
__asm MOV   Cntx.SegFs , EAX                                           \
__asm MOV   AX , ES                                                    \
__asm MOV   Cntx.SegEs , EAX                                           \
__asm MOV   AX , DS                                                    \
__asm MOV   Cntx.SegDs , EAX                                           \
__asm MOV   AX , CS                                                    \
__asm MOV   Cntx.SegCs , EAX                                           \
__asm MOV   AX , SS                                                    \
__asm MOV   Cntx.SegSs , EAX                                           \
/* Get the previous EBP value. */                                      \
__asm MOV  EAX , DWORD PTR [EBP]                                       \
__asm MOV  Cntx.Ebp , EAX                                              \
/* Get the previous ESP value. */                                      \
__asm MOV  EAX , EBP                                                   \
/* Two DWORDs up from EBP is the previous stack address. */            \
__asm ADD  EAX , 8                                                     \
__asm MOV  Cntx.Esp , EAX                                              \
/* Save changed registers. */                                          \
__asm PUSH ESI                                                         \
__asm PUSH EDI                                                         \
__asm PUSH EBX                                                         \
__asm PUSH ECX                                                         \
__asm PUSH EDX

// The common epilog for the naked functions,
// SnapCurrentProcessMiniDumpA and SnapCurrentProcessMiniDumpW.
#define SNAPEPILOG(eRetVal)                                            \
__asm POP     EDX             /* Restore saved registers.  */          \
__asm POP     ECX                                                      \
__asm POP     EBX                                                      \
__asm POP     EDI                                                      \
__asm POP     ESI                                                      \
__asm MOV     EAX , eRetVal   /* Set the return value.      */         \
__asm MOV     ESP , EBP       /* Restore the stack pointer. */         \
__asm POP     EBP             /* Restore the frame pointer. */         \
__asm RET                     /* Return to caller.          */

// Just a wrapper to do the context copy.
#define COPYKEYCONTEXTREGISTERS(stFinalCtx,stInitialCtx)               \
stFinalCtx.Eax   = stInitialCtx.Eax   ;                                \
stFinalCtx.Ebx   = stInitialCtx.Ebx   ;                                \
stFinalCtx.Ecx   = stInitialCtx.Ecx   ;                                \
stFinalCtx.Edx   = stInitialCtx.Edx   ;                                \
stFinalCtx.Edi   = stInitialCtx.Edi   ;                                \
stFinalCtx.Esi   = stInitialCtx.Esi   ;                                \
stFinalCtx.SegGs = stInitialCtx.SegGs ;                                \
stFinalCtx.SegFs = stInitialCtx.SegFs ;                                \
stFinalCtx.SegEs = stInitialCtx.SegEs ;                                \
stFinalCtx.SegDs = stInitialCtx.SegDs ;                                \
stFinalCtx.SegCs = stInitialCtx.SegCs ;                                \
stFinalCtx.SegSs = stInitialCtx.SegSs ;                                \
stFinalCtx.Ebp   = stInitialCtx.Ebp

DWORD __declspec (naked) SnapCurrentProcess(LPCONTEXT lpContext)
{
	// Where the registers coming into this function are stored.
	CONTEXT stInitialCtx;
	DWORD eRet;

	// Do the prolog.
	SNAPPROLOG(stInitialCtx);

	// Zero out the final context structure.
	ZeroMemory( lpContext, sizeof(CONTEXT) );

	// Inicate I want everything in the context.
	lpContext->ContextFlags = CONTEXT_FULL |
		CONTEXT_CONTROL |
		CONTEXT_DEBUG_REGISTERS |
		CONTEXT_EXTENDED_REGISTERS |
		CONTEXT_FLOATING_POINT;

	// Get all the groovy context registers and such for this
	// thread.
	if ( GetThreadContext(GetCurrentThread() ,lpContext) )
	{
		COPYKEYCONTEXTREGISTERS( (*lpContext), stInitialCtx ) ;

		// Get the return address and hunt down the call instruction
		// that got us into this function.  All the rest of the
		// registers are set up before the call so I'll ensure the
		// instruction pointer is set that way too.
		UINT_PTR dwRetAddr = (UINT_PTR)_ReturnAddress();
		lpContext->Eip = (DWORD)dwRetAddr;
		eRet = TRUE;            
	}
	else
	{
		eRet = FALSE;
	}

	// Do the epilog.
	SNAPEPILOG(eRet) ;
}
