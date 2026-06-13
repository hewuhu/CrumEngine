// 2D 精灵着色器 — SSBO 合批 + descriptor indexing 纹理数组

struct SpriteData
{
    float4x4 mvp;
    uint     textureId;
};

[[vk::binding(0, 0)]] StructuredBuffer<SpriteData> spriteData;
[[vk::binding(1, 0)]] Texture2D    uTextures[];
[[vk::binding(1, 0)]] SamplerState uSampler;

struct VertexIn
{
    float2 position : POSITION0;
    float2 uv       : TEXCOORD0;
};

struct VertexOut
{
    float4 position : SV_Position;
    float2 uv       : TEXCOORD0;
    nointerpolation uint texId : TEXCOORD1;
};

[shader("vertex")]
VertexOut VSMain(VertexIn input, uint instanceId : SV_InstanceID)
{
    VertexOut output;
    SpriteData data = spriteData[instanceId];
    output.position = mul(data.mvp, float4(input.position, 0.0, 1.0));
    output.uv       = input.uv;
    output.texId    = data.textureId;
    return output;
}

[shader("fragment")]
float4 PSMain(VertexOut input) : SV_Target0
{
    return uTextures[NonUniformResourceIndex(input.texId)].Sample(uSampler, input.uv);
}
