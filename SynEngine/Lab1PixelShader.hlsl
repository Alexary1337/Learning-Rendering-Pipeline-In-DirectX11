SamplerState SampleType;

struct PixelInputType
{
	float4 position : SV_POSITION;
	//float clip : SV_ClipDistance0; // clip planes feature
};

float4 main(PixelInputType input) : SV_TARGET
{
	return float4(1.0f, 0.0f, 0.0f, 1.0f);
}