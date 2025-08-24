// glesv output by Cg compiler
// cgc version 3.1.0013, build date Apr 18 2012
// command line args: -profile glesv
// source file: shader.hlsl
//vendor NVIDIA Corporation
//version 3.1.0.13
//profile glesv
//program vert
//semantic _MainTex
//semantic _LightDir
//semantic transform
//semantic camView 
//var float4x4 transform :  : _transform[0], 4 : -1 : 1
//var float4x4 camView :  : _camView[0], 4 : -1 : 1
//var float4x4 projection :  : _projection[0], 4 : -1 : 1
//var float4 IN.vertex : $vin.POSITION : POSITION : 0 : 1
//var float2 IN.uv : $vin.TEXCOORD0 : TEXCOORD0 : 0 : 1
//var float3 IN.normal : $vin.NORMAL : NORMAL : 0 : 1
//var float2 vert.uv : $vout.TEXCOORD0 : TEXCOORD0 : -1 : 1
//var float4 vert.pos : $vout.POSITION : POSITION : -1 : 1
//var float3 vert.normal : $vout.TEXCOORD1 : TEXCOORD1 : -1 : 1


struct appdata {
    vec4 _vertex;
    vec2 _uv1;
    vec3 _normal1;
};

struct v2f {
    vec2 _uv;
    vec4 _pos;
    vec3 _normal;
};

v2f _ret_0;
float _TMP4;
float _TMP3;
appdata _OUT0013;
vec4 _r0015;
vec4 _r0017;
vec4 _r0019;
vec3 _r0021;
attribute vec4 POSITION;
attribute vec4 NORMAL;
attribute vec4 TEXCOORD0;
varying vec4 TEX0;
varying vec4 TEX1;
vec3 _TMP28;
vec3 _TMP29;
vec3 _TMP30;
uniform vec4 _transform[4];
uniform vec4 _camView[4];
uniform vec4 _projection[4];

 // main procedure, the original name was vert
void main()
{


    _r0015 = POSITION.x*_transform[0];
    _r0015 = _r0015 + POSITION.y*_transform[1];
    _r0015 = _r0015 + POSITION.z*_transform[2];
    _r0015 = _r0015 + POSITION.w*_transform[3];
    _r0017 = _r0015.x*_camView[0];
    _r0017 = _r0017 + _r0015.y*_camView[1];
    _r0017 = _r0017 + _r0015.z*_camView[2];
    _r0017 = _r0017 + _r0015.w*_camView[3];
    _r0019 = _r0017.x*_projection[0];
    _r0019 = _r0019 + _r0017.y*_projection[1];
    _r0019 = _r0019 + _r0017.z*_projection[2];
    _r0019 = _r0019 + _r0017.w*_projection[3];
    _TMP28.x = _transform[0].x;
    _TMP28.y = _transform[0].y;
    _TMP28.z = _transform[0].z;
    _r0021 = NORMAL.x*_TMP28;
    _TMP29.x = _transform[1].x;
    _TMP29.y = _transform[1].y;
    _TMP29.z = _transform[1].z;
    _r0021 = _r0021 + NORMAL.y*_TMP29;
    _TMP30.x = _transform[2].x;
    _TMP30.y = _transform[2].y;
    _TMP30.z = _transform[2].z;
    _r0021 = _r0021 + NORMAL.z*_TMP30;
    _TMP3 = dot(_r0021, _r0021);
    _TMP4 = inversesqrt(_TMP3);
    _OUT0013._normal1 = _TMP4*_r0021;
    _ret_0._uv = TEXCOORD0.xy;
    _ret_0._pos = _r0019;
    _ret_0._normal = _OUT0013._normal1;
    gl_Position = _r0019;
    TEX0.xy = TEXCOORD0.xy;
    TEX1.xyz = _OUT0013._normal1;
    return;
    TEX0.xy = _ret_0._uv;
    TEX1.xyz = _ret_0._normal;
} // main end
