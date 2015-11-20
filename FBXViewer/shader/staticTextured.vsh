float4x4 matWorld;
float4x4 matView;
float4x4 matProj;

struct InputVS
{
	float3 Pos:		POSITION0;
	float2 TC:		TEXCOORD0;
};

struct OutputVS
{
	float4 Pos:		POSITION0;
	float2 TC:		TEXCOORD0;
};

OutputVS main(InputVS In)
{
	OutputVS outVS = (OutputVS)0;
	float4 pos4 = float4(In.Pos,1.0f);
	pos4 = mul(pos4, matWorld);
	pos4 = mul(pos4, matView);
	pos4 = mul(pos4, matProj);
	outVS.Pos = pos4;
	outVS.TC = In.TC;
	return outVS;
}
