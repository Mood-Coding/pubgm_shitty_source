#pragma once
#ifndef _DATATYPE_H_
#define _DATATYPE_H_

struct Vector2f
{
	float x;
	float y;
	Vector2f(float xAxis = 0, float yAxis = 0) {
		x = xAxis;
		y = yAxis;
	}
};

struct Vector3f {
	float x = 0;
	float y = 0;
	float z = 0;
};

struct Vector4f {
	float x = 0;
	float y = 0;
	float z = 0;
	float w = 0;
};

struct D3DMatrix
{
	float _11, _12, _13, _14;
	float _21, _22, _23, _24;
	float _31, _32, _33, _34;
	float _41, _42, _43, _44;
};

struct FTransform
{
	Vector4f Rotation;
	Vector3f Translation;
	unsigned char UnknownData00[0x4];
	Vector3f Scale3D;
	unsigned char UnknownData01[0x4];
};



#endif