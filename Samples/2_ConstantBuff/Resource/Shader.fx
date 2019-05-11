//--------------------------------------------------------------------------------------
// File: Tutorial02.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

cbuffer cbCamera : register(b0)
{
  float4x4 mViewMat;
  float4x4 mProjMat;
};

cbuffer cbObject : register(b1)
{
  float4x4 mModelMat;
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
  vout.PosH = 
    mul(
      mul(
        mul(float4(vin.Pos.xyz, 1.0f), mModelMat)
        , mViewMat)
      , mProjMat);
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