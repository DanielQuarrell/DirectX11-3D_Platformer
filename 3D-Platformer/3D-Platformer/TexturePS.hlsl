static const float3 direction = { 0.5f, 0.5f, -1.0f };
static const float3 ambient = { 0.2f, 0.2f, 0.2f };
static const float4 diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };

Texture2D tex;
SamplerState splr;

float4 main(float3 n : Normal, float2 tc : TexCoord) : SV_Target
{
	n = normalize(n);

	float4 texDiffuse = tex.Sample(splr, tc);
	float3 finalColor;

	finalColor = diffuse * ambient;
	finalColor += saturate(dot(direction, n) * texDiffuse);

	return float4(finalColor, 1.0f);
}