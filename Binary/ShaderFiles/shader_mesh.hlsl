
matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture		g_DiffuseTexture;

sampler DiffuseSampler = sampler_state 
{  
	texture = g_DiffuseTexture;	
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
	float		fShade : COLOR0;
	float		fSpecular : COLOR1;
	float2		vTexUV : TEXCOORD0;
};


VS_OUT VS_MAIN(/* Á¤Á¡ */VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;	
	
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
	float		fShade : COLOR0;
	float		fSpecular : COLOR1;
	float2		vTexUV : TEXCOORD0;
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
		AlphaTestEnable = true;
		AlphaFunc = Greater;
		AlphaRef = 30;
		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}	
}

