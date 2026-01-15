RaytracingAccelerationStructure sceneBVHAccStruct : register(t0);

StructuredBuffer<float3> vertexPositions : register(t1);
struct Light
{
    float3 position;
    float intesity;
};
StructuredBuffer<Light> sceneLights : register(t2);
RWTexture2D<float4> frameTexture : register (u0);
cbuffer CameraCB : register(b0)
{
    float3 cameraPosition;
    float _pad0;
    float3 cameraForward;
    float _pad1;
    float3 cameraUp;
    float _pad2;
    float3 cameraRight;
    float _pad3;
};
cbuffer lightsCB : register(b1)
{
    uint lightsCount;
};

struct RayPayload
{
    float4 color;
};
struct ShadowPayload
{
    uint hit; // 0 = clear, 1 = blocked
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
    rayDirection = normalize(rayDirection.x * cameraRight + rayDirection.y * cameraUp + rayDirection.z * cameraForward);
    cameraRay.Direction = rayDirection;
    cameraRay.TMin = 0.001f;
    cameraRay.TMax = 10000.f;
    
    RayPayload rayPayload;
    rayPayload.color = float4(0.f, 0.f, 0.f, 1.f);
    
    TraceRay(
        sceneBVHAccStruct,
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
    const float4 pallete[10] =
    {
        float4(1.0, 0.0, 0.0, 1.0),
        float4(0.0, 1.0, 0.0, 1.0),
        float4(0.0, 0.0, 1.0, 1.0),
        float4(1.0, 1.0, 0.0, 1.0),
        float4(1.0, 0.0, 1.0, 1.0),
        float4(1.0, 0.0, 1.0, 1.0),
        float4(1.0, 1.0, 1.0, 1.0),
        float4(0.5, 0.5, 0.5, 1.0),
        float4(1.0, 0.5, 0.0, 1.0),
        float4(0.5, 0.0, 1.0, 1.0)
    };
    
    uint primitiveIndex = PrimitiveIndex();
    float4 baseColor = pallete[primitiveIndex % 10];
    
    uint i0 = primitiveIndex * 3 + 0;
    uint i1 = primitiveIndex * 3 + 1;
    uint i2 = primitiveIndex * 3 + 2;

    // Fetch positions
    float3 v0 = vertexPositions[i0];
    float3 v1 = vertexPositions[i1];
    float3 v2 = vertexPositions[i2];

    // Geometric (flat) normal
    float3 N = normalize(cross(v1 - v0, v2 - v0));

    // Ensure correct orientation
    N = faceforward(N, -WorldRayDirection(), N);
    
    // Hit position
    float3 hitPos =
        WorldRayOrigin() + RayTCurrent() * WorldRayDirection();
    
    float3 lightning = float3(0.0, 0.0, 0.0);
    for (uint i = 0; i < lightsCount; ++i)
    {
        // Simple directional light
        float3 lightPos = sceneLights[i].position;
        float3 lightDir = normalize(lightPos - hitPos);
        float lightDist = length(lightPos - hitPos);

        // Shadow ray
        RayDesc shadowRay;
        shadowRay.Origin = hitPos + N * 0.001f; // bias
        shadowRay.Direction = lightDir;
        shadowRay.TMin = 0.001f;
        shadowRay.TMax = lightDist - 0.001f;

        ShadowPayload shadow;
        shadow.hit = 0;

        TraceRay(
        sceneBVHAccStruct,
        RAY_FLAG_ACCEPT_FIRST_HIT_AND_END_SEARCH |
        RAY_FLAG_SKIP_CLOSEST_HIT_SHADER,
        0xFF,
        1, // shadow ray hit group
        1,
        1,
        shadowRay,
        shadow
        );
    
        // Lighting
        float NdotL = max(0.0, dot(N, -lightDir));
        float shadowFactor = shadow.hit ? 0.0f : 1.0f;
        float attenuation = 1.0 / (lightDist * lightDist);

        lightning += sceneLights[i].intesity * attenuation *NdotL * shadowFactor;
    }

    
    float3 color = float3(baseColor.x, baseColor.y, baseColor.z) * lightning;

    rayPayload.color = float4(color, 1.0);

    
}
[shader("miss")]
void shadowMiss(inout ShadowPayload payload)
{
    payload.hit = 0;
}
[shader("anyhit")]
void anyHitShadow(inout ShadowPayload payload,
                  in BuiltInTriangleIntersectionAttributes att)
{
    payload.hit = 1;
    IgnoreHit(); // stop traversal immediately
}

