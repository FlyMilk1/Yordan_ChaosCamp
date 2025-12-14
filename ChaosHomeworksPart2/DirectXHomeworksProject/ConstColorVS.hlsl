struct VSInput
{
    float2 position : POSITION;
};

struct PSInput
{
    float4 position : SV_POSITION;
};
cbuffer RootConstants : register(b0)
{
    int frameIdx;
    float offsetX;
    float offsetY;
}
PSInput VSMain(VSInput inputVertex)
{
    PSInput output;
    
    float2 pos = inputVertex.position;
    float2 offsetPos;
    offsetPos.x = pos.x + offsetX;
    offsetPos.y = pos.y + offsetY;
    
    output.position = float4(offsetPos, 0.0, 1.0);
    return output;
}
