
matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

vector		g_vLightDir;
vector		g_vLightDiffuse;
vector		g_vLightAmbient;
vector		g_vLightSpecular;

vector		g_vMtrlAmbient = vector(0.3f, 0.3f, 0.3f, 1.f);
vector		g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);;

vector		g_vCamPosition;
float		g_fPower = 30.f;

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
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
};


VS_OUT VS_MAIN(/* Á¤Á¡ */VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;
	Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
	Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);

	return Out;
}

struct PS_IN
{
	float4		vPosition : POSITION;
	float4		vNormal : NORMAL;
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

	vector		vMtrlDiffuse = tex2D(DiffuseSampler, In.vTexUV);

	float		fShade = saturate(dot(normalize(g_vLightDir) * -1.f, In.vNormal));

	vector		vReflect = reflect(normalize(g_vLightDir), normalize(In.vNormal));
	vector		vLook = normalize(In.vWorldPos - g_vCamPosition);

	float		fSpecular = pow(saturate(dot(normalize(vReflect) * -1.f, vLook)), g_fPower);

	Out.vColor = (g_vLightDiffuse * vMtrlDiffuse) * (fShade + (g_vLightAmbient * g_vMtrlAmbient))
		+ (g_vLightSpecular * g_vMtrlSpecular) * fSpecular;

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

