//--------------------------------------------------------------------------------------
// File: Tutorial02.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

#if 0
cbuffer cbPerObject
{
  float4x4 mModelMatrix;
};

cbuffer cbCamera
{
  float4x4 mViewMatrix;
  float4x4 mProjMatrix;
};
#endif

cbuffer cbScale
{
  float mScale;
};

struct VertexIn
{
  float3 Pos : POSITION;
  float4 Color : COLOR;
};

struct VertexOut
{
  float4 PosH : SV_POSITION;
  float4 Color : COLOR;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VertexOut VS(VertexIn vin)
{
  VertexOut vout;
  vout.PosH   = float4(vin.Pos.xyz * mScale, 1.0f);
  vout.Color  = vin.Color;

  return vout;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VertexOut vout) : SV_TARGET
{
  return vout.Color;
}