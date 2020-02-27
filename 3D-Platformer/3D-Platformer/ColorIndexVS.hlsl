cbuffer CBuf
{
	matrix model;
	matrix modelViewProj;
};

struct VSOut
{
	float4 pos : SV_Position;
	float3 normal: Normal;
};

VSOut main(float3 pos : Position, float3 normal : Normal)
{
	VSOut vso;
	vso.pos = mul(float4(pos, 1.0f), modelViewProj);
	vso.normal = mul(normal, (float3x3)model);
	return vso;
}