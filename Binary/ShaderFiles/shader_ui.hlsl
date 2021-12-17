
matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float		g_fData = 1.f;
texture		g_DiffuseTexture;

sampler DiffuseSampler = sampler_state
{
	texture = g_DiffuseTexture;

	/* 현재 텍스쳐가 픽셀들에게 샘플링 될때의 상태를 설정한다. */
	mipfilter = point;
	magfilter = point;

	AddressU = clamp;
	AddressV = clamp;


};

struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;

};

struct VS_OUT
{

	float4		vPosition : POSITION;
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

	return Out;
}



struct PS_IN
{
	float4		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
};

struct PS_OUT
{

	float4		vColor : COLOR0;
};



PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out;

	Out.vColor = tex2D(DiffuseSampler, In.vTexUV);

	return Out;
}

technique DefaultTechnique
{
	pass DefaultRendering
	{
		AlphaTestEnable = false;
		AlphablendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}

	pass DefaultRendering
	{
		AlphablendEnable = false;
		AlphaTestEnable = true;
		AlphaRef = 10;
		AlphaFunc = Greater;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}
}

