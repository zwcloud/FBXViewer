uniform extern float4x4 matWorld;
uniform extern float4x4 matViewProj;
uniform extern float4x4 matWorldInverseTranspose;

//光照
float4 gAmbient;			//环境光照颜色
float4 gDiffuse;			//散射光照颜色
float4 gSpecular;		//镜面光照颜色
float gSpecularPower;	//镜面光照指数

float4 gMaterialAmbient;	//材质（环境光照）
float4 gMaterialDiffuse;	//材质（散射光照）
float4 gMaterialSpecular;	//材质（镜面光照）

float3 gEyePosition;		//Eye的位置

float3 gPointLightPosition;	//点光源的位置
float3 gAttenuation012;		//衰减

float4x4 MatrixPalette[50];	//所有骨骼的世界变换矩阵

struct InputVS
{
	float3 Pos:			POSITION0;
	float3 Normal:		NORMAL0;
	float2 TC:			TEXCOORD0;
	float4 BoneIndices:	BLENDINDICES;
	float3 Weights:		BLENDWEIGHT;
};

struct OutputVS
{
	float4 Pos:		POSITION0;
	float3 Normal:	NORMAL0;
	float2 TC:		TEXCOORD0;
	float4 Color:	COLOR0;
};

OutputVS main(InputVS In)
{
	OutputVS outVS = (OutputVS)0;

	//skinning 计算

	float4 p = {0.0f, 0.0f, 0.0f, 1.0f};
	float3 norm = {0.0f, 0.0f, 0.0f};
	//if(In.BoneIndices[3] == -10){
		p +=    In.Weights.x*mul(float4(In.Pos.xyz,1.0f),MatrixPalette[In.BoneIndices[0]]);
		norm += In.Weights.x*mul(float4(In.Normal,1.0f), MatrixPalette[In.BoneIndices[0]]).xyz;
	
		p +=    In.Weights.y*mul(float4(In.Pos.xyz,1.0f), MatrixPalette[In.BoneIndices[1]]);
		norm += In.Weights.y*mul(float4(In.Normal,1.0f), MatrixPalette[In.BoneIndices[1]]).xyz;

		p +=    In.Weights.z*mul(float4(In.Pos.xyz,1.0f), MatrixPalette[In.BoneIndices[2]]);
		norm += In.Weights.z*mul(float4(In.Normal,1.0f), MatrixPalette[In.BoneIndices[2]]).xyz;

		float finalWeight = 1 - In.Weights.x - In.Weights.y - In.Weights.z;
		p +=    finalWeight*mul(float4(In.Pos.xyz,1.0f), MatrixPalette[In.BoneIndices[3]]);
		norm += finalWeight*mul(float4(In.Normal,1.0f), MatrixPalette[In.BoneIndices[3]]).xyz;
	//}else{p.xyz = In.Pos.xyz; norm = In.Normal;}
	p.w = 1.0f;
	outVS.Pos = p;

	outVS.Pos = mul(outVS.Pos, matWorld);
	float3 PosW = outVS.Pos.xyz;
	outVS.Pos = mul(outVS.Pos, matViewProj);

	outVS.TC = In.TC;

	//计算法线
	norm = normalize(norm);
	float3 normalW = mul(float4(norm, 0.0f), matWorldInverseTranspose).xyz;
	normalW = normalize(normalW);
	outVS.Normal = normalW;
	//入射光方向
	float3 lightDir = normalize(PosW - gPointLightPosition);
	//反射光方向
	float3 r = reflect(lightDir, normalW);
	//当前顶点到眼睛的单位向量
	float3 toEye = normalize(gEyePosition - PosW);
	//当前顶点到光源的距离
	float d = distance(PosW, gPointLightPosition);
	//衰减
	float Attentuation = gAttenuation012.x + gAttenuation012.y*d + gAttenuation012.z*d*d;

	//高光
	float t = pow(max(dot(r, toEye), 0.0f), gSpecularPower);
	float3 specular = t*(gMaterialSpecular*gSpecular).rgb;
	//散射光
	float s = max(dot(lightDir, normalW), 0.0f);
	float3 diffuse = s*(gMaterialDiffuse*gDiffuse).rgb;
	//环境光
	float3 ambient = (gMaterialAmbient*gAmbient).rgb;

	//总光照
	outVS.Color.rgb = ambient + (diffuse + specular)/Attentuation;	//只有散射光和高光受衰减的影响
	outVS.Color.a = gMaterialDiffuse.a;

	return outVS;
}
