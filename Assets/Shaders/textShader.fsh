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

 // main procedure, the original name was frag
void main()
{
    float edge = 0.3f;
    float smooth = 0.01f;

    vec4 _color = texture2D(_MainTex, TEX0.xy);
    float dist = _color.r;
    float alpha = smoothstep(edge - smooth, edge + smooth, dist);
    gl_FragColor = vec4(diff_color.rgb, alpha);
    return;
} // main end
