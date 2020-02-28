//Directional Light
static const float3 lightDirection = { 0.25f, 0.5f, -0.8f };
static const float4 ambient = { 0.2, 0.2f, 0.2f, 1.0f };
static const float4 diffuseColor = { 1.0f, 1.0f, 1.0f, 1.0f };

cbuffer CBuf
{
	float4 face_colors[3000];
};

float4 main(float4 pos : SV_Position, float3 normal : Normal, uint tid : SV_PrimitiveID) : SV_TARGET
{
	normal = normalize(normal);

	float3 finalColor;

	finalColor = face_colors[tid / 2] * ambient;
	finalColor += saturate(dot(lightDirection, normal) * diffuseColor * face_colors[tid / 2]);

	return float4 (finalColor, face_colors[tid / 2].a);
}