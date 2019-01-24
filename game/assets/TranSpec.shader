// vs register
float4x4 g_wvp 		: register(c0);
float4x4 g_world 	: register(c4);

// ps register
float3 g_lightDir : register(c0);
float3 g_camPos 	: register(c1);

float3 		g_Ka			   : register(c2);
float3 		g_Kd 				 : register(c3);
float			g_hasDifTex	 : register(c4);
sampler2D g_diffuseTex : register(s0);	
	
float3 		g_Ks				 : register(c5);
float  		g_Ns				 : register(c6);
float			g_hasSpecTex : register(c7);
sampler2D g_specTex 	 : register(s1);

struct app2vert
{
	float3 pos : 			POSITION;
	float3 normal : 	NORMAL;
	float2 texcoord : TEXCOORD0;
};

struct vert2frag
{
	float4 pos : 			POSITION;
	float3 normal :		NORMAL;
	float2 texcoord : TEXCOORD0;
	float3 worldpos : TEXCOORD1;
};

struct fragout
{
	float4 color : COLOR0;
};

//---------------------------------------------------------
vert2frag vs_main( app2vert IN )
{
	vert2frag OUT = (vert2frag)0;
	OUT.pos = mul( g_wvp, float4(IN.pos.xyz, 1.0));
		
	float3 normal = mul( g_world, float4(IN.normal, 0)).xyz;
	normal = normalize(normal);
	OUT.normal = normal;
	
	float3 worldpos = mul( g_world, float4(IN.pos, 1)).xyz;
	OUT.worldpos = worldpos;
	
	OUT.texcoord = IN.texcoord;
	
	return OUT;
	
};

//---------------------------------------------------------
float blinn( in float3 light, in float3 normal, in float3 view, in float gloss )
{
	float spec = 0;
	
	float3 half = -view + light;
	half = normalize( half );
	
	spec = pow( saturate(dot(half, normal)), gloss );
	
	return spec;
	
}

fragout ps_main( vert2frag IN )
{
	fragout OUT;
	
	float3 light = normalize(-g_lightDir);
	
	float3 normal = normalize(IN.normal);
	float ndotl = saturate( dot(light, normal) );
	
	float3 refl = reflect( light, normal );
	refl = normalize(refl);
	
	float3 view = g_camPos - IN.worldpos;
	view = normalize(-view);
	
	float spec = blinn( light, normal, view, g_Ns );
	float fresnel = 1.0 - saturate( dot(-view, normal) );
	spec += pow( fresnel, 2.0 );
	if (g_hasSpecTex != 0)
	{
			float spec_power = tex2D( g_specTex, IN.texcoord.xy );
			spec *= spec_power;
	}
	
	float3 colorrgb;
	if (g_hasDifTex == 0)
		colorrgb = g_Kd;
	else
	  colorrgb = tex2D( g_diffuseTex, IN.texcoord.xy );	
	colorrgb = colorrgb * ndotl;
	colorrgb += g_Ka * 0.25;
	
	// alpha-blending
	float bFrac = 1.0 - saturate( dot( -view, normal ) );
	OUT.color = float4(colorrgb, bFrac);
	
	OUT.color.rgb += g_Ks * spec;
	return OUT;
	
};
