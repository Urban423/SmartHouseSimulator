struct appdata {
    float4 vertex : POSITION; // Vertex position
    float3 normal : NORMAL;   // Vertex normal
    float2 uv : TEXCOORD0;    // Vertex texture coordinates
};

struct v2f {
    float4 pos : POSITION;    // Transformed vertex position
    float3 normal : TEXCOORD0; // Transformed normal
    float2 uv : TEXCOORD1;    // Pass through texture coordinates
};

uniform float4x4 _transform; // Object to world matrix
uniform float4x4 _projection;   // World to view matrix
uniform float4x4 _camView;    // View to projection matrix
uniform sampler2D _MainTex;   // Texture sampler

v2f vert(appdata IN) {
    v2f OUT;
    OUT.pos = mul(mul(mul(IN.vertex, _transform), _camView), _projection);
    OUT.normal = normalize(mul(IN.normal, (float3x3)_transform));
    OUT.uv = IN.uv;
  
    return OUT;
}

float4 frag(v2f IN) : COLOR {
    float diffuse = max(dot(IN.normal, vec3(0, 0, 1)), 0.0);
    float4 texColor = tex2D(_MainTex, IN.uv);
    return texColor * diffuse;
}






