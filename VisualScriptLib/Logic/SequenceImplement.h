#pragma once

#define IMPLEMENT_HELP_SEQUENCE_ALL( TMethod, TArg ) \
    IMPLEMENT_HELP( vs::CSequenceObject, TMethod, TArg ) \
    IMPLEMENT_HELP( vs::CSequenceFrame, TMethod, TArg ) \
    IMPLEMENT_HELP( vs::CSequenceOp, TMethod, TArg ) \
    IMPLEMENT_HELP( vs::CSequence, TMethod, TArg ) \
    IMPLEMENT_HELP( vs::CSequenceEvent, TMethod, TArg ) \
    IMPLEMENT_HELP( vs::CSequenceVariable, TMethod, TArg ) \
    IMPLEMENT_HELP( vs::CSequenceAction, TMethod, TArg ) \
    IMPLEMENT_HELP( vs::CSeqEvent_LevelLoaded, TMethod, TArg ) \
    IMPLEMENT_HELP( vs::CSeqEvent_SequenceActivated, TMethod, TArg ) \
    IMPLEMENT_HELP( vs::CSeqEvent_Touch, TMethod, TArg ) \
    IMPLEMENT_HELP( vs::CSeqEvent_RemoteEvent, TMethod, TArg ) \
    IMPLEMENT_HELP( vs::CSeqVar_Bool, TMethod, TArg ) \
    IMPLEMENT_HELP( vs::CSeqVar_Double, TMethod, TArg ) \
    IMPLEMENT_HELP( vs::CSeqVar_String, TMethod, TArg ) \
    IMPLEMENT_HELP( vs::CSeqVar_Vector3, TMethod, TArg ) \
    IMPLEMENT_HELP( vs::CSeqVar_Object, TMethod, TArg ) \
    IMPLEMENT_HELP( vs::CSeqVar_Player, TMethod, TArg ) \
    IMPLEMENT_HELP( vs::CSeqVar_Named, TMethod, TArg ) \
    IMPLEMENT_HELP( vs::CSeqAct_FinishSequence, TMethod, TArg ) \
    IMPLEMENT_HELP( vs::CSeqAct_ActivateRemoteEvent, TMethod, TArg ) \
    IMPLEMENT_HELP( vs::CSeqAct_SetBool, TMethod, TArg ) \
    IMPLEMENT_HELP( vs::CSeqAct_SetDouble, TMethod, TArg ) \
    IMPLEMENT_HELP( vs::CSeqAct_SetString, TMethod, TArg ) \
    IMPLEMENT_HELP( vs::CSeqAct_SetVector3, TMethod, TArg ) \
    IMPLEMENT_HELP( vs::CSeqAct_SetVector3Components, TMethod, TArg ) \
    IMPLEMENT_HELP( vs::CSeqAct_GetVector3Components, TMethod, TArg ) \
    IMPLEMENT_HELP( vs::CSeqAct_SetObject, TMethod, TArg ) \
    IMPLEMENT_HELP( vs::CSeqAct_AddDouble, TMethod, TArg ) \
    IMPLEMENT_HELP( vs::CSeqAct_SubtractDouble, TMethod, TArg ) \
    IMPLEMENT_HELP( vs::CSeqAct_MultiplyDouble, TMethod, TArg ) \
    IMPLEMENT_HELP( vs::CSeqAct_DivideDouble, TMethod, TArg ) \
    IMPLEMENT_HELP( vs::CSeqAct_CompareBool, TMethod, TArg ) \
    IMPLEMENT_HELP( vs::CSeqAct_CompareDouble, TMethod, TArg ) \
    IMPLEMENT_HELP( vs::CSeqAct_CompareString, TMethod, TArg ) \
    IMPLEMENT_HELP( vs::CSeqAct_CompareVector3, TMethod, TArg ) \
    IMPLEMENT_HELP( vs::CSeqAct_CompareObject, TMethod, TArg ) \
    IMPLEMENT_HELP( vs::CSeqAct_Toggle, TMethod, TArg ) \
    IMPLEMENT_HELP( vs::CSeqAct_Switch, TMethod, TArg ) \
    IMPLEMENT_HELP( vs::CSeqAct_RandomSwitch, TMethod, TArg ) \
    IMPLEMENT_HELP( vs::CSeqAct_PlaySound, TMethod, TArg ) \
    IMPLEMENT_HELP( vs::CSeqAct_PlayEffect, TMethod, TArg ) \
    IMPLEMENT_HELP( vs::CSeqAct_OutputString, TMethod, TArg ) \
    IMPLEMENT_HELP( vs::CSeqAct_SetProperty, TMethod, TArg ) \
    IMPLEMENT_HELP( vs::CSeqAct_GetProperty, TMethod, TArg ) \
    IMPLEMENT_HELP( vs::CSeqAct_Function, TMethod, TArg )

IMPLEMENT_HELP_SEQUENCE_ALL( VERSION, EMPTY )