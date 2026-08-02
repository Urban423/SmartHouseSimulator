// glesf output by Cg compiler
// cgc version 3.1.0013, build date Apr 18 2012
// command line args: -profile glesf
// source file: shader.hlsl
//vendor NVIDIA Corporation
//version 3.1.0.13
//profile glesf
//program frag
//semantic _MainTex
//semantic _LightDir
//semantic transform
//semantic camView
//semantic projection
//var sampler2D _MainTex :  : _MainTex : -1 : 1
//var float2 IN.uv : $vin.TEXCOORD0 : TEXCOORD0 : 0 : 1
//var float3 IN.normal : $vin.TEXCOORD1 : TEXCOORD1 : 0 : 1
//var float4 frag : $vout.COLOR : COL : -1 : 1



struct appdata {
    vec2 _uv1;
    vec3 _normal1;
};

struct v2f {
    vec2 _uv;
    vec3 _normal;
};

vec4 _ret_0;
float _TMP0;
varying vec4 TEX0;
varying vec4 TEX1;
uniform sampler2D _MainTex;
uniform vec4 diff_color;
uniform float time;

float median(float a, float b, float c)
{
    return min(min(a, b), min(min(a, b), c));
}

 // main procedure, the original name was frag
void main()
{
    vec2 uv = floor(TEX0.xy * textureSize(_MainTex,0)) / textureSize(_MainTex,0);
    float edge = 0.55f;

    vec4 msdf = texture2D(_MainTex, TEX0.xy);
    float dist = msdf.r;
    float md = median(msdf.r, msdf.g, msdf.b);
    float sd = md - edge;
    float alpha = clamp(sd / fwidth(md) + edge, 0.0, 1.0);
    gl_FragColor = vec4(diff_color.rgb, alpha);
    return;
} // main end
