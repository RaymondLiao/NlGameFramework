// vs register

// ps register
sampler2D g_tex0 : register(s0);

struct app2vert
{
	float3 pos: 			POSITION;
	float2 texcoord:	TEXCOORD0;
};

struct vert2frag
{
	float4 pos: 			POSITION;
	float2 texcoord:  TEXCOORD0;
};

struct fragout
{
	float4 color : COLOR0;
};

vert2frag vs_main( app2vert IN )
{
	vert2frag OUT = (vert2frag)0;
	OUT.pos = float4(IN.pos.xyz, 1.0);
	OUT.texcoord = IN.texcoord;
	return OUT;
};

fragout ps_main( vert2frag IN )
{
	fragout OUT;
	
	//float2 offset = float2( 0.002, 0.0014 ) * 4.0;
	float4 color = tex2D( g_tex0, IN.texcoord.xy );
	
	OUT.color = color;
	
	return OUT;
};
