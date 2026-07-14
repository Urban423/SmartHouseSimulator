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

    float _diffuse;
    vec4 _color;
	vec3 morning = vec3(1.0, 0.8, 0.6);
    vec3 day     = vec3(1.0, 1.0, 1.0);
    vec3 evening = vec3(1.0, 0.5, 0.3);
    vec3 night   = vec3(0.1, 0.2, 0.5);
	
	float dayCycle = mod(3200 + time, 12220.0) / 12220.0; // [0.0, 1.0]
	vec3 lightColor;

    if (dayCycle < 0.25)
        lightColor = mix(night, morning, dayCycle / 0.25);       // Night → Morning
    else if (dayCycle < 0.5)
        lightColor = mix(morning, day, (dayCycle - 0.25) / 0.25); // Morning → Day
    else if (dayCycle < 0.75)
        lightColor = mix(day, evening, (dayCycle - 0.5) / 0.25);  // Day → Evening
    else
        lightColor = mix(evening, night, (dayCycle - 0.75) / 0.25); // Evening → Night
	
    _TMP0 = dot(TEX1.xyz, vec3( 0.00000000E+000, 0.00000000E+000, 1.00000000E+000));
    _diffuse = max(_TMP0, 0.00000000E+000) + 0.1;
    _color = texture2D(_MainTex, TEX0.xy);
    _ret_0 = _color*_diffuse;
	vec4 albedo = _color * diff_color;
    gl_FragColor = vec4(albedo.xyz, _color.w);
    return;
} // main end
