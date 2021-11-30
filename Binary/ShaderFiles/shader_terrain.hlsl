
matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

vector		g_vBrushPos;
float		g_fRange = 0.f;

texture		g_DiffuseSourTexture;

sampler DiffuseSourSampler = sampler_state
{
	texture = g_DiffuseSourTexture;

	/* 현재 텍스쳐가 픽셀들에게 샘플링 될때의 상태를 설정한다. */
	mipfilter = linear;
	magfilter = linear;
};

texture		g_DiffuseDestTexture;

sampler DiffuseDestSampler = sampler_state
{
	texture = g_DiffuseDestTexture;

	/* 현재 텍스쳐가 픽셀들에게 샘플링 될때의 상태를 설정한다. */
	mipfilter = linear;
	magfilter = linear;
};

texture		g_FilterTexture;

sampler FilterSampler = sampler_state
{
	texture = g_FilterTexture;

	/* 현재 텍스쳐가 픽셀들에게 샘플링 될때의 상태를 설정한다. */
	mipfilter = linear;
	magfilter = linear;
};

texture		g_BrushTexture;

sampler BrushSampler = sampler_state
{
	texture = g_BrushTexture;

	/* 현재 텍스쳐가 픽셀들에게 샘플링 될때의 상태를 설정한다. */
	mipfilter = linear;
	magfilter = linear;
};

vector		g_vLightDir;
vector		g_vLightDiffuse;
vector		g_vLightAmbient;
vector		g_vLightSpecular;


/* 지형의 머테리얼이다. */
vector		g_vMtrlAmbient = vector(0.3f, 0.3f, 0.3f, 1.f);
vector		g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);;

vector		g_vCamPosition;

struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition : POSITION;
	float		fShade : COLOR0;
	float		fSpecular : COLOR1;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
};


VS_OUT VS_MAIN(/* 정점 */VS_IN In)
{
	VS_OUT			Out;

	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;
	Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);

	vector		vWorldNormal = mul(vector(In.vNormal, 0.f), g_WorldMatrix);

	Out.fShade = saturate(dot(normalize(g_vLightDir) * -1.f, normalize(vWorldNormal)));

	vector		vReflect = reflect(normalize(g_vLightDir), normalize(vWorldNormal));
	vector		vCamLook = Out.vWorldPos - g_vCamPosition;
	Out.fSpecular = pow(saturate(dot(normalize(vReflect) * -1.f, normalize(vCamLook))), 30.f);

	// max(dot(normalize(g_vLightDir) * -1.f, normalize(In.vNormal)), 0.f);

	return Out;
}



struct PS_IN
{
	float4		vPosition : POSITION;
	float		fShade : COLOR0;
	float		fSpecular : COLOR1;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
};

struct PS_OUT
{

	float4		vColor : COLOR0;
};



PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector	vSourDiffuse = tex2D(DiffuseSourSampler, In.vTexUV * 20.f);
	vector	vDestDiffuse = tex2D(DiffuseDestSampler, In.vTexUV * 20.f);
	vector	vFilter = tex2D(FilterSampler, In.vTexUV);

	/* 지형의 디퓨즈머테리얼이다. */
	vector	vDiffuse = vFilter * vSourDiffuse + (1.f - vFilter) * vDestDiffuse;

	if (g_vBrushPos.x - g_fRange < In.vWorldPos.x && In.vWorldPos.x < g_vBrushPos.x + g_fRange &&
		g_vBrushPos.z - g_fRange < In.vWorldPos.z && In.vWorldPos.z < g_vBrushPos.z + g_fRange)
	{
		float2		vTexUV = float2((In.vWorldPos.x - (g_vBrushPos.x - g_fRange)) / (2.f * g_fRange),
			((g_vBrushPos.z + g_fRange) - (In.vWorldPos.z)) / (2.f * g_fRange));

		vector	vBrush = tex2D(BrushSampler, vTexUV);

		Out.vColor += vBrush;
	}

	Out.vColor = (g_vLightDiffuse * vDiffuse) * saturate(In.fShade + (g_vLightAmbient * g_vMtrlAmbient))
		+ (g_vLightSpecular * g_vMtrlSpecular) * In.fSpecular;

	return Out;


}

technique DefaultTechnique
{
	pass DefaultRendering
	{
		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}
}

