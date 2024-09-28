#include "pch.h"

#include "NSBloomHDR.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


namespace NSBloomHDR
{
	void SetFX( TSTRING& strFX )
	{
		strFX = 

			//------------------------------------------------------------------
			"float g_fBrightPassThreshold = 1.f; \r\n"
			"float4 g_vtcBightPassOffsets[4]; \r\n"			// The sampling offsets used by 'DownSample' and 'BrightPass'
			"float4 g_vtcDownSampleOffsets[16]; \r\n"         // The sampling offsets used by 'DownSample' and 'BrightPass'

			"float g_fHBloomWeights[9]; \r\n"                 // Description of the sampling distribution used by
			"float g_fHBloomOffsets[9]; \r\n"                 // the HorizontalBlur() function

			"float g_fVBloomWeights[9]; \r\n"                 // Description of the sampling distribution used by
			"float g_fVBloomOffsets[9]; \r\n"                 // the VerticalBlur() function

			"texture g_BaseTex; \r\n"
			"sampler BaseTexSampler = sampler_state  \r\n"
			"{ \r\n"
			"	Texture = (g_BaseTex); \r\n"
			"}; \r\n"

			"texture g_BloomTex; \r\n"
			"sampler BloomTexSampler = sampler_state  \r\n"
			"{ \r\n"
			"	Texture = (g_BloomTex); \r\n"
			"}; \r\n"

			//------------------------------------------------------------------
			// BRIGHT PASS AND 2x2 DOWN-SAMPLING PIXEL SHADER
			//
			// performs the 2x2 down sample, and then accepts any pixels
			// that are greater or equal to the configured threshold
			//------------------------------------------------------------------
			"float4 PS_BrightPass( in float2 t : TEXCOORD0 ) : COLOR \r\n"
			"{ \r\n"
			"  float4 average = { 0.0f, 0.0f, 0.0f, 0.0f }; \r\n"

			// load in and combine the 4 samples from the source HDR texture
			"	for( int i = 0; i < 4; i++ ) \r\n"
			"	{ \r\n"
			"		average += tex2D( BaseTexSampler, t + float2( g_vtcBightPassOffsets[i].x, g_vtcBightPassOffsets[i].y ) ); \r\n"
			"	} \r\n"

			"	average *= 0.25f; \r\n"

			//// Determine whether this pixel passes the test...
			//"	float fSaturation = (average.x*0.2125) + (average.y*0.7154) + (average.z*0.0721); \r\n"	// 채도값을 곱해줘서 붉은 옷이라고 너무 밝게만 나오지는 않게 하자.
			//"	average *= fSaturation - g_fBrightPassThreshold; \r\n"
			//"	average = max( average, 0.f ); \r\n"

			"	average = average - g_fBrightPassThreshold; \r\n"
			"	average = max( average, 0.f ); \r\n"

			// Write the colour to the bright-pass render target
			"	return average; \r\n"
			"} \r\n"

			//------------------------------------------------------------------
			// 4x4 DOWN-SAMPLING PIXEL SHADER
			//
			// Samples the input texture 16x according to the provided offsets
			// and then writes the average to the output texture
			//------------------------------------------------------------------
			"float4 PS_DownSample4x4( in float2 t : TEXCOORD0 ) : COLOR \r\n"
			"{ \r\n"

			"	float4 average = { 0.0f, 0.0f, 0.0f, 0.0f }; \r\n"

			"	for( int i = 0; i < 16; i++ ) \r\n"
			"	{ \r\n"
			"		average += tex2D( BaseTexSampler, t + float2(g_vtcDownSampleOffsets[i].x, g_vtcDownSampleOffsets[i].y) ); \r\n"
			"	} \r\n"

			"	average *= ( 1.0f / 16.0f ); \r\n"
			
			// Determine whether this pixel passes the test...
			"	float fSaturation = (average.x*0.2125) + (average.y*0.7154) + (average.z*0.0721); \r\n"
			//"	float fSaturation = (average.x*0.2125*2) + (average.y*0.7154) + (average.z*0.0721*3); \r\n"	// 강도값을 줘서 빨간색과 블루색의 강도가 너무 강하게 나오지 않도록 한다.

			// 0.12이하가 되도록 하자. ( 색이 너무 오버되지 않도록 한다 )
			"	float fSaturationMax = min( fSaturation, 0.12f ); \r\n"
			"	average *= fSaturationMax / fSaturation; \r\n"

			// 채도값이 낮은 색이 너무 강하게 나오지 않도록 한다. (빨간색과 블루색)
			"	average *= min( 1.f, (fSaturation * 3.f) / (average.x + average.y + average.z) ); \r\n"

			//"	float4 vSaturation = min( average, float4(0.1f/0.2125f, 0.1f/0.7154f, 0.1f/0.0721f, 1.f) ); \r\n"
			//"	float fSaturation = (vSaturation.x*0.2125) + (vSaturation.y*0.7154) + (vSaturation.z*0.0721); \r\n"	// 채도값을 곱해줘서 붉은 옷이라고 너무 밝게만 나오지는 않게 하자.
			//"	average *= 0.15f / fSaturation; \r\n"	// 0.15 보다 크지는 않도록 한다. 효과 같은 경우에 넘어갈 수 있다.

			"	return average; \r\n"

			"} \r\n"

			//------------------------------------------------------------------
			// HORIZONTAL BLUR
			//
			// Takes 9 samples from the down-sampled texture (4 either side and
			// one central) biased by the provided weights. Different weight
			// distributions will give more subtle/pronounced blurring.
			//------------------------------------------------------------------
			"float4 PS_HorizontalBlur( in float2 t : TEXCOORD0 ) : COLOR \r\n"
			"{ \r\n"

			"	float4 color = { 0.0f, 0.0f, 0.0f, 0.0f }; \r\n"

			"	for( int i = 0; i < 9; i++ ) \r\n"
			"	{ \r\n"
			"		color += (tex2D( BaseTexSampler, t + float2( g_fHBloomOffsets[i], 0.0f ) ) * g_fHBloomWeights[i] ); \r\n"
			"	} \r\n"

			"	return float4( color.rgb, 1.0f ); \r\n"

			"} \r\n"

			//------------------------------------------------------------------
			// VERTICAL BLUR
			//
			// Takes 9 samples from the down-sampled texture (4 above/below and
			// one central) biased by the provided weights. Different weight
			// distributions will give more subtle/pronounced blurring.
			//------------------------------------------------------------------
			"float4 PS_VerticalBlur( in float2 t : TEXCOORD0 ) : COLOR \r\n"
			"{ \r\n"

			"	float4 color = { 0.0f, 0.0f, 0.0f, 0.0f }; \r\n"

			"	for( int i = 0; i < 9; i++ ) \r\n"
			"	{ \r\n"
			"		color += (tex2D( BaseTexSampler, t + float2( 0.0f, g_fVBloomOffsets[i] ) ) * g_fVBloomWeights[i] ); \r\n"
			"	} \r\n"

			"	return float4( color.rgb, 1.0f ); \r\n"

			"} \r\n"

			//------------------------------------------------------------------
			// Final
			//------------------------------------------------------------------
			"float4 PS_Final( in float2 t : TEXCOORD0 ) : COLOR \r\n"
			"{ \r\n"

			"	float4 color = tex2D( BaseTexSampler, t ) + tex2D( BloomTexSampler, t ); \r\n"

			"	return float4( color.rgb, 1.0f ); \r\n"

			"} \r\n"

			//------------------------------------------------------------------
			// tec0
			//------------------------------------------------------------------
			"technique tec0 \r\n"
			"{ \r\n"
			"    pass BrightPass \r\n"
			"    { \r\n"
			"        PixelShader = compile ps_2_0 PS_BrightPass(); \r\n"
			"    } \r\n"
			"    pass DownSample4x4 \r\n"
			"    { \r\n"
			"        PixelShader = compile ps_2_0 PS_DownSample4x4(); \r\n"
			"    } \r\n"
			"    pass HorizontalBlur \r\n"
			"    { \r\n"
			"        PixelShader = compile ps_2_0 PS_HorizontalBlur(); \r\n"
			"    } \r\n"
			"    pass VerticalBlur \r\n"
			"    { \r\n"
			"        PixelShader = compile ps_2_0 PS_VerticalBlur(); \r\n"
			"    } \r\n"
			"    pass Final \r\n"
			"    { \r\n"
			"        PixelShader = compile ps_2_0 PS_Final(); \r\n"
			"    } \r\n"
			"} \r\n";
	}
}
