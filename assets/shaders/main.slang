struct VertexOutput {
    float4 position : SV_Position;
};

[shader("vertex")]
VertexOutput vertMain(uint vertexID: SV_VertexID) {
    float2 pos[3] = { float2(0.0, 1.0), float2(-1.0, -1.0), float2(1.0, -1.0) };
    VertexOutput output;
    output.position = float4(pos[vertexID], 0.0, 1.0);
    return output;
}

[shader("fragment")]
float4 fragMain() : SV_Target {
    return float4(1.0, 0.0, 0.0, 1.0);
}
