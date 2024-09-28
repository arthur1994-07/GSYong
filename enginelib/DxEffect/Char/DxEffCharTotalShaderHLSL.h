static const char* pTotalHLSL = 
{
	// 노말맵, 오버레이, 리플렉션, 텍스쳐업다운
	"bool     m_bUseNormalMap;     \r\n"
	"bool     m_bUseOverlay;     \r\n"
	"bool     m_bUseReflect;     \r\n"
	"bool     m_bUseTexUpDown;     \r\n"

	// 좌표변환 행렬
	"float4x4 m_mWVP;		\r\n"
	// 좌표변환 월드 행렬
	"float4x4 m_mWorld;	\r\n"
	// 좌표변환 뷰 행렬
	"float4x4 m_mView;	\r\n"
	// 좌표변환 프로젝션 행렬
	"float4x4 m_mProj;	\r\n"

	"float4   m_fvAmbient;     \r\n"
	"float4   m_fvSpecular;     \r\n"
	"float4   m_fvDiffuse;     \r\n"
	"float    m_fSpecularPower;     \r\n"

	// 광원방향
	"float4   m_vLightDir;	\r\n"
	// 카메라위치(로컬좌표계)
	"float3	  m_vEyePos;		\r\n"

	// 스펙큘라, 글로시니스, 글로우 텍스쳐
	"bool     m_bUseSpecularTex;     \r\n"
	"bool     m_bUseGlossinessTex;     \r\n"
	"bool     m_bUseGlossNormalAlpha;     \r\n"
	"bool     m_bUseDirectionLight;     \r\n"

	// 텍스쳐 업다운 값
	"float	  m_fTexUpDownBright;     \r\n"
	"float	  m_fAniTexUpDownAlpha;     \r\n"

	// 오버레이 채널별 색상
	"float4   m_fvInputColor1;     \r\n"
	"float4   m_fvInputColor2;     \r\n"

	// 반사광 정도
	"float    m_fSpecularLevel = 1.0f; \r\n"


	// 리플렉션 정도
	"bool   m_bUseReflectAlpha;     \r\n"
	"float  m_fReflectivity;     \r\n"

	"static const int MAX_MATRICES = 60;     \r\n"
	"float4x3    mWorldMatrixArray[MAX_MATRICES] : WORLDMATRIXARRAY;     \r\n"

	// -------------------------------------------------------------
	// 텍스처
	// -------------------------------------------------------------
	// 디컬 텍스처
	"texture m_DiffuseTex;     \r\n"
	"sampler DiffuseSamp = sampler_state\r\n"
	"{\r\n"
	"	Texture   = <m_DiffuseTex>;     \r\n"
	"	MinFilter = LINEAR;     \r\n"
	"	MagFilter = LINEAR;     \r\n"
	"	MipFilter = LINEAR;     \r\n"
	"};     \r\n"

	// -------------------------------------------------------------
	// 스펙큘라 텍스처
	"texture m_SpecularTex;     \r\n"
	"sampler SpecularSamp = sampler_state\r\n"
	"{\r\n"
	"	Texture   = <m_SpecularTex>;     \r\n"
	"	MinFilter = LINEAR;     \r\n"
	"	MagFilter = LINEAR;     \r\n"
	"	MipFilter = LINEAR;     \r\n"
	"};     \r\n"

	// -------------------------------------------------------------
	// 스펙큘라 파워(글로시니스) 텍스처
	"texture m_GlossinessTex;     \r\n"
	"sampler GlossinessSamp = sampler_state\r\n"
	"{\r\n"
	"	Texture   = <m_GlossinessTex>;     \r\n"
	"	MinFilter = LINEAR;     \r\n"
	"	MagFilter = LINEAR;     \r\n"
	"	MipFilter = LINEAR;     \r\n"
	"};     \r\n"

	// -------------------------------------------------------------
	// 법선맵
	"texture m_NormalMap;     \r\n"
	"sampler NormalSamp = sampler_state\r\n"
	"{\r\n"
	"	Texture   = <m_NormalMap>;     \r\n"
	"	MinFilter = LINEAR;     \r\n"
	"	MagFilter = LINEAR;     \r\n"
	"	MipFilter = LINEAR;     \r\n"

	"	AddressU = Wrap;     \r\n"
	"	AddressV = Wrap;     \r\n"
	"};     \r\n"

	// -------------------------------------------------------------
	// 글로우 텍스처
	"texture m_TexUpDownTex;     \r\n"
	"sampler TexUpDownSamp = sampler_state\r\n"
	"{\r\n"
	"	Texture   = <m_TexUpDownTex>;     \r\n"
	"	MinFilter = LINEAR;     \r\n"
	"	MagFilter = LINEAR;     \r\n"
	"	MipFilter = LINEAR;     \r\n"
	"};     \r\n"

	// -------------------------------------------------------------
	// 오버레이 칼라 텍스쳐
	"texture m_OverlayTexture;         \r\n"
	"sampler OverlayTextureSampler = \r\n"
	"sampler_state\r\n"
	"{\r\n"
	"	Texture = <m_OverlayTexture>;     \r\n"
	"	MipFilter = LINEAR;     \r\n"
	"	MinFilter = LINEAR;     \r\n"
	"	MagFilter = LINEAR;     \r\n"
	"};     \r\n"

	// -------------------------------------------------------------
	// 리플렉션 텍스쳐
	"texture m_ReflectTexture; \r\n"
	/*"<		string type = \"CUBE\"; \r\n"
	">;       \r\n"*/
	"sampler ReflectTextureSampler = \r\n"
	"sampler_state\r\n"
	"{\r\n"
	"	Texture = <m_ReflectTexture>;     \r\n"
	"	MipFilter = LINEAR;     \r\n"
	"	MinFilter = LINEAR;     \r\n"
	"	MagFilter = LINEAR;     \r\n"
	"};     \r\n"
	
	// -------------------------------------------------------------
	// 리플렉션 알파 텍스쳐
	"texture m_ReflectAlphaTex;		 \r\n"
	"sampler ReflectAlphaTextureSampler = \r\n"
	"sampler_state\r\n"
	"{\r\n"
	"	Texture = <m_ReflectAlphaTex>;     \r\n"
	"	MipFilter = LINEAR;     \r\n"
	"	MinFilter = LINEAR;     \r\n"
	"	MagFilter = LINEAR;     \r\n"
	"};     \r\n"


	// -------------------------------------------------------------
	// 정점셰이더에서 픽셀셰이더로 넘기는 데이터
	// -------------------------------------------------------------
	"struct VS_OUTPUT\r\n"
	"{\r\n"
	"	float4 Position		  : POSITION;     \r\n"
	"	float4 Diffuse		  : COLOR0;     \r\n"
	"	float4 Specular		  : COLOR1;     \r\n"
	"	float2 Texcoord		  : TEXCOORD0;	// 디컬텍스처 좌표\r\n"
	"	float3 LightDirection : TEXCOORD1;	// 광원벡터\r\n"
	"	float3 ViewDirection  : TEXCOORD2;	// 법선벡터\r\n"
	 "	float3 Reflection	  : TEXCOORD3;     \r\n"
	"};     \r\n"

	// -------------------------------------------------------------
	// 정점셰이더에서 받아오는 데이터
	// -------------------------------------------------------------
	"struct VS_INPUT\r\n"
	"{\r\n"
	"	float4 Position	: POSITION0; // 로컬 좌표\r\n"
	"	float4 BlendWeights    : BLENDWEIGHT;     \r\n"
	"	float4 BlendIndices    : BLENDINDICES;     \r\n"	
	"	float3 Normal	: NORMAL0; // 법선 벡터\r\n"
	"	float3 Tangent	: TANGENT0; // 접선 벡터\r\n"
	"	float2 Texcoord : TEXCOORD0; // 텍스쳐 좌표  \r\n"  
	"	float3 Binormal : BINORMAL0;  \r\n"  
	"};     \r\n"

	//---------------------------------------------------------------
	// 장면 랜더링
	//---------------------------------------------------------------
	"VS_OUTPUT VS( VS_INPUT Input, uniform int NumBones)"
	"	{\r\n"
	"		VS_OUTPUT Output = (VS_OUTPUT)0; // 출력 데이터\r\n"

	"		float3      Pos		 = 0.0f;     \r\n"
	"		float3      Normal	 = 0.0f;     \r\n"
	"		float3      Binormal = 0.0f;     \r\n"

	"		float       LastWeight = 0.0f;     \r\n"

	"		int4 IndexVector		   = D3DCOLORtoUBYTE4(Input.BlendIndices);     \r\n"
	"		float BlendWeightsArray[4] = (float[4])Input.BlendWeights;     \r\n"
	"		int   IndexArray[4]        = (int[4])IndexVector;     \r\n"

	"		for (int iBone = 0; iBone < NumBones-1; iBone++)\r\n"
	"		{\r\n"
	"			LastWeight = LastWeight + BlendWeightsArray[iBone];     \r\n"

	"			Pos		 += mul(Input.Position, mWorldMatrixArray[IndexArray[iBone]]) * BlendWeightsArray[iBone];     \r\n"
	"			Normal	 += mul(Input.Normal,   mWorldMatrixArray[IndexArray[iBone]]) * BlendWeightsArray[iBone];     \r\n"
	"			Binormal += mul(Input.Binormal, mWorldMatrixArray[IndexArray[iBone]]) * BlendWeightsArray[iBone];     \r\n"
	"		}\r\n"
	"		LastWeight = 1.0f - LastWeight; \r\n"

	"		Pos		 += (mul(Input.Position, mWorldMatrixArray[IndexArray[NumBones-1]]) * LastWeight); \r\n"
	"		Normal	 += (mul(Input.Normal,   mWorldMatrixArray[IndexArray[NumBones-1]]) * LastWeight); \r\n"
	"		Binormal += (mul(Input.Binormal, mWorldMatrixArray[IndexArray[NumBones-1]]) * LastWeight); \r\n"
	
	"		Output.Position = mul(float4(Pos.xyz, 1.0f), m_mProj);     \r\n"
	"		Output.Texcoord = Input.Texcoord;     \r\n"	

	"		float3 fvObjectPosition = mul( Input.Position, m_mWorld );     \r\n"

			// 시선벡터
	"		float3 fvViewDirection  = m_vEyePos - fvObjectPosition;	\r\n"
			// 광원벡터
	"		float3 fvLightDirection = m_vLightDir;					\r\n"
			// 광원벡터
	"		if( !m_bUseDirectionLight )\r\n"
	"			fvLightDirection = m_vLightDir - fvObjectPosition;	\r\n"



	"		float3 TotalNormal	 = normalize( Input.Normal );     \r\n"
	"		float3 TotalBinormal = normalize( Input.Binormal );     \r\n"
	"		float3 TotalTangent = cross(TotalNormal,TotalBinormal);     \r\n"

	"		if( Input.Texcoord.x >= 1.0f )\r\n"
	"		{\r\n"
	"			TotalTangent = -TotalTangent;     \r\n"
	"		}\r\n"

	"		float3 fvTangent  = normalize(mul( TotalTangent, m_mWorld ));     \r\n"
	"		float3 fvBinormal = normalize(mul( TotalBinormal, m_mWorld ));     \r\n"
	"		float3 fvNormal	  = normalize(mul( TotalNormal, m_mWorld ));     \r\n"
	"		fvViewDirection  = normalize( fvViewDirection );     \r\n"
	"		fvLightDirection  = normalize( fvLightDirection );     \r\n"

	"		float fSpecPower = 10 - ( m_fSpecularPower * 0.1f ); \r\n"
	"		float fSpecLevel = ( (m_fSpecularLevel*0.1) );     \r\n"

	// 반사광(specular)
	// 반사벡터를 구한다(view 공간)
	"		float3 Reflect	 = normalize(2 * dot(fvNormal, fvLightDirection) * fvNormal - fvLightDirection);     \r\n"
	// view벡터를 구한다(view 공간)
	"		float3 ViewSpace = -normalize(Output.Position);     \r\n"
	"		float  fRDotV    = max(0, dot(Reflect, ViewSpace));     \r\n"
	"		Output.Specular  = m_fvSpecular * pow( fRDotV, fSpecPower ) * fSpecLevel;     \r\n"
	// 주변광(ambient) + 확산광(diffuse)
	
	"		Output.Diffuse = max(0, dot(fvNormal, fvLightDirection));     \r\n"

	"		Output.ViewDirection.y  = dot( fvTangent,  fvViewDirection );     \r\n"
	"		Output.ViewDirection.x  = dot( fvBinormal, fvViewDirection );     \r\n"
	"		Output.ViewDirection.z  = dot( fvNormal,   fvViewDirection );     \r\n"

	"		Output.LightDirection.y  = dot( fvTangent,  fvLightDirection );     \r\n"
	"		Output.LightDirection.x  = dot( fvBinormal, fvLightDirection );     \r\n"
	"		Output.LightDirection.z  = dot( fvNormal,   fvLightDirection );     \r\n"

	"		Output.ViewDirection  = normalize( Output.ViewDirection );     \r\n"
	"		Output.LightDirection = normalize( Output.LightDirection );     \r\n"


	"		float3 reflection = reflect(-fvViewDirection, fvNormal);     \r\n"
	"		Output.Reflection.xyz = reflection; \r\n"

	"		return Output;	\r\n"
	"	}\r\n"

	"float4 CalculateOverlay( float4 fvDiffuseColor, VS_OUTPUT Input )\r\n"
	"{\r\n"
	"	float4 AreaColor = (tex2D(OverlayTextureSampler, Input.Texcoord));    \r\n"
	"	float3 Color1, Color2;       \r\n" 

	"	if( fvDiffuseColor.r > 0.5 ) \r\n"
	"		Color1.r = 1 - ( 1 - ( 2 * (fvDiffuseColor.r-0.5) ) ) * (1-m_fvInputColor1.r);     \r\n"
	"	else\r\n"
	"		Color1.r = (2 * fvDiffuseColor.r) * m_fvInputColor1.r;     \r\n"

	"	if( fvDiffuseColor.g > 0.5 ) \r\n"
	"		Color1.g = 1 - ( 1 - ( 2 * (fvDiffuseColor.g-0.5) ) ) * (1-m_fvInputColor1.g);     \r\n"
	"	else												\r\n"
	"		Color1.g = (2 * fvDiffuseColor.g) * m_fvInputColor1.g;     \r\n"

	"	if( fvDiffuseColor.b > 0.5 ) \r\n"
	"		Color1.b = 1 - ( 1 - ( 2 * (fvDiffuseColor.b-0.5) ) ) * (1-m_fvInputColor1.b);     \r\n"
	"	else														\r\n"
	"		Color1.b = (2 * fvDiffuseColor.b) * m_fvInputColor1.b;		\r\n"


	"	if( fvDiffuseColor.r > 0.5 ) \r\n"
	"		Color2.r = 1 - ( 1 - ( 2 * (fvDiffuseColor.r-0.5) ) ) * (1-m_fvInputColor2.r);     \r\n"
	"	else												\r\n"
	"		Color2.r = (2 * fvDiffuseColor.r) * m_fvInputColor2.r;     \r\n"

	"	if( fvDiffuseColor.g > 0.5 ) \r\n"
	"		Color2.g = 1 - ( 1 - ( 2 * (fvDiffuseColor.g-0.5) ) ) * (1-m_fvInputColor2.g);     \r\n"
	"	else												\r\n"
	"		Color2.g = (2 * fvDiffuseColor.g) * m_fvInputColor2.g;     \r\n"

	"	if( fvDiffuseColor.b > 0.5 ) \r\n"
	"		Color2.b = 1 - ( 1 - ( 2 * (fvDiffuseColor.b-0.5) ) ) * (1-m_fvInputColor2.b);     \r\n"
	"	else														\r\n"
	"		Color2.b = (2 * fvDiffuseColor.b) * m_fvInputColor2.b;		\r\n"

	"	float4 Overlay   = float4( 0.0f, 0.0f, 0.0f, 0.0f );     \r\n"
	"	float4 DestColor = float4( 0.0f, 0.0f, 0.0f, 0.0f );     \r\n"

	"	DestColor.r = fvDiffuseColor.r - ((fvDiffuseColor.r * AreaColor.r)+(fvDiffuseColor.r * AreaColor.b));     \r\n"
	"	DestColor.g = fvDiffuseColor.g - ((fvDiffuseColor.g * AreaColor.r)+(fvDiffuseColor.g * AreaColor.b));     \r\n"
	"	DestColor.b = fvDiffuseColor.b - ((fvDiffuseColor.b * AreaColor.r)+(fvDiffuseColor.b * AreaColor.b));     \r\n"
	"	DestColor.a = 1.0f;     \r\n"

	"	Overlay.r = (Color1.r * AreaColor.r) + (Color2.r * AreaColor.b); \r\n"		
	"	Overlay.g = (Color1.g * AreaColor.r) + (Color2.g * AreaColor.b);     \r\n"
	"	Overlay.b = (Color1.b * AreaColor.r) + (Color2.b * AreaColor.b);     \r\n"
	"	Overlay.a = 1.0f;     \r\n"

	"	float4 fvOverlayColor = Overlay + DestColor;     \r\n"
	"	fvOverlayColor.a = 1.0f;    	\r\n"
	"	return fvOverlayColor;    	\r\n"

	"}\r\n"

	"float4 CalculateAmbient( float4 fvDiffuseColor )\r\n"
	"{\r\n"
	"	return m_fvAmbient * fvDiffuseColor;     \r\n"
	"}\r\n"

	"float4 CalculateDiffuse( float4 fvDiffuseColor, VS_OUTPUT Input )\r\n"
	"{\r\n"
	"	float4 fvTotalDiffuse = float4( 0.0f, 0.0f, 0.0f, 0.0f );     \r\n"

	"	float3 fvNormalMapColor = normalize( ( tex2D( NormalSamp, Input.Texcoord ).xyz * 2.0f ) - 1.0f );     \r\n"
	"	float  fNDotL           = dot( fvNormalMapColor, Input.LightDirection ); \r\n"

	"	if( m_bUseNormalMap )\r\n"
	"	{\r\n"
	"		fvTotalDiffuse   = m_fvDiffuse * fNDotL * fvDiffuseColor; 			\r\n"
	"	}else{ \r\n"
	"		fvTotalDiffuse   = ( m_fvDiffuse  ) * Input.Diffuse * fvDiffuseColor;     \r\n"
	"	}\r\n"
		
	"	return fvTotalDiffuse;     \r\n"
	"}\r\n"

	"float4 CalculateSpecular( float4 fvDiffuseColor, VS_OUTPUT Input )\r\n"
	"{\r\n"
	"	float4 fvTotalSpecular = float4( 0.0f, 0.0f, 0.0f, 0.0f );		\r\n"

	"	float4 fvTexUpDownColor =  float4(0.0,0.0,0.0,0.0);     \r\n"
	"	if( m_bUseTexUpDown )\r\n"
	"	{\r\n"
	"		fvTexUpDownColor  = tex2D( TexUpDownSamp, Input.Texcoord).rgba;     \r\n"
	"		fvTexUpDownColor -= (1.0f - m_fAniTexUpDownAlpha );     \r\n"
	"		fvTexUpDownColor  = max(0.0f, fvTexUpDownColor);     \r\n"
	"		fvTexUpDownColor *= m_fTexUpDownBright;     \r\n"
	"	}\r\n"

	"	if( m_bUseNormalMap )\r\n"
	"	{\r\n"
	"		float3 fvNormalMapColor = normalize( ( tex2D( NormalSamp, Input.Texcoord ).xyz * 2.0f ) - 1.0f );     \r\n"
	"		float  fNormalMapNDotL  = dot( fvNormalMapColor, Input.LightDirection );     \r\n"
	"		float3 fvReflection     = normalize( ( ( 2.0f * fvNormalMapColor ) * ( fNormalMapNDotL ) ) - Input.LightDirection );     \r\n"
	"		float  fNormalMapRDotV  = max( 0.0f, dot( fvReflection, Input.ViewDirection ) );     \r\n"

	"		float fSpecPower = 10 - ( m_fSpecularPower * 0.1f ); \r\n"

	"		float fGlossinessLevel = 1.0f;     \r\n"
	"		if( m_bUseGlossinessTex )\r\n"
	"		{\r\n"
	"			if( m_bUseGlossNormalAlpha )\r\n"
	"				fGlossinessLevel = fSpecPower * ( 1.0f - tex2D(NormalSamp, Input.Texcoord).a );  \r\n"
	"			else\r\n"
	"				fGlossinessLevel = fSpecPower * ( 1.0f - tex2D(GlossinessSamp, Input.Texcoord).rgb );  \r\n"
	"		}else{\r\n"
	"			fGlossinessLevel = fSpecPower;     \r\n"
	"		}\r\n"

	"		float4 fSpecularLevel   = float4(1.0,1.0,1.0,1.0);     \r\n"

	"		if( m_bUseSpecularTex )\r\n"
	"			fSpecularLevel = tex2D(SpecularSamp, Input.Texcoord);     \r\n"

	"		fvTotalSpecular = ( m_fvSpecular * pow( fNormalMapRDotV, fGlossinessLevel ) * fSpecularLevel ) + fvTexUpDownColor;     \r\n"
	"	}else{ \r\n"
	"		fvTotalSpecular   = Input.Specular + fvTexUpDownColor; \r\n"
	"	}\r\n"

	"	return fvTotalSpecular;     \r\n"
	"}\r\n"

	"float4 CalculateReflect( float4 fvTotalColor, VS_OUTPUT Input )\r\n"
	"{\r\n"
	"	float4 fvReflectColor = float4( 0.0f, 0.0f, 0.0f, 0.0f );		\r\n"

	"	float fReflectAlpha = 1.0f;	  \r\n"	
	"	if( m_bUseReflectAlpha )	  \r\n"	
	"	{\r\n"	
	"		fReflectAlpha = tex2D(ReflectAlphaTextureSampler, Input.Texcoord).a;    \r\n"
	"	}\r\n"	
	"	float3 fvReflection = Input.Reflection;\r\n"
	"	if( m_bUseNormalMap )\r\n"
	"	{\r\n"
	"		float3 fvNormalMapColor = normalize( ( tex2D( NormalSamp, Input.Texcoord ).xyz * 2.0f ) - 1.0f );     \r\n"
	"		float  fNormalMapNDotL  = dot( fvNormalMapColor, Input.LightDirection );     \r\n"
	"		fvReflection     = normalize( ( ( 2.0f * fvNormalMapColor ) * ( fNormalMapNDotL ) ) - Input.LightDirection );     \r\n"
	"	}\r\n"
	"	float3 ReflectColor = texCUBE(ReflectTextureSampler, fvReflection) * fReflectAlpha;    \r\n"
	"	fvReflectColor.xyz = lerp( ReflectColor, fvTotalColor, 1-m_fReflectivity );     \r\n"	

	"	return fvReflectColor;     \r\n"
	"}\r\n"

	//-------------------------------------------------------------------------------------
	"	float4 PS_Main(VS_OUTPUT Input) : COLOR0\r\n"
	"	{\r\n"
	"		float4 fvBaseColor	    = tex2D( DiffuseSamp, Input.Texcoord );     \r\n"
	"		float4 fvDiffuseColor   = fvBaseColor;     \r\n"
	"		if( m_bUseOverlay )\r\n"
	"		{\r\n"
	"			fvDiffuseColor = CalculateOverlay(fvBaseColor, Input );     \r\n"
	"		}\r\n"

	"		float4 fvTotalColor = saturate( CalculateAmbient( fvDiffuseColor ) + \r\n"
	"									    CalculateDiffuse( fvDiffuseColor, Input ) + \r\n"
	"										CalculateSpecular( fvDiffuseColor, Input ) );     \r\n"

	"		float4 result = fvTotalColor; \r\n"
	"		if( m_bUseReflect )\r\n"
	"			result = CalculateReflect( fvTotalColor, Input );		  \r\n"

	"		return saturate( result  );     \r\n"

	"	}\r\n"

	"	int CurNumBones = 2;     \r\n"
	"	VertexShader vsArray[4] = { compile vs_3_0 VS(1), \r\n"
	"								compile vs_3_0 VS(2),\r\n"
	"								compile vs_3_0 VS(3),\r\n"
	"								compile vs_3_0 VS(4)\r\n"
	"	};     \r\n"

	// -------------------------------------------------------------
	// 테크닉
	// -------------------------------------------------------------
	"	technique TShader\r\n"
	"	{\r\n"
	"		pass P0\r\n"
	"		{\r\n"
	"			VertexShader = (vsArray[CurNumBones]);     \r\n"
	"			PixelShader  = compile ps_3_0 PS_Main();     \r\n"
	"		}\r\n"
	"	}\r\n"

};