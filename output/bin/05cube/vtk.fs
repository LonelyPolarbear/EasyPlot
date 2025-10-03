#version 150
#ifdef GL_ES
#if __VERSION__ == 300
#define varying in
#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
precision highp sampler2D;
precision highp sampler3D;
#else
precision mediump float;
precision mediump sampler2D;
precision mediump sampler3D;
#endif
#define texelFetchBuffer texelFetch
#define texture1D texture
#define texture2D texture
#define texture3D texture
#endif // 300
#else // GL_ES
#define highp
#define mediump
#define lowp
#if __VERSION__ == 150
#define varying in
#define texelFetchBuffer texelFetch
#define texture1D texture
#define texture2D texture
#define texture3D texture
#endif
#if __VERSION__ == 120
#extension GL_EXT_gpu_shader4 : require
#endif
#endif // GL_ES


uniform int PrimitiveIDOffset;

uniform float ambientIntensity; // the material ambient
uniform float diffuseIntensity; // the material diffuse
uniform float opacityUniform; // the fragment opacity
uniform vec3 ambientColorUniform; // ambient color
uniform vec3 diffuseColorUniform; // diffuse color
uniform int shapeState;
uniform int elementMask;
uniform int filterMask;
uniform vec4 selectColor;
uniform vec4 preselectColor;
uniform vec4 highlightColor;
uniform isamplerBuffer textureType;
uniform isamplerBuffer textureVisible;
uniform isamplerBuffer textureId; 
uniform usamplerBuffer textureState;
uniform samplerBuffer textureC;


// Depth Peeling Support
uniform sampler2D opaqueDepth;


// the output of this shader
out vec4 fragOutput0;
out vec4 fragOutput1;



// handle coincident offsets
uniform float cfactor;
uniform float coffset;

// Value raster
//VTK::ValuePass::Dec

void main()
{
  // VC position of this fragment. This should not branch/return/discard.
  //VTK::PositionVC::Impl

  // Place any calls that require uniform flow (e.g. dFdx) here.
  float cscale = length(vec2(dFdx(gl_FragCoord.z),dFdy(gl_FragCoord.z)));
  //VTK::UniformFlow::Impl


  // Set gl_FragDepth here (gl_FragCoord.z by default)
  gl_FragDepth = gl_FragCoord.z + cfactor*cscale + 0.000016*coffset;


  uint normalState = 0u;
  uint preSelectedState = 1u;
  uint selectedState = 2u;
  uint highlightState = 4u;
 int shapeIdx = gl_PrimitiveID + PrimitiveIDOffset;
 uint state = texelFetchBuffer(textureState, shapeIdx).r;
 state |= uint(shapeState);
 if(bool(elementMask & filterMask) == false) {
   if(bool(state & preSelectedState) || bool(state & selectedState) || bool(state & highlightState)){
   }
   else {
     discard;
   }
 }
 int visible = texelFetchBuffer(textureVisible, shapeIdx).x;
 if(visible == 0) discard;

  // Early depth peeling abort:
  ivec2 pixel = ivec2(gl_FragCoord.xy);
  float oDepth = texelFetch(opaqueDepth, pixel, 0).y;
  if (oDepth != -1. && gl_FragDepth > oDepth)
    { // Ignore fragments that are occluded by opaque geometry:
    fragOutput1.xy = vec2(-1., oDepth);
    return;
    }
  else
    {
    fragOutput1.xy = vec2(-gl_FragDepth, gl_FragDepth);
    return;
    }


  vec3 ambientColor;
  vec3 diffuseColor;
  float opacity = 1.0;
  vec4 texColor = vec4(0,0,0,1); 
  texColor = texelFetchBuffer(textureC, gl_PrimitiveID + PrimitiveIDOffset);
  int type = texelFetchBuffer(textureType, shapeIdx).r;
  if(bool(state & preSelectedState)) texColor = preselectColor;
  else if(bool(state & selectedState)) texColor = selectColor;
  else if(bool(state & highlightState)){
          texColor = highlightColor;
       }
  if (gl_FrontFacing == false) {
    
   }
  ambientColor = ambientIntensity * texColor.rgb;
  diffuseColor = diffuseIntensity * texColor.rgb;
  opacity = opacityUniform * texColor.a;


    fragOutput0 = vec4(ambientColor + diffuseColor, opacity);


  //VTK::TCoord::Impl

  if (fragOutput0.a <= 0.0)
    {
    discard;
    }
}

