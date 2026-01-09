RaytracingAccelerationStructure sceneBVHAccStrict : register(t0);
RWTexture2D<float4> frameTexture : register (u0);
cbuffer CameraCB : register(b0)
{
    float3 cameraPosition;
    float3 cameraRotation;
    float _padding;
};


struct RayPayload
{
    float4 color;
};

[shader("raygeneration")]
void rayGen()
{
    float width;
    float height;
    frameTexture.GetDimensions(width, height);
    
    RayDesc cameraRay;
    cameraRay.Origin = float3(cameraPosition);
    
    uint2 pixelRasterCoords = DispatchRaysIndex().xy;
    
    float x = pixelRasterCoords.x;
    float y = pixelRasterCoords.y;
    
    x += 0.5f;
    y += 0.5f;
    
    x /= width;
    y /= height;
    
    x = (2.f * x) - 1.f;
    y = 1.f - (2.f * y);
    
    x *= (width / height);
    
    float3 rayDirection = normalize(float3(x, y, 1.f));
    cameraRay.Direction = rayDirection;
    cameraRay.TMin = 0.001f;
    cameraRay.TMax = 10000.f;
    
    RayPayload rayPayload;
    rayPayload.color = float4(0.f, 0.f, 0.f, 1.f);
    
    TraceRay(
        sceneBVHAccStrict,
        RAY_FLAG_NONE,
        0xFF,
        0,
        1,
        0,
        cameraRay,
        rayPayload);
    
    frameTexture[pixelRasterCoords] = rayPayload.color;
    
}

[shader("miss")]
void miss(inout RayPayload payload)
{
    payload.color = float4(0.0, 1.0, 1.0, 1.0);
}
[shader("closesthit")]
void closestHit(inout RayPayload rayPayload, in BuiltInTriangleIntersectionAttributes att)
{
    rayPayload.color = float4(1.0, 0.0, 0.0, 1.0);
}