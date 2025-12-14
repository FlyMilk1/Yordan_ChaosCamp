struct PSInput
{
    float4 position : SV_POSITION;
};

cbuffer RootConstants : register(b0)
{
    int frameIdx;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    float4 red_color = float4(1.0, 0.0, 0.0, 1.0);
    float4 purple_color = float4(0.5, 0.0, 0.5, 1.0);
    
    float4 final_color = float4(0.0, 0.0, 0.0, 1.0);
    if (frameIdx % 100 >= 0 && frameIdx % 100 <= 50)
    {
        final_color = red_color;
    }
    else
    {
        final_color = purple_color;
    }
    return final_color;
}