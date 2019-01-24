// vs register
float4x4 g_wvp 		: register(c0);
float4x4 g_world 	: register(c4);

struct app2vert
{
	float3 pos : 			POSITION;
	float3 color:			COLOR;
};

struct vert2frag
{
	float4 pos : 			POSITION;
	float3 color:			COLOR;
	float3 worldpos : TEXCOORD0;
};

struct fragout
{
	float4 color : COLOR0;
};

vert2frag vs_main( app2vert IN )
{
	vert2frag OUT = (vert2frag)0;
	OUT.pos = mul( g_wvp, float4(IN.pos.xyz, 1.0));
	OUT.color = IN.color;

	float3 worldpos = mul( g_world, float4(IN.pos, 1)).xyz;
	OUT.worldpos = worldpos;

	return OUT;
};

fragout ps_main( vert2frag IN )
{
	fragout OUT;

	OUT.color = float4(IN.color, 1.0);
	
	return OUT;
};
