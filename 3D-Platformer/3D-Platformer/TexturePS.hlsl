//Directional Light
static const float3 lightDirection = { 0.25f, 0.5f, -0.8f };
static const float  lightIntensity = { 0.1f };
static const float4 diffuseColor = { 1.0f, 1.0f, 1.0f, 1.0f };
static const float4 ambient = { 0.2, 0.2f, 0.2f, 1.0f };

Texture2D objTexture;
SamplerState splr;

float4 main(float4 pos : SV_Position, float2 tc : TexCoord, float3 normal: Normal) : SV_TARGET
{
	normal = normalize(normal);

	float4 textureColor = objTexture.Sample(splr, tc);

	float3 finalColor;

	finalColor = textureColor * ambient;
	finalColor += saturate(dot(lightDirection, normal) * diffuseColor * textureColor * lightIntensity);

	return float4(finalColor, textureColor.a);
}