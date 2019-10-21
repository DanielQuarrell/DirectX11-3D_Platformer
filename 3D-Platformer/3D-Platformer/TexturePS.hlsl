
static const float3 lightDirection = { 0.2f, 0.2f, 0.2f };
static const float4 ambient = { 0.05, 0.05f, 0.05f, 1.0f };
static const float4 diffuseColor = { 1.0f, 1.0f, 1.0f, 1.0f };

Texture2D objTex;
SamplerState splr;

float4 main(float4 pos : SV_Position, float2 tex : TexCoord, float3 normal: Normal) : SV_TARGET
{
	float4 textureColor = objTex.Sample(splr, tex);

	float3 finalColor;

	finalColor = textureColor * ambient;
	finalColor += saturate(dot(lightDirection, normal) * diffuseColor * textureColor);

	return float4(finalColor, textureColor.a);
}

/*
float4 textureColor;
float lightIntensity;
float4 finalColor;

textureColor = objTex.Sample(splr, tex);

lightIntensity = saturate(dot(normal, lightDirection));

finalColor = saturate(diffuseColor * lightIntensity);

finalColor = finalColor * textureColor;
*/