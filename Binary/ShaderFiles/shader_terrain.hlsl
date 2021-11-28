
matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

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

struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
};


VS_OUT VS_MAIN(/* 정점 */VS_IN In)
{
	VS_OUT			Out;

	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	return Out;
}



struct PS_IN
{
	float4		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct PS_OUT
{

	float4		vColor : COLOR0;
};



PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out;

	vector	vSourDiffuse = tex2D(DiffuseSourSampler, In.vTexUV * 20.f);
	vector	vDestDiffuse = tex2D(DiffuseDestSampler, In.vTexUV * 20.f);
	vector	vFilter = tex2D(FilterSampler, In.vTexUV);

	Out.vColor = vFilter * vSourDiffuse + (1.f - vFilter) * vDestDiffuse;

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

