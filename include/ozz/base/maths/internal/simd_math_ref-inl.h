//============================================================================//
//                                                                            //
// ozz-animation, 3d skeletal animation libraries and tools.                  //
// https://code.google.com/p/ozz-animation/                                   //
//                                                                            //
//----------------------------------------------------------------------------//
//                                                                            //
// Copyright (c) 2012-2014 Guillaume Blanc                                    //
//                                                                            //
// This software is provided 'as-is', without any express or implied          //
// warranty. In no event will the authors be held liable for any damages      //
// arising from the use of this software.                                     //
//                                                                            //
// Permission is granted to anyone to use this software for any purpose,      //
// including commercial applications, and to alter it and redistribute it     //
// freely, subject to the following restrictions:                             //
//                                                                            //
// 1. The origin of this software must not be misrepresented; you must not    //
// claim that you wrote the original software. If you use this software       //
// in a product, an acknowledgment in the product documentation would be      //
// appreciated but is not required.                                           //
//                                                                            //
// 2. Altered source versions must be plainly marked as such, and must not be //
// misrepresented as being the original software.                             //
//                                                                            //
// 3. This notice may not be removed or altered from any source               //
// distribution.                                                              //
//                                                                            //
//============================================================================//

#ifndef OZZ_OZZ_BASE_MATHS_INTERNAL_SIMD_MATH_REF_INL_H_
#define OZZ_OZZ_BASE_MATHS_INTERNAL_SIMD_MATH_REF_INL_H_

#include <cstddef>
#include <cmath>
#include <cassert>

#include "ozz/base/maths/math_constant.h"

namespace ozz {
namespace math {

namespace internal {
// Defines union cast helpers that are used internally for binary logical
// operations.
union SimdFI4 {
  SimdFloat4 f;
  SimdInt4 i;
};
union SimdIF4 {
  SimdInt4 i;
  SimdFloat4 f;
};
}  // internal

#define OZZ_RCP_EST(_in, _out) do {\
  const float in = _in;\
  const union {float f; int i;} uf = {in};\
  const union {int i; float f;} ui = {(0x3f800000 * 2) - uf.i};\
  const float fp = ui.f * (2.f - in * ui.f);\
  _out = fp * (2.f - in * fp);\
} while (void(0), 0)

#define OZZ_RSQRT_EST(_in, _out) do {\
  const float in = _in;\
  union {float f; int i;} uf = {in};\
  union {int i; float f;} ui = {0x5f3759df - (uf.i / 2)};\
  const float fp = ui.f * (1.5f - (in * .5f * ui.f * ui.f));\
  _out = fp * (1.5f - (in * .5f * fp * fp));\
} while (void(0), 0)

namespace simd_float4 {

OZZ_INLINE SimdFloat4 zero() {
  const SimdFloat4 ret = {0.f, 0.f, 0.f, 0.f};
  return ret;
}

OZZ_INLINE SimdFloat4 one() {
  const SimdFloat4 ret = {1.f, 1.f, 1.f, 1.f};
  return ret;
}

OZZ_INLINE SimdFloat4 x_axis() {
  const SimdFloat4 ret = {1.f, 0.f, 0.f, 0.f};
  return ret;
}

OZZ_INLINE SimdFloat4 y_axis() {
  const SimdFloat4 ret = {0.f, 1.f, 0.f, 0.f};
  return ret;
}

OZZ_INLINE SimdFloat4 z_axis() {
  const SimdFloat4 ret = {0.f, 0.f, 1.f, 0.f};
  return ret;
}

OZZ_INLINE SimdFloat4 w_axis() {
  const SimdFloat4 ret = {0.f, 0.f, 0.f, 1.f};
  return ret;
}

OZZ_INLINE SimdFloat4 Load(float _x, float _y, float _z, float _w) {
  const SimdFloat4 ret = {_x, _y, _z, _w};
  return ret;
}

OZZ_INLINE SimdFloat4 LoadX(float _x) {
  const SimdFloat4 ret = {_x, 0.f, 0.f, 0.f};
  return ret;
}

OZZ_INLINE SimdFloat4 Load1(float _x) {
  const SimdFloat4 ret = {_x, _x, _x, _x};
  return ret;
}

OZZ_INLINE SimdFloat4 LoadPtr(const float* _f) {
  assert(!(uintptr_t(_f) & 0xf) && "Invalid alignment");
  const SimdFloat4 ret = {_f[0], _f[1], _f[2], _f[3]};
  return ret;
}

OZZ_INLINE SimdFloat4 LoadPtrU(const float* _f) {
    assert(!(uintptr_t(_f) & 0x3) && "Invalid alignment");
    const SimdFloat4 ret = {_f[0], _f[1], _f[2], _f[3]};
    return ret;
}

OZZ_INLINE SimdFloat4 LoadXPtrU(const float* _f) {
  assert(!(uintptr_t(_f) & 0x3) && "Invalid alignment");
  const SimdFloat4 ret = {*_f, 0.f, 0.f, 0.f};
  return ret;
}

OZZ_INLINE SimdFloat4 Load1PtrU(const float* _f) {
  assert(!(uintptr_t(_f) & 0x3) && "Invalid alignment");
  const SimdFloat4 ret = {*_f, *_f, *_f, *_f};
  return ret;
}

OZZ_INLINE SimdFloat4 Load2PtrU(const float* _f) {
  assert(!(uintptr_t(_f) & 0x3) && "Invalid alignment");
  const SimdFloat4 ret = {_f[0], _f[1], 0.f, 0.f};
  return ret;
}

OZZ_INLINE SimdFloat4 Load3PtrU(const float* _f) {
  assert(!(uintptr_t(_f) & 0x3) && "Invalid alignment");
  const SimdFloat4 ret = {_f[0], _f[1], _f[2]};
  return ret;
}
}  // simd_float4

OZZ_INLINE float GetX(_SimdFloat4 _v) {
  return _v.x;
}

OZZ_INLINE float GetY(_SimdFloat4 _v) {
  return _v.y;
}

OZZ_INLINE float GetZ(_SimdFloat4 _v) {
  return _v.z;
}

OZZ_INLINE float GetW(_SimdFloat4 _v) {
  return _v.w;
}

OZZ_INLINE SimdFloat4 SetX(_SimdFloat4 _v, float _f) {
  const SimdFloat4 ret = {_f, _v.y, _v.z, _v.w};
  return ret;
}

OZZ_INLINE SimdFloat4 SetY(_SimdFloat4 _v, float _f) {
  const SimdFloat4 ret = {_v.x, _f, _v.z, _v.w};
  return ret;
}

OZZ_INLINE SimdFloat4 SetZ(_SimdFloat4 _v, float _f) {
  const SimdFloat4 ret = {_v.x, _v.y, _f, _v.w};
  return ret;
}

OZZ_INLINE SimdFloat4 SetW(_SimdFloat4 _v, float _f) {
  const SimdFloat4 ret = {_v.x, _v.y, _v.z, _f};
  return ret;
}

OZZ_INLINE SimdFloat4 SetI(_SimdFloat4 _v, int _ith, float _f) {
  assert(_ith >= 0 && _ith <= 3 && "Invalid index ranges");
  SimdFloat4 ret = _v;
  (&ret.x)[_ith] = _f;
  return ret;
}

OZZ_INLINE void StorePtr(_SimdFloat4 _v, float* _f) {
  assert(!(uintptr_t(_f) & 0xf) && "Invalid alignment");
  _f[0] = _v.x;
  _f[1] = _v.y;
  _f[2] = _v.z;
  _f[3] = _v.w;
}

OZZ_INLINE void Store1Ptr(_SimdFloat4 _v, float* _f) {
  assert(!(uintptr_t(_f) & 0xf) && "Invalid alignment");
  _f[0] = _v.x;
}

OZZ_INLINE void Store2Ptr(_SimdFloat4 _v, float* _f) {
  assert(!(uintptr_t(_f) & 0xf) && "Invalid alignment");
  _f[0] = _v.x;
  _f[1] = _v.y;
}

OZZ_INLINE void Store3Ptr(_SimdFloat4 _v, float* _f) {
  assert(!(uintptr_t(_f) & 0xf) && "Invalid alignment");
  _f[0] = _v.x;
  _f[1] = _v.y;
  _f[2] = _v.z;
}

OZZ_INLINE void StorePtrU(_SimdFloat4 _v, float* _f) {
  assert(!(uintptr_t(_f) & 0x3) && "Invalid alignment");
  _f[0] = _v.x;
  _f[1] = _v.y;
  _f[2] = _v.z;
  _f[3] = _v.w;
}

OZZ_INLINE void Store1PtrU(_SimdFloat4 _v, float* _f) {
  assert(!(uintptr_t(_f) & 0x3) && "Invalid alignment");
  _f[0] = _v.x;
}

OZZ_INLINE void Store2PtrU(_SimdFloat4 _v, float* _f) {
  assert(!(uintptr_t(_f) & 0x3) && "Invalid alignment");
  _f[0] = _v.x;
  _f[1] = _v.y;
}

OZZ_INLINE void Store3PtrU(_SimdFloat4 _v, float* _f) {
  assert(!(uintptr_t(_f) & 0x3) && "Invalid alignment");
  _f[0] = _v.x;
  _f[1] = _v.y;
  _f[2] = _v.z;
}

OZZ_INLINE SimdFloat4 SplatX(_SimdFloat4 _v) {
  const SimdFloat4 ret = {_v.x, _v.x, _v.x, _v.x};
  return ret;
}

OZZ_INLINE SimdFloat4 SplatY(_SimdFloat4 _v) {
  const SimdFloat4 ret = {_v.y, _v.y, _v.y, _v.y};
  return ret;
}

OZZ_INLINE SimdFloat4 SplatZ(_SimdFloat4 _v) {
  const SimdFloat4 ret = {_v.z, _v.z, _v.z, _v.z};
  return ret;
}

OZZ_INLINE SimdFloat4 SplatW(_SimdFloat4 _v) {
  const SimdFloat4 ret = {_v.w, _v.w, _v.w, _v.w};
  return ret;
}

OZZ_INLINE void Transpose4x1(const SimdFloat4 _in[4], SimdFloat4 _out[1]) {
  _out[0].x = _in[0].x;
  _out[0].y = _in[1].x;
  _out[0].z = _in[2].x;
  _out[0].w = _in[3].x;
}

OZZ_INLINE void Transpose1x4(const SimdFloat4 _in[1], SimdFloat4 _out[4]) {
  _out[0].x = _in[0].x;
  _out[0].y =  _out[0].z =  _out[0].w = 0.f;
  _out[1].x = _in[0].y;
  _out[1].y =  _out[1].z =  _out[1].w = 0.f;
  _out[2].x = _in[0].z;
  _out[2].y =  _out[2].z =  _out[2].w = 0.f;
  _out[3].x = _in[0].w;
  _out[3].y =  _out[3].z =  _out[3].w = 0.f;
}

OZZ_INLINE void Transpose4x2(const SimdFloat4 _in[4], SimdFloat4 _out[2]) {
  _out[0].x = _in[0].x;
  _out[0].y = _in[1].x;
  _out[0].z = _in[2].x;
  _out[0].w = _in[3].x;
  _out[1].x = _in[0].y;
  _out[1].y = _in[1].y;
  _out[1].z = _in[2].y;
  _out[1].w = _in[3].y;
}

OZZ_INLINE void Transpose2x4(const SimdFloat4 _in[2], SimdFloat4 _out[4]) {
  _out[0].x = _in[0].x;
  _out[0].y = _in[1].x;
  _out[0].z = _out[0].w = 0.f;
  _out[1].x = _in[0].y;
  _out[1].y = _in[1].y;
  _out[1].z = _out[1].w = 0.f;
  _out[2].x = _in[0].z;
  _out[2].y = _in[1].z;
  _out[2].z = _out[2].w = 0.f;
  _out[3].x = _in[0].w;
  _out[3].y = _in[1].w;
  _out[3].z = _out[3].w = 0.f;
}

OZZ_INLINE void Transpose4x3(const SimdFloat4 _in[4], SimdFloat4 _out[3]) {
  _out[0].x = _in[0].x;
  _out[0].y = _in[1].x;
  _out[0].z = _in[2].x;
  _out[0].w = _in[3].x;
  _out[1].x = _in[0].y;
  _out[1].y = _in[1].y;
  _out[1].z = _in[2].y;
  _out[1].w = _in[3].y;
  _out[2].x = _in[0].z;
  _out[2].y = _in[1].z;
  _out[2].z = _in[2].z;
  _out[2].w = _in[3].z;
}

OZZ_INLINE void Transpose3x4(const SimdFloat4 _in[3], SimdFloat4 _out[4]) {
  _out[0].x = _in[0].x;
  _out[0].y = _in[1].x;
  _out[0].z = _in[2].x;
  _out[0].w = 0.f;
  _out[1].x = _in[0].y;
  _out[1].y = _in[1].y;
  _out[1].z = _in[2].y;
  _out[1].w = 0.f;
  _out[2].x = _in[0].z;
  _out[2].y = _in[1].z;
  _out[2].z = _in[2].z;
  _out[2].w = 0.f;
  _out[3].x = _in[0].w;
  _out[3].y = _in[1].w;
  _out[3].z = _in[2].w;
  _out[3].w = 0.f;
}

OZZ_INLINE void Transpose4x4(const SimdFloat4 _in[4], SimdFloat4 _out[4]) {
  _out[0].x = _in[0].x;
  _out[1].x = _in[0].y;
  _out[2].x = _in[0].z;
  _out[3].x = _in[0].w;
  _out[0].y = _in[1].x;
  _out[1].y = _in[1].y;
  _out[2].y = _in[1].z;
  _out[3].y = _in[1].w;
  _out[0].z = _in[2].x;
  _out[1].z = _in[2].y;
  _out[2].z = _in[2].z;
  _out[3].z = _in[2].w;
  _out[0].w = _in[3].x;
  _out[1].w = _in[3].y;
  _out[2].w = _in[3].z;
  _out[3].w = _in[3].w;
}

OZZ_INLINE void Transpose16x16(const SimdFloat4 _in[16], SimdFloat4 _out[16]) {
  for (int i = 0; i < 4; ++i) {
    const int i4 = i * 4;
    _out[i4 + 0].x = *(&_in[0].x + i);
    _out[i4 + 0].y = *(&_in[1].x + i);
    _out[i4 + 0].z = *(&_in[2].x + i);
    _out[i4 + 0].w = *(&_in[3].x + i);
    _out[i4 + 1].x = *(&_in[4].x + i);
    _out[i4 + 1].y = *(&_in[5].x + i);
    _out[i4 + 1].z = *(&_in[6].x + i);
    _out[i4 + 1].w = *(&_in[7].x + i);
    _out[i4 + 2].x = *(&_in[8].x + i);
    _out[i4 + 2].y = *(&_in[9].x + i);
    _out[i4 + 2].z = *(&_in[10].x + i);
    _out[i4 + 2].w = *(&_in[11].x + i);
    _out[i4 + 3].x = *(&_in[12].x + i);
    _out[i4 + 3].y = *(&_in[13].x + i);
    _out[i4 + 3].z = *(&_in[14].x + i);
    _out[i4 + 3].w = *(&_in[15].x + i);
  }
}

OZZ_INLINE SimdFloat4 MAdd(
  _SimdFloat4 _a, _SimdFloat4 _b, _SimdFloat4 _addend) {
  const SimdFloat4 ret = {_a.x * _b.x + _addend.x,
                          _a.y * _b.y + _addend.y,
                          _a.z * _b.z + _addend.z,
                          _a.w * _b.w + _addend.w};
  return ret;
}

OZZ_INLINE SimdFloat4 DivX(_SimdFloat4 _a, _SimdFloat4 _b) {
  const SimdFloat4 ret = {_a.x / _b.x, _a.y, _a.z, _a.w};
  return ret;
}

OZZ_INLINE SimdFloat4 HAdd2(_SimdFloat4 _v) {
  const SimdFloat4 ret = {_v.x + _v.y, _v.y, _v.z, _v.w};
  return ret;
}

OZZ_INLINE SimdFloat4 HAdd3(_SimdFloat4 _v) {
  const SimdFloat4 ret = {_v.x + _v.y + _v.z, _v.y, _v.z, _v.w};
  return ret;
}

OZZ_INLINE SimdFloat4 HAdd4(_SimdFloat4 _v) {
  const SimdFloat4 ret = {_v.x + _v.y + _v.z + _v.w, _v.y, _v.z, _v.w};
  return ret;
}

OZZ_INLINE SimdFloat4 Dot2(_SimdFloat4 _a, _SimdFloat4 _b) {
  const SimdFloat4 ret = {_a.x * _b.x + _a.y * _b.y, _a.y, _a.z, _a.w};
  return ret;
}

OZZ_INLINE SimdFloat4 Dot3(_SimdFloat4 _a, _SimdFloat4 _b) {
  const SimdFloat4 ret = {_a.x * _b.x + _a.y * _b.y + _a.z * _b.z,
                          _a.y,
                          _a.z,
                          _a.w};
  return ret;
}

OZZ_INLINE SimdFloat4 Dot4(_SimdFloat4 _a, _SimdFloat4 _b) {
  const SimdFloat4 ret = {_a.x * _b.x + _a.y * _b.y + _a.z * _b.z + _a.w * _b.w,
                          _a.y,
                          _a.z,
                          _a.w};
  return ret;
}

OZZ_INLINE SimdFloat4 Cross3(_SimdFloat4 _a, _SimdFloat4 _b) {
  const SimdFloat4 ret = {_a.y * _b.z - _a.z * _b.y,
                          _a.z * _b.x - _a.x * _b.z,
                          _a.x * _b.y - _a.y * _b.x,
                          0.f};
  return ret;
}

OZZ_INLINE SimdFloat4 RcpEst(_SimdFloat4 _v) {
  SimdFloat4 ret;
  OZZ_RCP_EST(_v.x, ret.x);
  OZZ_RCP_EST(_v.y, ret.y);
  OZZ_RCP_EST(_v.z, ret.z);
  OZZ_RCP_EST(_v.w, ret.w);
  return ret;
}

OZZ_INLINE SimdFloat4 RcpEstX(_SimdFloat4 _v) {
  SimdFloat4 ret;
  OZZ_RCP_EST(_v.x, ret.x);
  ret.y = _v.y;
  ret.z = _v.z;
  ret.w = _v.w;
  return ret;
}

OZZ_INLINE SimdFloat4 Sqrt(_SimdFloat4 _v) {
  const SimdFloat4 ret = {
    std::sqrt(_v.x), std::sqrt(_v.y), std::sqrt(_v.z), std::sqrt(_v.w)};
  return ret;
}

OZZ_INLINE SimdFloat4 SqrtX(_SimdFloat4 _v) {
  const SimdFloat4 ret = {std::sqrt(_v.x), _v.y, _v.z, _v.w};
  return ret;
}

OZZ_INLINE SimdFloat4 RSqrtEst(_SimdFloat4 _v) {
  SimdFloat4 ret;
  OZZ_RSQRT_EST(_v.x, ret.x);
  OZZ_RSQRT_EST(_v.y, ret.y);
  OZZ_RSQRT_EST(_v.z, ret.z);
  OZZ_RSQRT_EST(_v.w, ret.w);
  return ret;
}

OZZ_INLINE SimdFloat4 RSqrtEstX(_SimdFloat4 _v) {
  SimdFloat4 ret;
  OZZ_RSQRT_EST(_v.x, ret.x);
  ret.y = _v.y;
  ret.z = _v.z;
  ret.w = _v.w;
  return ret;
}

OZZ_INLINE SimdFloat4 Abs(_SimdFloat4 _v) {
  const SimdFloat4 ret = {
    std::abs(_v.x), std::abs(_v.y), std::abs(_v.z), std::abs(_v.w)};
  return ret;
}

OZZ_INLINE SimdInt4 Sign(_SimdFloat4 _v) {
  internal::SimdFI4 fi = {_v};
  const SimdInt4 ret = {
    fi.i.x & 0x80000000,
    fi.i.y & 0x80000000,
    fi.i.z & 0x80000000,
    fi.i.w & 0x80000000};
  return ret;
}

OZZ_INLINE SimdFloat4 Length2(_SimdFloat4 _v) {
  const float sq_len = _v.x * _v.x + _v.y * _v.y;
  const SimdFloat4 ret = {std::sqrt(sq_len), _v.y, _v.z, _v.w};
  return ret;
}

OZZ_INLINE SimdFloat4 Length3(_SimdFloat4 _v) {
  const float sq_len = _v.x * _v.x + _v.y * _v.y + _v.z * _v.z;
  const SimdFloat4 ret = {std::sqrt(sq_len), _v.y, _v.z, _v.w};
  return ret;
}

OZZ_INLINE SimdFloat4 Length4(_SimdFloat4 _v) {
  const float sq_len = _v.x * _v.x + _v.y * _v.y + _v.z * _v.z + _v.w * _v.w;
  const SimdFloat4 ret = {std::sqrt(sq_len), _v.y, _v.z, _v.w};
  return ret;
}

OZZ_INLINE SimdFloat4 Normalize2(_SimdFloat4 _v) {
  const float sq_len = _v.x * _v.x + _v.y * _v.y;
  assert(sq_len != 0.f && "_v is not normalizable");
  const float inv_len = 1.f / std::sqrt(sq_len);
  const SimdFloat4 ret = {_v.x * inv_len, _v.y * inv_len, _v.z, _v.w};
  return ret;
}

OZZ_INLINE SimdFloat4 Normalize3(_SimdFloat4 _v) {
  const float sq_len = _v.x * _v.x + _v.y * _v.y + _v.z * _v.z;
  assert(sq_len != 0.f && "_v is not normalizable");
  const float inv_len = 1.f / std::sqrt(sq_len);
  const SimdFloat4 ret = {_v.x * inv_len, _v.y * inv_len, _v.z * inv_len, _v.w};
  return ret;
}

OZZ_INLINE SimdFloat4 Normalize4(_SimdFloat4 _v) {
  const float sq_len = _v.x * _v.x + _v.y * _v.y + _v.z * _v.z + _v.w * _v.w;
  assert(sq_len != 0.f && "_v is not normalizable");
  const float inv_len = 1.f / std::sqrt(sq_len);
  const SimdFloat4 ret = {_v.x * inv_len,
                          _v.y * inv_len,
                          _v.z * inv_len,
                          _v.w * inv_len};
  return ret;
}

OZZ_INLINE SimdFloat4 NormalizeEst2(_SimdFloat4 _v) {
  const float sq_len = _v.x * _v.x + _v.y * _v.y;
  assert(sq_len != 0.f && "_v is not normalizable");
  float inv_len;
  OZZ_RSQRT_EST(sq_len, inv_len);
  const SimdFloat4 ret = {_v.x * inv_len, _v.y * inv_len, _v.z, _v.w};
  return ret;
}

OZZ_INLINE SimdFloat4 NormalizeEst3(_SimdFloat4 _v) {
  const float sq_len = _v.x * _v.x + _v.y * _v.y + _v.z * _v.z;
  assert(sq_len != 0.f && "_v is not normalizable");
  float inv_len;
  OZZ_RSQRT_EST(sq_len, inv_len);
  const SimdFloat4 ret = {_v.x * inv_len, _v.y * inv_len, _v.z * inv_len, _v.w};
  return ret;
}

OZZ_INLINE SimdFloat4 NormalizeEst4(_SimdFloat4 _v) {
  const float sq_len = _v.x * _v.x + _v.y * _v.y + _v.z * _v.z + _v.w * _v.w;
  assert(sq_len != 0.f && "_v is not normalizable");
  float inv_len;
  OZZ_RSQRT_EST(sq_len, inv_len);
  const SimdFloat4 ret = {_v.x * inv_len,
                          _v.y * inv_len,
                          _v.z * inv_len,
                          _v.w * inv_len};
  return ret;
}

OZZ_INLINE SimdInt4 IsNormalized2(_SimdFloat4 _v) {
  const float sq_len = _v.x * _v.x + _v.y * _v.y;
  const bool normalized = std::abs(sq_len - 1.f) < kNormalizationTolerance;
  const SimdInt4 ret = {-static_cast<int>(normalized), 0, 0, 0};
  return ret;
}

OZZ_INLINE SimdInt4 IsNormalized3(_SimdFloat4 _v) {
  const float sq_len = _v.x * _v.x + _v.y * _v.y + _v.z * _v.z;
  const bool normalized = std::abs(sq_len - 1.f) < kNormalizationTolerance;
  const SimdInt4 ret = {-static_cast<int>(normalized), 0, 0, 0};
  return ret;
}

OZZ_INLINE SimdInt4 IsNormalized4(_SimdFloat4 _v) {
  const float sq_len = _v.x * _v.x + _v.y * _v.y + _v.z * _v.z + _v.w * _v.w;
  const bool normalized = std::abs(sq_len - 1.f) < kNormalizationTolerance;
  const SimdInt4 ret = {-static_cast<int>(normalized), 0, 0, 0};
  return ret;
}

OZZ_INLINE SimdInt4 IsNormalizedEst2(_SimdFloat4 _v) {
  const float sq_len = _v.x * _v.x + _v.y * _v.y;
  const bool normalized =
    std::abs(sq_len - 1.f) < kNormalizationToleranceEst;
  const SimdInt4 ret = {-static_cast<int>(normalized), 0, 0, 0};
  return ret;
}

OZZ_INLINE SimdInt4 IsNormalizedEst3(_SimdFloat4 _v) {
  const float sq_len = _v.x * _v.x + _v.y * _v.y + _v.z * _v.z;
  const bool normalized =
    std::abs(sq_len - 1.f) < kNormalizationToleranceEst;
  const SimdInt4 ret = {-static_cast<int>(normalized), 0, 0, 0};
  return ret;
}

OZZ_INLINE SimdInt4 IsNormalizedEst4(_SimdFloat4 _v) {
  const float sq_len = _v.x * _v.x + _v.y * _v.y + _v.z * _v.z + _v.w * _v.w;
  const bool normalized =
    std::abs(sq_len - 1.f) < kNormalizationToleranceEst;
  const SimdInt4 ret = {-static_cast<int>(normalized), 0, 0, 0};
  return ret;
}

OZZ_INLINE SimdFloat4 NormalizeSafe2(_SimdFloat4 _v, _SimdFloat4 _safe) {
  const float sq_len = _v.x * _v.x + _v.y * _v.y;
  if (sq_len == 0.f) {
    const SimdFloat4 ret = {_safe.x, _safe.y, _v.z, _v.w};
    return ret;
  }
  const float inv_len = 1.f / std::sqrt(sq_len);
  const SimdFloat4 ret = {_v.x * inv_len, _v.y * inv_len, _v.z, _v.w};
  return ret;
}

OZZ_INLINE SimdFloat4 NormalizeSafe3(_SimdFloat4 _v, _SimdFloat4 _safe) {
  const float sq_len = _v.x * _v.x + _v.y * _v.y + _v.z * _v.z;
  if (sq_len == 0.f) {
    const SimdFloat4 ret = {_safe.x, _safe.y, _safe.z, _v.w};
    return ret;
  }
  const float inv_len = 1.f / std::sqrt(sq_len);
  const SimdFloat4 ret = {_v.x * inv_len, _v.y * inv_len, _v.z * inv_len, _v.w};
  return ret;
}

OZZ_INLINE SimdFloat4 NormalizeSafe4(_SimdFloat4 _v, _SimdFloat4 _safe) {
  const float sq_len = _v.x * _v.x + _v.y * _v.y + _v.z * _v.z + _v.w * _v.w;
  if (sq_len == 0.f) {
    return _safe;
  }
  const float inv_len = 1.f / std::sqrt(sq_len);
  const SimdFloat4 ret = {_v.x * inv_len,
                          _v.y * inv_len,
                          _v.z * inv_len,
                          _v.w * inv_len};
  return ret;
}

OZZ_INLINE SimdFloat4 NormalizeSafeEst2(_SimdFloat4 _v, _SimdFloat4 _safe) {
  const float sq_len = _v.x * _v.x + _v.y * _v.y;
  if (sq_len == 0.f) {
    const SimdFloat4 ret = {_safe.x, _safe.y, _v.z, _v.w};
    return ret;
  }
  float inv_len;
  OZZ_RSQRT_EST(sq_len, inv_len);
  const SimdFloat4 ret = {_v.x * inv_len, _v.y * inv_len, _v.z, _v.w};
  return ret;
}

OZZ_INLINE SimdFloat4 NormalizeSafeEst3(_SimdFloat4 _v, _SimdFloat4 _safe) {
  const float sq_len = _v.x * _v.x + _v.y * _v.y + _v.z * _v.z;
  if (sq_len == 0.f) {
    const SimdFloat4 ret = {_safe.x, _safe.y, _safe.z, _v.w};
    return ret;
  }
  float inv_len;
  OZZ_RSQRT_EST(sq_len, inv_len);
  const SimdFloat4 ret = {_v.x * inv_len, _v.y * inv_len, _v.z * inv_len, _v.w};
  return ret;
}

OZZ_INLINE SimdFloat4 NormalizeSafeEst4(_SimdFloat4 _v, _SimdFloat4 _safe) {
  const float sq_len = _v.x * _v.x + _v.y * _v.y + _v.z * _v.z + _v.w * _v.w;
  if (sq_len == 0.f) {
    return _safe;
  }
  float inv_len;
  OZZ_RSQRT_EST(sq_len, inv_len);
  const SimdFloat4 ret = {_v.x * inv_len,
                          _v.y * inv_len,
                          _v.z * inv_len,
                          _v.w * inv_len};
  return ret;
}

OZZ_INLINE SimdFloat4 Lerp(_SimdFloat4 _a,
                           _SimdFloat4 _b,
                           _SimdFloat4 _alpha) {
  const SimdFloat4 ret = {(_b.x - _a.x) * _alpha.x + _a.x,
                          (_b.y - _a.y) * _alpha.y + _a.y,
                          (_b.z - _a.z) * _alpha.z + _a.z,
                          (_b.w - _a.w) * _alpha.w + _a.w};
  return ret;
}

OZZ_INLINE SimdFloat4 Min(_SimdFloat4 _a, _SimdFloat4 _b) {
  const SimdFloat4 ret = {_a.x < _b.x ? _a.x:_b.x,
                          _a.y < _b.y ? _a.y:_b.y,
                          _a.z < _b.z ? _a.z:_b.z,
                          _a.w < _b.w ? _a.w:_b.w};
  return ret;
}

OZZ_INLINE SimdFloat4 Max(_SimdFloat4 _a, _SimdFloat4 _b) {
  const SimdFloat4 ret = {_a.x > _b.x ? _a.x:_b.x,
                          _a.y > _b.y ? _a.y:_b.y,
                          _a.z > _b.z ? _a.z:_b.z,
                          _a.w > _b.w ? _a.w:_b.w};
  return ret;
}

OZZ_INLINE SimdFloat4 Min0(_SimdFloat4 _v) {
  const SimdFloat4 ret = {_v.x < 0.f ? _v.x : 0.f,
                          _v.y < 0.f ? _v.y : 0.f,
                          _v.z < 0.f ? _v.z : 0.f,
                          _v.w < 0.f ? _v.w : 0.f};
  return ret;
}

OZZ_INLINE SimdFloat4 Max0(_SimdFloat4 _v) {
  const SimdFloat4 ret = {_v.x > 0.f ? _v.x : 0.f,
                          _v.y > 0.f ? _v.y : 0.f,
                          _v.z > 0.f ? _v.z : 0.f,
                          _v.w > 0.f ? _v.w : 0.f};
  return ret;
}

OZZ_INLINE SimdFloat4 Clamp(_SimdFloat4 _a, _SimdFloat4 _v, _SimdFloat4 _b) {
  const SimdFloat4 min = {_v.x < _b.x? _v.x : _b.x,
                          _v.y < _b.y? _v.y : _b.y,
                          _v.z < _b.z? _v.z : _b.z,
                          _v.w < _b.w? _v.w : _b.w};
  const SimdFloat4 r = {_a.x > min.x? _a.x : min.x,
                        _a.y > min.y? _a.y : min.y,
                        _a.z > min.z? _a.z : min.z,
                        _a.w > min.w? _a.w : min.w};
  return r;
}

OZZ_INLINE SimdFloat4 Select(
  _SimdInt4 _b, _SimdFloat4 _true, _SimdFloat4 _false) {
  using internal::SimdFI4;
  using internal::SimdIF4;

  const SimdFI4 i_true = {_true};
  const SimdFI4 i_false = {_false};
  const SimdIF4 ret = {{i_false.i.x ^ (_b.x & (i_true.i.x ^ i_false.i.x)),
                        i_false.i.y ^ (_b.y & (i_true.i.y ^ i_false.i.y)),
                        i_false.i.z ^ (_b.z & (i_true.i.z ^ i_false.i.z)),
                        i_false.i.w ^ (_b.w & (i_true.i.w ^ i_false.i.w))}};
  return ret.f;
}

OZZ_INLINE SimdInt4 CmpEq(_SimdFloat4 _a, _SimdFloat4 _b) {
  const SimdInt4 ret = {-static_cast<int>(_a.x == _b.x),
                        -static_cast<int>(_a.y == _b.y),
                        -static_cast<int>(_a.z == _b.z),
                        -static_cast<int>(_a.w == _b.w)};
  return ret;
}

OZZ_INLINE SimdInt4 CmpNe(_SimdFloat4 _a, _SimdFloat4 _b) {
  const SimdInt4 ret = {-static_cast<int>(_a.x != _b.x),
                        -static_cast<int>(_a.y != _b.y),
                        -static_cast<int>(_a.z != _b.z),
                        -static_cast<int>(_a.w != _b.w)};
  return ret;
}

OZZ_INLINE SimdInt4 CmpLt(_SimdFloat4 _a, _SimdFloat4 _b) {
  const SimdInt4 ret = {-static_cast<int>(_a.x < _b.x),
                        -static_cast<int>(_a.y < _b.y),
                        -static_cast<int>(_a.z < _b.z),
                        -static_cast<int>(_a.w < _b.w)};
  return ret;
}

OZZ_INLINE SimdInt4 CmpLe(_SimdFloat4 _a, _SimdFloat4 _b) {
  const SimdInt4 ret = {-static_cast<int>(_a.x <= _b.x),
                        -static_cast<int>(_a.y <= _b.y),
                        -static_cast<int>(_a.z <= _b.z),
                        -static_cast<int>(_a.w <= _b.w)};
  return ret;
}

OZZ_INLINE SimdInt4 CmpGt(_SimdFloat4 _a, _SimdFloat4 _b) {
  const SimdInt4 ret = {-static_cast<int>(_a.x > _b.x),
                        -static_cast<int>(_a.y > _b.y),
                        -static_cast<int>(_a.z > _b.z),
                        -static_cast<int>(_a.w > _b.w)};
  return ret;
}

OZZ_INLINE SimdInt4 CmpGe(_SimdFloat4 _a, _SimdFloat4 _b) {
  const SimdInt4 ret = {-static_cast<int>(_a.x >= _b.x),
                        -static_cast<int>(_a.y >= _b.y),
                        -static_cast<int>(_a.z >= _b.z),
                        -static_cast<int>(_a.w >= _b.w)};
  return ret;
}

OZZ_INLINE SimdFloat4 And(_SimdFloat4 _a, _SimdInt4 _b) {
  using internal::SimdFI4;
  using internal::SimdIF4;

  const SimdFI4 a = {_a};
  const SimdIF4 ret = {{
    a.i.x & _b.x, a.i.y & _b.y, a.i.z & _b.z, a.i.w & _b.w}};
  return ret.f;
}

OZZ_INLINE SimdFloat4 Or(_SimdFloat4 _a, _SimdInt4 _b) {
  using internal::SimdFI4;
  using internal::SimdIF4;

  const SimdFI4 a = {_a};
  const SimdIF4 ret = {{
    a.i.x | _b.x, a.i.y | _b.y, a.i.z | _b.z, a.i.w | _b.w}};
  return ret.f;
}

OZZ_INLINE SimdFloat4 Xor(_SimdFloat4 _a, _SimdInt4 _b) {
  using internal::SimdFI4;
  using internal::SimdIF4;

  const SimdFI4 a = {_a};
  const SimdIF4 ret = {{
    a.i.x ^ _b.x, a.i.y ^ _b.y, a.i.z ^ _b.z, a.i.w ^ _b.w}};
  return ret.f;
}

OZZ_INLINE SimdFloat4 Cos(_SimdFloat4 _v) {
  const SimdFloat4 ret = {
    std::cos(_v.x), std::cos(_v.y), std::cos(_v.z), std::cos(_v.w)};
  return ret;
}

OZZ_INLINE SimdFloat4 CosX(_SimdFloat4 _v) {
  const SimdFloat4 ret = {std::cos(_v.x), _v.y, _v.z, _v.w};
  return ret;
}

OZZ_INLINE SimdFloat4 ACos(_SimdFloat4 _v) {
  const SimdFloat4 ret = {std::acos(_v.x), std::acos(_v.y), std::acos(_v.z), std::acos(_v.w)};
  return ret;
}

OZZ_INLINE SimdFloat4 ACosX(_SimdFloat4 _v) {
  const SimdFloat4 ret = {std::acos(_v.x), _v.y, _v.z, _v.w};
  return ret;
}

OZZ_INLINE SimdFloat4 Sin(_SimdFloat4 _v) {
  const SimdFloat4 ret = {
    std::sin(_v.x), std::sin(_v.y), std::sin(_v.z), std::sin(_v.w)};
  return ret;
}

OZZ_INLINE SimdFloat4 SinX(_SimdFloat4 _v) {
  const SimdFloat4 ret = {std::sin(_v.x), _v.y, _v.z, _v.w};
  return ret;
}

OZZ_INLINE SimdFloat4 ASin(_SimdFloat4 _v) {
  const SimdFloat4 ret = {std::asin(_v.x), std::asin(_v.y), std::asin(_v.z), std::asin(_v.w)};
  return ret;
}

OZZ_INLINE SimdFloat4 ASinX(_SimdFloat4 _v) {
  const SimdFloat4 ret = {std::asin(_v.x), _v.y, _v.z, _v.w};
  return ret;
}

OZZ_INLINE SimdFloat4 Tan(_SimdFloat4 _v) {
  const SimdFloat4 ret = {
    std::tan(_v.x), std::tan(_v.y), std::tan(_v.z), std::tan(_v.w)};
  return ret;
}

OZZ_INLINE SimdFloat4 TanX(_SimdFloat4 _v) {
  const SimdFloat4 ret = {std::tan(_v.x), _v.y, _v.z, _v.w};
  return ret;
}

OZZ_INLINE SimdFloat4 ATan(_SimdFloat4 _v) {
  const SimdFloat4 ret = {std::atan(_v.x), std::atan(_v.y), std::atan(_v.z), std::atan(_v.w)};
  return ret;
}

OZZ_INLINE SimdFloat4 ATanX(_SimdFloat4 _v) {
  const SimdFloat4 ret = {std::atan(_v.x), _v.y, _v.z, _v.w};
  return ret;
}

namespace simd_int4 {

OZZ_INLINE SimdInt4 zero() {
  const SimdInt4 ret = {0, 0, 0, 0};
  return ret;
}

OZZ_INLINE SimdInt4 one() {
  const SimdInt4 ret = {1, 1, 1, 1};
  return ret;
}

OZZ_INLINE SimdInt4 x_axis() {
  const SimdInt4 ret = {1, 0, 0, 0};
  return ret;
}

OZZ_INLINE SimdInt4 y_axis() {
  const SimdInt4 ret = {0, 1, 0, 0};
  return ret;
}

OZZ_INLINE SimdInt4 z_axis() {
  const SimdInt4 ret = {0, 0, 1, 0};
  return ret;
}

OZZ_INLINE SimdInt4 w_axis() {
  const SimdInt4 ret = {0, 0, 0, 1};
  return ret;
}

OZZ_INLINE SimdInt4 all_true() {
  const SimdInt4 ret = {0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff};
  return ret;
}

OZZ_INLINE SimdInt4 all_false() {
  const SimdInt4 ret = {0, 0, 0, 0};
  return ret;
}

OZZ_INLINE SimdInt4 mask_sign() {
  const SimdInt4 ret = {0x80000000, 0x80000000, 0x80000000, 0x80000000};
  return ret;
}

OZZ_INLINE SimdInt4 mask_not_sign() {
  const SimdInt4 ret = {0x7fffffff, 0x7fffffff, 0x7fffffff, 0x7fffffff};
  return ret;
}

OZZ_INLINE SimdInt4 mask_ffff() {
  const SimdInt4 ret = {0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff};
  return ret;
}

OZZ_INLINE SimdInt4 mask_fff0() {
  const SimdInt4 ret = {0xffffffff, 0xffffffff, 0xffffffff, 0};
  return ret;
}

OZZ_INLINE SimdInt4 mask_0000() {
  const SimdInt4 ret = {0, 0, 0, 0};
  return ret;
}

OZZ_INLINE SimdInt4 mask_f000() {
  const SimdInt4 ret = {0xffffffff, 0, 0, 0};
  return ret;
}

OZZ_INLINE SimdInt4 mask_0f00() {
  const SimdInt4 ret = {0, 0xffffffff, 0, 0};
  return ret;
}

OZZ_INLINE SimdInt4 mask_00f0() {
  const SimdInt4 ret = {0, 0, 0xffffffff, 0};
  return ret;
}

OZZ_INLINE SimdInt4 mask_000f() {
  const SimdInt4 ret = {0, 0, 0, 0xffffffff};
  return ret;
}

OZZ_INLINE SimdInt4 Load(int _x, int _y, int _z, int _w) {
  const SimdInt4 ret = {_x, _y, _z, _w};
  return ret;
}

OZZ_INLINE SimdInt4 LoadX(int _x) {
  const SimdInt4 ret = {_x, 0, 0, 0};
  return ret;
}

OZZ_INLINE SimdInt4 Load1(int _x) {
  const SimdInt4 ret = {_x, _x, _x, _x};
  return ret;
}

OZZ_INLINE SimdInt4 Load(bool _x, bool _y, bool _z, bool _w) {
  const SimdInt4 ret = {-static_cast<int>(_x),
                        -static_cast<int>(_y),
                        -static_cast<int>(_z),
                        -static_cast<int>(_w)};
  return ret;
}

OZZ_INLINE SimdInt4 LoadX(bool _x) {
  const SimdInt4 ret = {-static_cast<int>(_x), 0, 0, 0};
  return ret;
}

OZZ_INLINE SimdInt4 Load1(bool _x) {
  const int i = -static_cast<int>(_x);
  const SimdInt4 ret = {i, i, i, i};
  return ret;
}

OZZ_INLINE SimdInt4 LoadPtr(const int* _i) {
  assert(!(uintptr_t(_i) & 0xf) && "Invalid alignment");
  const SimdInt4 ret = {_i[0], _i[1], _i[2], _i[3]};
  return ret;
}

OZZ_INLINE SimdInt4 LoadXPtr(const int* _i) {
  assert(!(uintptr_t(_i) & 0xf) && "Invalid alignment");
  const SimdInt4 ret = {*_i, 0, 0, 0};
  return ret;
}

OZZ_INLINE SimdInt4 Load1Ptr(const int* _i) {
  assert(!(uintptr_t(_i) & 0xf) && "Invalid alignment");
  const SimdInt4 ret = {*_i, *_i, *_i, *_i};
  return ret;
}

OZZ_INLINE SimdInt4 Load2Ptr(const int* _i) {
  assert(!(uintptr_t(_i) & 0xf) && "Invalid alignment");
  const SimdInt4 ret = {_i[0], _i[1], 0, 0};
  return ret;
}

OZZ_INLINE SimdInt4 Load3Ptr(const int* _i) {
  assert(!(uintptr_t(_i) & 0xf) && "Invalid alignment");
  const SimdInt4 ret = {_i[0], _i[1], _i[2], 0};
  return ret;
}

OZZ_INLINE SimdInt4 LoadPtrU(const int* _i) {
  assert(!(uintptr_t(_i) & 0x3) && "Invalid alignment");
  const SimdInt4 ret = {_i[0], _i[1], _i[2], _i[3]};
  return ret;
}

OZZ_INLINE SimdInt4 LoadXPtrU(const int* _i) {
  assert(!(uintptr_t(_i) & 0x3) && "Invalid alignment");
  const SimdInt4 ret = {*_i, 0, 0, 0};
  return ret;
}

OZZ_INLINE SimdInt4 Load1PtrU(const int* _i) {
  assert(!(uintptr_t(_i) & 0x3) && "Invalid alignment");
  const SimdInt4 ret = {*_i, *_i, *_i, *_i};
  return ret;
}

OZZ_INLINE SimdInt4 Load2PtrU(const int* _i) {
  assert(!(uintptr_t(_i) & 0x3) && "Invalid alignment");
  const SimdInt4 ret = {_i[0], _i[1], 0, 0};
  return ret;
}

OZZ_INLINE SimdInt4 Load3PtrU(const int* _i) {
  assert(!(uintptr_t(_i) & 0x3) && "Invalid alignment");
  const SimdInt4 ret = {_i[0], _i[1], _i[2], 0};
  return ret;
}
}  // simd_int4

OZZ_INLINE int GetX(_SimdInt4 _v) {
  return _v.x;
}

OZZ_INLINE int GetY(_SimdInt4 _v) {
  return _v.y;
}

OZZ_INLINE int GetZ(_SimdInt4 _v) {
  return _v.z;
}

OZZ_INLINE int GetW(_SimdInt4 _v) {
  return _v.w;
}

OZZ_INLINE SimdInt4 SetX(_SimdInt4 _v, int _i) {
  const SimdInt4 ret = {_i, _v.y, _v.z, _v.w};
  return ret;
}

OZZ_INLINE SimdInt4 SetY(_SimdInt4 _v, int _i) {
  const SimdInt4 ret = {_v.x, _i, _v.z, _v.w};
  return ret;
}

OZZ_INLINE SimdInt4 SetZ(_SimdInt4 _v, int _i) {
  const SimdInt4 ret = {_v.x, _v.y, _i, _v.w};
  return ret;
}

OZZ_INLINE SimdInt4 SetW(_SimdInt4 _v, int _i) {
  const SimdInt4 ret = {_v.x, _v.y, _v.z, _i};
  return ret;
}

OZZ_INLINE SimdInt4 SetI(_SimdInt4 _v, int _ith, int _i) {
  assert(_ith >= 0 && _ith <= 3 && "Invalid index ranges");
  SimdInt4 ret = _v;
  (&ret.x)[_ith] = _i;
  return ret;
}

OZZ_INLINE void StorePtr(_SimdInt4 _v, int* _i) {
  assert(!(uintptr_t(_i) & 0xf) && "Invalid alignment");
  _i[0] = _v.x;
  _i[1] = _v.y;
  _i[2] = _v.z;
  _i[3] = _v.w;
}

OZZ_INLINE void Store1Ptr(_SimdInt4 _v, int* _i) {
  assert(!(uintptr_t(_i) & 0xf) && "Invalid alignment");
  _i[0] = _v.x;
}

OZZ_INLINE void Store2Ptr(_SimdInt4 _v, int* _i) {
  assert(!(uintptr_t(_i) & 0xf) && "Invalid alignment");
  _i[0] = _v.x;
  _i[1] = _v.y;
}

OZZ_INLINE void Store3Ptr(_SimdInt4 _v, int* _i) {
  assert(!(uintptr_t(_i) & 0xf) && "Invalid alignment");
  _i[0] = _v.x;
  _i[1] = _v.y;
  _i[2] = _v.z;
}

OZZ_INLINE void StorePtrU(_SimdInt4 _v, int* _i) {
  assert(!(uintptr_t(_i) & 0x3) && "Invalid alignment");
  _i[0] = _v.x;
  _i[1] = _v.y;
  _i[2] = _v.z;
  _i[3] = _v.w;
}

OZZ_INLINE void Store1PtrU(_SimdInt4 _v, int* _i) {
  assert(!(uintptr_t(_i) & 0x3) && "Invalid alignment");
  _i[0] = _v.x;
}

OZZ_INLINE void Store2PtrU(_SimdInt4 _v, int* _i) {
  assert(!(uintptr_t(_i) & 0x3) && "Invalid alignment");
  _i[0] = _v.x;
  _i[1] = _v.y;
}

OZZ_INLINE void Store3PtrU(_SimdInt4 _v, int* _i) {
  assert(!(uintptr_t(_i) & 0x3) && "Invalid alignment");
  _i[0] = _v.x;
  _i[1] = _v.y;
  _i[2] = _v.z;
}

OZZ_INLINE SimdInt4 SplatX(_SimdInt4 _a) {
  const SimdInt4 ret = {_a.x, _a.x, _a.x, _a.x};
  return ret;
}

OZZ_INLINE SimdInt4 SplatY(_SimdInt4 _a) {
  const SimdInt4 ret = {_a.y, _a.y, _a.y, _a.y};
  return ret;
}

OZZ_INLINE SimdInt4 SplatZ(_SimdInt4 _a) {
  const SimdInt4 ret = {_a.z, _a.z, _a.z, _a.z};
  return ret;
}

OZZ_INLINE SimdInt4 SplatW(_SimdInt4 _a) {
  const SimdInt4 ret = {_a.w, _a.w, _a.w, _a.w};
  return ret;
}

OZZ_INLINE int MoveMask(_SimdInt4 _v) {
  return ((_v.x & 0x80000000) >> 31) |
         ((_v.y & 0x80000000) >> 30) |
         ((_v.z & 0x80000000) >> 29) |
         ((_v.w & 0x80000000) >> 28);
}

OZZ_INLINE bool AreAllTrue(_SimdInt4 _v) {
  return _v.x != 0 && _v.y != 0 && _v.z != 0 && _v.w != 0;
}

OZZ_INLINE bool AreAllTrue3(_SimdInt4 _v) {
  return _v.x != 0 && _v.y != 0 && _v.z != 0;
}

OZZ_INLINE bool AreAllTrue2(_SimdInt4 _v) {
  return _v.x != 0 && _v.y != 0;
}

OZZ_INLINE bool AreAllTrue1(_SimdInt4 _v) {
  return _v.x != 0;
}

OZZ_INLINE bool AreAllFalse(_SimdInt4 _v) {
  return _v.x == 0 && _v.y == 0 && _v.z == 0 && _v.w == 0;
}

OZZ_INLINE bool AreAllFalse3(_SimdInt4 _v) {
  return _v.x == 0 && _v.y == 0 && _v.z == 0;
}

OZZ_INLINE bool AreAllFalse2(_SimdInt4 _v) {
  return _v.x == 0 && _v.y == 0;
}

OZZ_INLINE bool AreAllFalse1(_SimdInt4 _v) {
  return _v.x == 0;
}

OZZ_INLINE SimdInt4 MAdd(_SimdInt4 _a, _SimdInt4 _b, _SimdInt4 _addend) {
  const SimdInt4 ret = {_a.x * _b.x + _addend.x,
                        _a.y * _b.y + _addend.y,
                        _a.z * _b.z + _addend.z,
                        _a.w * _b.w + _addend.w};
  return ret;
}

OZZ_INLINE SimdInt4 DivX(_SimdInt4 _a, _SimdInt4 _b) {
  const SimdInt4 ret = {_a.x / _b.x, _a.y, _a.z, _a.w};
  return ret;
}

OZZ_INLINE SimdInt4 HAdd2(_SimdInt4 _v) {
  const SimdInt4 ret = {_v.x + _v.y, _v.y, _v.z, _v.w};
  return ret;
}

OZZ_INLINE SimdInt4 HAdd3(_SimdInt4 _v) {
  const SimdInt4 ret = {_v.x + _v.y + _v.z, _v.y, _v.z, _v.w};
  return ret;
}

OZZ_INLINE SimdInt4 HAdd4(_SimdInt4 _v) {
  const SimdInt4 ret = {_v.x + _v.y + _v.z + _v.w, _v.y, _v.z, _v.w};
  return ret;
}

OZZ_INLINE SimdInt4 Dot2(_SimdInt4 _a, _SimdInt4 _b) {
  const SimdInt4 ret = {_a.x * _b.x + _a.y * _b.y, _a.y, _a.z, _a.w};
  return ret;
}

OZZ_INLINE SimdInt4 Dot3(_SimdInt4 _a, _SimdInt4 _b) {
  const SimdInt4 ret = {_a.x * _b.x + _a.y * _b.y + _a.z * _b.z,
                        _a.y,
                        _a.z,
                        _a.w};
  return ret;
}

OZZ_INLINE SimdInt4 Dot4(_SimdInt4 _a, _SimdInt4 _b) {
  const SimdInt4 ret = {_a.x * _b.x + _a.y * _b.y + _a.z * _b.z + _a.w * _b.w,
                        _a.y,
                        _a.z,
                        _a.w};
  return ret;
}
OZZ_INLINE SimdInt4 Abs(_SimdInt4 _v) {
  const SimdInt4 mash = {_v.x>>31, _v.y>>31, _v.z>>31, _v.w>>31};
  const SimdInt4 ret = {(_v.x + (mash.x)) ^ (mash.x),
                        (_v.y + (mash.y)) ^ (mash.y),
                        (_v.z + (mash.z)) ^ (mash.z),
                        (_v.w + (mash.w)) ^ (mash.w)};
  return ret;
}

OZZ_INLINE SimdInt4 Sign(_SimdInt4 _v) {
  const SimdInt4 ret = { _v.x & 0x80000000,
                         _v.y & 0x80000000,
                         _v.z & 0x80000000,
                         _v.w & 0x80000000};
  return ret;
}

OZZ_INLINE SimdInt4 Min(_SimdInt4 _a, _SimdInt4 _b) {
  const SimdInt4 ret = {_a.x < _b.x ? _a.x:_b.x,
                        _a.y < _b.y ? _a.y:_b.y,
                        _a.z < _b.z ? _a.z:_b.z,
                        _a.w < _b.w ? _a.w:_b.w};
  return ret;
}

OZZ_INLINE SimdInt4 Max(_SimdInt4 _a, _SimdInt4 _b) {
  const SimdInt4 ret = {_a.x > _b.x ? _a.x:_b.x,
                        _a.y > _b.y ? _a.y:_b.y,
                        _a.z > _b.z ? _a.z:_b.z,
                        _a.w > _b.w ? _a.w:_b.w};
  return ret;
}

OZZ_INLINE SimdInt4 Min0(_SimdInt4 _v) {
  const SimdInt4 ret = {_v.x < 0 ? _v.x : 0,
                        _v.y < 0 ? _v.y : 0,
                        _v.z < 0 ? _v.z : 0,
                        _v.w < 0 ? _v.w : 0};
  return ret;
}

OZZ_INLINE SimdInt4 Max0(_SimdInt4 _v) {
  const SimdInt4 ret = {_v.x > 0 ? _v.x : 0,
                        _v.y > 0 ? _v.y : 0,
                        _v.z > 0 ? _v.z : 0,
                        _v.w > 0 ? _v.w : 0};
  return ret;
}

OZZ_INLINE SimdInt4 Clamp(_SimdInt4 _a, _SimdInt4 _v, _SimdInt4 _b) {
  const SimdInt4 min = {_v.x < _b.x? _v.x : _b.x,
                        _v.y < _b.y? _v.y : _b.y,
                        _v.z < _b.z? _v.z : _b.z,
                        _v.w < _b.w? _v.w : _b.w};
  const SimdInt4 r = {_a.x > min.x? _a.x : min.x,
                      _a.y > min.y? _a.y : min.y,
                      _a.z > min.z? _a.z : min.z,
                      _a.w > min.w? _a.w : min.w};
  return r;
}

OZZ_INLINE SimdInt4 Select(_SimdInt4 _b, _SimdInt4 _true, _SimdInt4 _false) {
  const SimdInt4 ret = {_false.x ^ (_b.x & (_true.x ^ _false.x)),
                        _false.y ^ (_b.y & (_true.y ^ _false.y)),
                        _false.z ^ (_b.z & (_true.z ^ _false.z)),
                        _false.w ^ (_b.w & (_true.w ^ _false.w))};
  return ret;
}

OZZ_INLINE SimdInt4 And(_SimdInt4 _a, _SimdInt4 _b) {
  const SimdInt4 ret = {_a.x & _b.x, _a.y & _b.y, _a.z & _b.z, _a.w & _b.w};
  return ret;
}

OZZ_INLINE SimdInt4 Or(_SimdInt4 _a, _SimdInt4 _b) {
  const SimdInt4 ret = {_a.x | _b.x, _a.y | _b.y, _a.z | _b.z, _a.w | _b.w};
  return ret;
}

OZZ_INLINE SimdInt4 Xor(_SimdInt4 _a, _SimdInt4 _b) {
  const SimdInt4 ret = {_a.x ^ _b.x, _a.y ^ _b.y, _a.z ^ _b.z, _a.w ^ _b.w};
  return ret;
}

OZZ_INLINE SimdInt4 Not(_SimdInt4 _v) {
  const SimdInt4 ret = {~_v.x, ~_v.y, ~_v.z, ~_v.w};
  return ret;
}

OZZ_INLINE SimdInt4 ShiftL(_SimdInt4 _v, int _bits){
  const SimdInt4 ret = {
    _v.x << _bits, _v.y << _bits, _v.z << _bits, _v.w << _bits};
  return ret;
}

OZZ_INLINE SimdInt4 ShiftR(_SimdInt4 _v, int _bits) {
  const SimdInt4 ret = {
    _v.x >> _bits, _v.y >> _bits, _v.z >> _bits, _v.w >> _bits};
  return ret;
}

OZZ_INLINE SimdInt4 ShiftRu(_SimdInt4 _v, int _bits) {
  const union IU { int i[4]; unsigned int u[4]; } iu = {{
    _v.x, _v.y, _v.z, _v.w}};
  const union UI { unsigned int u[4]; int i[4]; } ui = {{
    iu.u[0] >> _bits, iu.u[1] >> _bits, iu.u[2] >> _bits, iu.u[3] >> _bits}};
  const SimdInt4 ret = {ui.i[0], ui.i[1], ui.i[2], ui.i[3]};
  return ret;
}

OZZ_INLINE SimdInt4 CmpEq(_SimdInt4 _a, _SimdInt4 _b) {
  const SimdInt4 ret = {-static_cast<int>(_a.x == _b.x),
                        -static_cast<int>(_a.y == _b.y),
                        -static_cast<int>(_a.z == _b.z),
                        -static_cast<int>(_a.w == _b.w)};
  return ret;
}

OZZ_INLINE SimdInt4 CmpNe(_SimdInt4 _a, _SimdInt4 _b) {
  const SimdInt4 ret = {-static_cast<int>(_a.x != _b.x),
                        -static_cast<int>(_a.y != _b.y),
                        -static_cast<int>(_a.z != _b.z),
                        -static_cast<int>(_a.w != _b.w)};
  return ret;
}

OZZ_INLINE SimdInt4 CmpLt(_SimdInt4 _a, _SimdInt4 _b) {
  const SimdInt4 ret = {-static_cast<int>(_a.x < _b.x),
                        -static_cast<int>(_a.y < _b.y),
                        -static_cast<int>(_a.z < _b.z),
                        -static_cast<int>(_a.w < _b.w)};
  return ret;
}

OZZ_INLINE SimdInt4 CmpLe(_SimdInt4 _a, _SimdInt4 _b) {
  const SimdInt4 ret = {-static_cast<int>(_a.x <= _b.x),
                        -static_cast<int>(_a.y <= _b.y),
                        -static_cast<int>(_a.z <= _b.z),
                        -static_cast<int>(_a.w <= _b.w)};
  return ret;
}

OZZ_INLINE SimdInt4 CmpGt(_SimdInt4 _a, _SimdInt4 _b) {
  const SimdInt4 ret = {-static_cast<int>(_a.x > _b.x),
                        -static_cast<int>(_a.y > _b.y),
                        -static_cast<int>(_a.z > _b.z),
                        -static_cast<int>(_a.w > _b.w)};
  return ret;
}

OZZ_INLINE SimdInt4 CmpGe(_SimdInt4 _a, _SimdInt4 _b) {
  const SimdInt4 ret = {-static_cast<int>(_a.x >= _b.x),
                        -static_cast<int>(_a.y >= _b.y),
                        -static_cast<int>(_a.z >= _b.z),
                        -static_cast<int>(_a.w >= _b.w)};
  return ret;
}

OZZ_INLINE Float4x4 Float4x4::identity() {
  const Float4x4 ret = {{{1.f, 0.f, 0.f, 0.f},
                         {0.f, 1.f, 0.f, 0.f},
                         {0.f, 0.f, 1.f, 0.f},
                         {0.f, 0.f, 0.f, 1.f}}};
  return ret;
}

OZZ_INLINE Float4x4 Transpose(const Float4x4& _m) {
  const Float4x4 ret = {{
    {_m.cols[0].x, _m.cols[1].x, _m.cols[2].x, _m.cols[3].x},
    {_m.cols[0].y, _m.cols[1].y, _m.cols[2].y, _m.cols[3].y},
    {_m.cols[0].z, _m.cols[1].z, _m.cols[2].z, _m.cols[3].z},
    {_m.cols[0].w, _m.cols[1].w, _m.cols[2].w, _m.cols[3].w}}};
  return ret;
}

OZZ_INLINE Float4x4 Invert(const Float4x4& _m) {
  const SimdFloat4* cols = _m.cols;
  const float a00 = cols[2].z * cols[3].w - cols[3].z * cols[2].w;
  const float a01 = cols[2].y * cols[3].w - cols[3].y * cols[2].w;
  const float a02 = cols[2].y * cols[3].z - cols[3].y * cols[2].z;
  const float a03 = cols[2].x * cols[3].w - cols[3].x * cols[2].w;
  const float a04 = cols[2].x * cols[3].z - cols[3].x * cols[2].z;
  const float a05 = cols[2].x * cols[3].y - cols[3].x * cols[2].y;
  const float a06 = cols[1].z * cols[3].w - cols[3].z * cols[1].w;
  const float a07 = cols[1].y * cols[3].w - cols[3].y * cols[1].w;
  const float a08 = cols[1].y * cols[3].z - cols[3].y * cols[1].z;
  const float a09 = cols[1].x * cols[3].w - cols[3].x * cols[1].w;
  const float a10 = cols[1].x * cols[3].z - cols[3].x * cols[1].z;
  const float a11 = cols[1].y * cols[3].w - cols[3].y * cols[1].w;
  const float a12 = cols[1].x * cols[3].y - cols[3].x * cols[1].y;
  const float a13 = cols[1].z * cols[2].w - cols[2].z * cols[1].w;
  const float a14 = cols[1].y * cols[2].w - cols[2].y * cols[1].w;
  const float a15 = cols[1].y * cols[2].z - cols[2].y * cols[1].z;
  const float a16 = cols[1].x * cols[2].w - cols[2].x * cols[1].w;
  const float a17 = cols[1].x * cols[2].z - cols[2].x * cols[1].z;
  const float a18 = cols[1].x * cols[2].y - cols[2].x * cols[1].y;

  const float b0x = cols[1].y * a00 - cols[1].z * a01 + cols[1].w * a02;
  const float b1x = -cols[1].x * a00 + cols[1].z * a03 - cols[1].w * a04;
  const float b2x = cols[1].x * a01 - cols[1].y * a03 + cols[1].w * a05;
  const float b3x = -cols[1].x * a02 + cols[1].y * a04 - cols[1].z * a05;

  const float b0y = -cols[0].y * a00 + cols[0].z * a01 - cols[0].w * a02;
  const float b1y = cols[0].x * a00 - cols[0].z * a03 + cols[0].w * a04;
  const float b2y = -cols[0].x * a01 + cols[0].y * a03 - cols[0].w * a05;
  const float b3y = cols[0].x * a02 - cols[0].y * a04 + cols[0].z * a05;

  const float b0z = cols[0].y * a06 - cols[0].z * a07 + cols[0].w * a08;
  const float b1z = -cols[0].x * a06 + cols[0].z * a09 - cols[0].w * a10;
  const float b2z = cols[0].x * a11 - cols[0].y * a09 + cols[0].w * a12;
  const float b3z = -cols[0].x * a08 + cols[0].y * a10 - cols[0].z * a12;

  const float b0w = -cols[0].y * a13 + cols[0].z * a14 - cols[0].w * a15;
  const float b1w = cols[0].x * a13 - cols[0].z * a16 + cols[0].w * a17;
  const float b2w = -cols[0].x * a14 + cols[0].y * a16 - cols[0].w * a18;
  const float b3w = cols[0].x * a15 - cols[0].y * a17 + cols[0].z * a18;

  const float det = cols[0].x * b0x +
                    cols[0].y * b1x +
                    cols[0].z * b2x +
                    cols[0].w * b3x;
  assert(det != 0.f && "Matrix is not invertible");
  const float inv_det = 1.f / det;

  const Float4x4 ret = {{{b0x * inv_det,
                          b0y * inv_det,
                          b0z * inv_det,
                          b0w * inv_det},
                         {b1x * inv_det,
                          b1y * inv_det,
                          b1z * inv_det,
                          b1w * inv_det},
                         {b2x * inv_det,
                          b2y * inv_det,
                          b2z * inv_det,
                          b2w * inv_det},
                         {b3x * inv_det,
                          b3y * inv_det,
                          b3z * inv_det,
                          b3w * inv_det}}};
  return ret;
}

Float4x4 Float4x4::Scaling(_SimdFloat4 _v) {
  const Float4x4 ret = {{{_v.x, 0.f, 0.f, 0.f},
                         {0.f, _v.y, 0.f, 0.f},
                         {0.f, 0.f, _v.z, 0.f},
                         {0.f, 0.f, 0.f, 1.f}}};
  return ret;
}

Float4x4 Float4x4::Translation(_SimdFloat4 _v) {
  const Float4x4 ret = {{{1.f, 0.f, 0.f, 0.f},
                         {0.f, 1.f, 0.f, 0.f},
                         {0.f, 0.f, 1.f, 0.f},
                         {_v.x, _v.y, _v.z, 1.f}}};
  return ret;
}

OZZ_INLINE Float4x4 Translate(const Float4x4& _m, _SimdFloat4 _v) {
  const Float4x4 ret = {{_m.cols[0],
                         _m.cols[1],
                         _m.cols[2],
                         {_m.cols[0].x * _v.x +
                          _m.cols[1].x * _v.y +
                          _m.cols[2].x * _v.z +
                          _m.cols[3].x,
                          _m.cols[0].y * _v.x +
                          _m.cols[1].y * _v.y +
                          _m.cols[2].y * _v.z +
                          _m.cols[3].y,
                          _m.cols[0].z * _v.x +
                          _m.cols[1].z * _v.y +
                          _m.cols[2].z * _v.z +
                          _m.cols[3].z,
                          _m.cols[0].w * _v.x +
                          _m.cols[1].w * _v.y +
                          _m.cols[2].w * _v.z +
                          _m.cols[3].w}}};
  return ret;
}

OZZ_INLINE Float4x4 Scale(const Float4x4& _m, _SimdFloat4 _v) {
  const Float4x4 ret = {{{_m.cols[0].x * _v.x,
                          _m.cols[0].y * _v.x,
                          _m.cols[0].z * _v.x,
                          _m.cols[0].w * _v.x},
                         {_m.cols[1].x * _v.y,
                          _m.cols[1].y * _v.y,
                          _m.cols[1].z * _v.y,
                          _m.cols[1].w * _v.y},
                         {_m.cols[2].x * _v.z,
                          _m.cols[2].y * _v.z,
                          _m.cols[2].z * _v.z,
                          _m.cols[2].w * _v.z},
                         _m.cols[3]}};
  return ret;
}

OZZ_INLINE SimdInt4 IsNormalized(const Float4x4& _m) {
  const SimdInt4 ret = {IsNormalized3(_m.cols[0]).x,
                        IsNormalized3(_m.cols[1]).x,
                        IsNormalized3(_m.cols[2]).x,
                        0};
  return ret;
}

OZZ_INLINE SimdInt4 IsNormalizedEst(const Float4x4& _m) {
  const SimdInt4 ret = {IsNormalizedEst3(_m.cols[0]).x,
                        IsNormalizedEst3(_m.cols[1]).x,
                        IsNormalizedEst3(_m.cols[2]).x,
                        0};
  return ret;
}

OZZ_INLINE SimdInt4 IsOrthogonal(const Float4x4& _m) {
  // Use simd_float4::zero() if one of the normalization fails. _m will then be
  // considered not orthogonal.
  const SimdFloat4 cross =
    NormalizeSafe3(Cross3(_m.cols[0], _m.cols[1]), simd_float4::zero());
  const SimdFloat4 at = NormalizeSafe3(_m.cols[2], simd_float4::zero());

  const float sq_len = cross.x * at.x + cross.y * at.y + cross.z * at.z;
  const bool same = std::abs(sq_len - 1.f) < kNormalizationTolerance;
  const SimdInt4 ret = {-static_cast<int>(same), 0, 0, 0};
  return ret;
}

OZZ_INLINE SimdFloat4 ToQuaternion(const Float4x4& _m) {
  assert(AreAllTrue3(IsNormalized(_m)));
  assert(AreAllTrue1(IsOrthogonal(_m)));
  // Cf From Quaternion to Matrix and Back, J.M.P. van Waveren 2005.
  SimdFloat4 ret;
  if (_m.cols[0].x + _m.cols[1].y + _m.cols[2].z > .0f) {
    const float t = _m.cols[0].x + _m.cols[1].y + _m.cols[2].z + 1.0f;
    const float s = (1.f / std::sqrt(t)) * .5f;
    ret.x = (_m.cols[1].z - _m.cols[2].y) * s;
    ret.y = (_m.cols[2].x - _m.cols[0].z) * s;
    ret.z = (_m.cols[0].y - _m.cols[1].x) * s;
    ret.w = s * t;
  } else if (_m.cols[0].x > _m.cols[1].y && _m.cols[0].x > _m.cols[2].z) {
    const float t = _m.cols[0].x - _m.cols[1].y - _m.cols[2].z + 1.0f;
    const float s = (1.f / std::sqrt(t)) * .5f;
    ret.x = s * t;
    ret.y = (_m.cols[0].y + _m.cols[1].x) * s;
    ret.z = (_m.cols[2].x + _m.cols[0].z) * s;
    ret.w = (_m.cols[1].z - _m.cols[2].y) * s;
  } else if (_m.cols[1].y > _m.cols[2].z) {
    const float t = -_m.cols[0].x + _m.cols[1].y - _m.cols[2].z + 1.0f;
    const float s = (1.f / std::sqrt(t)) * .5f;
    ret.x = (_m.cols[0].y + _m.cols[1].x) * s;
    ret.y = s * t;
    ret.z = (_m.cols[1].z + _m.cols[2].y) * s;
    ret.w = (_m.cols[2].x - _m.cols[0].z) * s;
  } else {
    const float t = -_m.cols[0].x - _m.cols[1].y + _m.cols[2].z + 1.0f;
    const float s = (1.f / std::sqrt(t)) * .5f;
    ret.x = (_m.cols[2].x + _m.cols[0].z) * s;
    ret.y = (_m.cols[1].z + _m.cols[2].y) * s;
    ret.z = s * t;
    ret.w = (_m.cols[0].y - _m.cols[1].x) * s;
  }
  assert(AreAllTrue1(IsNormalizedEst4(ret)));
  return ret;
}

OZZ_INLINE bool ToAffine(const Float4x4& _m,
                         SimdFloat4* _translation,
                         SimdFloat4* _quaternion,
                         SimdFloat4* _scale) {
  _translation->x = _m.cols[3].x;
  _translation->y = _m.cols[3].y;
  _translation->z = _m.cols[3].z;
  _translation->w = 1.f;

  // Extracts scale.
  const float scale_x = Length3(_m.cols[0]).x;
  const float scale_y = Length3(_m.cols[1]).x;
  const float scale_z = Length3(_m.cols[2]).x;

  const bool x_zero = std::abs(scale_x) < kNormalizationTolerance;
  const bool y_zero = std::abs(scale_y) < kNormalizationTolerance;
  const bool z_zero = std::abs(scale_z) < kNormalizationTolerance;

  // Builds an orthonormal matrix in order to support quaternion extraction.
  Float4x4 orthonormal;
  if (x_zero) {
    if (y_zero || z_zero) {
      return false;
    }
    orthonormal.cols[1].x = _m.cols[1].x / scale_y;
    orthonormal.cols[1].y = _m.cols[1].y / scale_y;
    orthonormal.cols[1].z = _m.cols[1].z / scale_y;
    orthonormal.cols[1].w = 0.f;
    orthonormal.cols[0] = Normalize3(
      Cross3(orthonormal.cols[1], _m.cols[2]));
    orthonormal.cols[2] = Normalize3(
      Cross3(orthonormal.cols[0], orthonormal.cols[1]));
  } else if (z_zero) {
    if (x_zero || y_zero) {
      return false;
    }
    orthonormal.cols[0].x = _m.cols[0].x / scale_x;
    orthonormal.cols[0].y = _m.cols[0].y / scale_x;
    orthonormal.cols[0].z = _m.cols[0].z / scale_x;
    orthonormal.cols[0].w = 0.f;
    orthonormal.cols[2] = Normalize3(
      Cross3(orthonormal.cols[0], _m.cols[1]));
    orthonormal.cols[1] = Normalize3(
      Cross3(orthonormal.cols[2], orthonormal.cols[0]));
  } else {  // Favor z axis in the default case
    if (x_zero || z_zero) {
      return false;
    }
    orthonormal.cols[2].x = _m.cols[2].x / scale_z;
    orthonormal.cols[2].y = _m.cols[2].y / scale_z;
    orthonormal.cols[2].z = _m.cols[2].z / scale_z;
    orthonormal.cols[2].w = 0.f;
    orthonormal.cols[1] = Normalize3(
      Cross3(orthonormal.cols[2], _m.cols[0]));
    orthonormal.cols[0] = Normalize3(
      Cross3(orthonormal.cols[1], orthonormal.cols[2]));
  }

  // orthonormal.cols[3] = simd_float4::w_axis();  Not used by ToQuaternion.

  // Get back scale signs in case of reflexions
  _scale->x = Dot3(orthonormal.cols[0], _m.cols[0]).x > 0.f ?
    scale_x : -scale_x;
  _scale->y = Dot3(orthonormal.cols[1], _m.cols[1]).x > 0.f ?
    scale_y : -scale_y;
  _scale->z = Dot3(orthonormal.cols[2], _m.cols[2]).x > 0.f ?
    scale_z : -scale_z;
  _scale->w = 1.f;

  // Extracts quaternion.
  *_quaternion = ToQuaternion(orthonormal);
  return true;
}

/*


#define XMRANKDECOMPOSE(a, b, c, x, y, z)      \
    if((x) < (y))                   \
    {                               \
        if((y) < (z))               \
        {                           \
            (a) = 2;                \
            (b) = 1;                \
            (c) = 0;                \
        }                           \
        else                        \
        {                           \
            (a) = 1;                \
                                    \
            if((x) < (z))           \
            {                       \
                (b) = 2;            \
                (c) = 0;            \
            }                       \
            else                    \
            {                       \
                (b) = 0;            \
                (c) = 2;            \
            }                       \
        }                           \
    }                               \
    else                            \
    {                               \
        if((x) < (z))               \
        {                           \
            (a) = 2;                \
            (b) = 0;                \
            (c) = 1;                \
        }                           \
        else                        \
        {                           \
            (a) = 0;                \
                                    \
            if((y) < (z))           \
            {                       \
                (b) = 2;            \
                (c) = 1;            \
            }                       \
            else                    \
            {                       \
                (b) = 1;            \
                (c) = 2;            \
            }                       \
        }                           \
    }
                                    
#define XM_DECOMP_EPSILON 0.0001f

OZZ_INLINE bool ToAffine(const Float4x4& _m,
                         SimdFloat4* _translation,
                         SimdFloat4* _quaternion,
                         SimdFloat4* _scale) {
  float fDet;
  float *pfScales;

  SimdFloat4 *ppvBasis[3];
  Float4x4 matTemp;
  unsigned int a, b, c;
  const SimdFloat4 pvCanonicalBasis[3] = {
    simd_float4::x_axis(),
    simd_float4::y_axis(),
    simd_float4::z_axis()};

  // Get the translation
  _translation[0].x = _m.cols[3].x;
  _translation[0].y = _m.cols[3].y;
  _translation[0].z = _m.cols[3].z;
  _translation[0].w = 1.f;

  ppvBasis[0] = &matTemp.cols[0];
  ppvBasis[1] = &matTemp.cols[1];
  ppvBasis[2] = &matTemp.cols[2];

  matTemp.r[0] = _m.cols[0];
  matTemp.r[1] = _m.cols[1];
  matTemp.r[2] = _m.cols[2];
  matTemp.r[3] = simd_float4::w_axis();

  pfScales = &_scale->x;

  scales->x = Length3(*ppvBasis[0]);
  scales->y = Length3(*ppvBasis[1]);
  scales->z = Length3(*ppvBasis[2]);

  XMRANKDECOMPOSE(a, b, c, pfScales[0], pfScales[1], pfScales[2])

  if(pfScales[a] < XM_DECOMP_EPSILON)
  {
    *ppvBasis[a] = pvCanonicalBasis[a];
  }
  *ppvBasis[a] = Normalize3(*ppvBasis[a]);

  if(pfScales[b] < XM_DECOMP_EPSILON)
  {
    UINT aa, bb, cc;
    FLOAT fAbsX, fAbsY, fAbsZ;

    fAbsX = std::absf(ppvBasis[a]->x);
    fAbsY = std::absf(ppvBasis[a]->y);
    fAbsZ = std::absf(ppvBasis[a]->z);

    XMRANKDECOMPOSE(aa, bb, cc, fAbsX, fAbsY, fAbsZ)

    *ppvBasis[b] = Cross3(*ppvBasis[a], pvCanonicalBasis[cc]);
  }

  *ppvBasis[b] = Normalize3(*ppvBasis[b]);

  if(pfScales[c] < XM_DECOMP_EPSILON)
  {
    *ppvBasis[c] = Cross3(*ppvBasis[a], *ppvBasis[b]);
  }
    
  *ppvBasis[c] = Normalize3(ppvBasis[c]);

  fDet = XMVectorGetX(XMMatrixDeterminant(matTemp));

  // use Kramer's rule to check for handedness of coordinate system
  if(fDet < 0.0f)
  {
    // switch coordinate system by negating the scale and inverting the basis vector on the x-axis
    pfScales[a] = -pfScales[a];
    ppvBasis[a][0] = XMVectorNegate(ppvBasis[a][0]);

    fDet = -fDet;
  }

  fDet -= 1.0f;
  fDet *= fDet;

  if(XM_DECOMP_EPSILON < fDet)
  {
//    Non-SRT matrix encountered
    return FALSE;
  }

  // generate the quaternion from the matrix
  outRotQuat[0] = XMQuaternionRotationMatrix(matTemp);
    return TRUE;
}
*/
OZZ_INLINE Float4x4 Float4x4::FromEuler(_SimdFloat4 _v) {
  const float ch = std::cos(_v.x);
  const float sh = std::sin(_v.x);
  const float ca = std::cos(_v.y);
  const float sa = std::sin(_v.y);
  const float cb = std::cos(_v.z);
  const float sb = std::sin(_v.z);

  const float sa_cb = sa*cb;
  const float sa_sb = sa*sb;

  const Float4x4 ret = {{{ch * ca,
                          sh * sb - ch * sa_cb,
                          ch * sa_sb + sh * cb,
                          0.f},
                         {sa,
                          ca * cb,
                          -ca * sb,
                          0.f},
                         {-sh * ca,
                          sh * sa_cb + ch * sb,
                          -sh * sa_sb + ch * cb,
                          0.f},
                         {0.f, 0.f, 0.f, 1.f}}};
  return ret;
}

OZZ_INLINE Float4x4 Float4x4::FromAxisAngle(_SimdFloat4 _v) {
  assert(AreAllTrue1(IsNormalizedEst3(_v)));

  const float cos = std::cos(_v.w);
  const float sin = std::sin(_v.w);
  const float t = 1.f - cos;

  const float a = _v.x * _v.y * t;
  const float b = _v.z * sin;
  const float c = _v.x * _v.z * t;
  const float d = _v.y * sin;
  const float e = _v.y * _v.z * t;
  const float f = _v.x * sin;

  const Float4x4 ret = {{{cos + _v.x * _v.x * t, a + b, c - d},
                         {a - b, cos + _v.y * _v.y * t, e + f, 0.f},
                         {c + d, e - f, cos + _v.z * _v.z * t, 0.f},
                         {0.f, 0.f, 0.f, 1.f}}};
  return ret;
}

OZZ_INLINE Float4x4 Float4x4::FromQuaternion(_SimdFloat4 _v) {
  assert(AreAllTrue1(IsNormalizedEst4(_v)));

  const float xx = _v.x * _v.x;
  const float xy = _v.x * _v.y;
  const float xz = _v.x * _v.z;
  const float xw = _v.x * _v.w;
  const float yy = _v.y * _v.y;
  const float yz = _v.y * _v.z;
  const float yw = _v.y * _v.w;
  const float zz = _v.z * _v.z;
  const float zw = _v.z * _v.w;

  const Float4x4 ret = {{{1.f - 2.f * (yy + zz),
                          2.f * (xy + zw),
                          2.f * (xz - yw),
                          0.f},
                         {2.f * (xy - zw),
                          1.f - 2.f * (xx + zz),
                          2.f * (yz + xw),
                          0.f},
                         {2.f * (xz + yw),
                          2.f * (yz - xw),
                          1.f - 2.f * (xx + yy),
                          0.f},
                         {0.f,
                          0.f,
                          0.f,
                          1.f}}};
  return ret;
}

OZZ_INLINE Float4x4 Float4x4::FromAffine(_SimdFloat4 _translation,
                                         _SimdFloat4 _quaternion,
                                         _SimdFloat4 _scale) {
  assert(AreAllTrue1(IsNormalizedEst4(_quaternion)));

  const float xx = _quaternion.x * _quaternion.x;
  const float xy = _quaternion.x * _quaternion.y;
  const float xz = _quaternion.x * _quaternion.z;
  const float xw = _quaternion.x * _quaternion.w;
  const float yy = _quaternion.y * _quaternion.y;
  const float yz = _quaternion.y * _quaternion.z;
  const float yw = _quaternion.y * _quaternion.w;
  const float zz = _quaternion.z * _quaternion.z;
  const float zw = _quaternion.z * _quaternion.w;

  const Float4x4 ret = {{{_scale.x * (1.f - 2.f * (yy + zz)),
                          _scale.x * 2.f * (xy + zw),
                          _scale.x * 2.f * (xz - yw),
                          0.f},
                         {_scale.y * 2.f * (xy - zw),
                          _scale.y * (1.f - 2.f * (xx + zz)),
                          _scale.y * (2.f * (yz + xw)),
                          0.f},
                         {_scale.z * 2.f * (xz + yw),
                          _scale.z * 2.f * (yz - xw),
                          _scale.z * (1.f - 2.f * (xx + yy)),
                          0.f},
                         {_translation.x,
                          _translation.y,
                          _translation.z,
                          1.f}}};
  return ret;
}
}  // math
}  // ozz

OZZ_INLINE ozz::math::SimdFloat4 operator+(
  ozz::math::_SimdFloat4 _a, ozz::math::_SimdFloat4 _b) {
  const ozz::math::SimdFloat4 ret = {
    _a.x + _b.x, _a.y + _b.y, _a.z + _b.z, _a.w + _b.w};
  return ret;
}

OZZ_INLINE ozz::math::SimdFloat4 operator-(
  ozz::math::_SimdFloat4 _a, ozz::math::_SimdFloat4 _b) {
  const ozz::math::SimdFloat4 ret = {
    _a.x - _b.x, _a.y - _b.y, _a.z - _b.z, _a.w - _b.w};
  return ret;
}

OZZ_INLINE ozz::math::SimdFloat4 operator-(ozz::math::_SimdFloat4 _v) {
  const ozz::math::SimdFloat4 ret = {-_v.x, -_v.y, -_v.z, -_v.w};
  return ret;
}

OZZ_INLINE ozz::math::SimdFloat4 operator*(
  ozz::math::_SimdFloat4 _a, ozz::math::_SimdFloat4 _b) {
  const ozz::math::SimdFloat4 ret = {
    _a.x * _b.x, _a.y * _b.y, _a.z * _b.z, _a.w * _b.w};
  return ret;
}

OZZ_INLINE ozz::math::SimdFloat4 operator/(
  ozz::math::_SimdFloat4 _a, ozz::math::_SimdFloat4 _b) {
  const ozz::math::SimdFloat4 ret = {
    _a.x / _b.x, _a.y / _b.y, _a.z / _b.z, _a.w / _b.w};
  return ret;
}

OZZ_INLINE ozz::math::SimdFloat4 operator*(
  const ozz::math::Float4x4& _m, ozz::math::_SimdFloat4 _v) {
  const ozz::math::SimdFloat4 ret = {_m.cols[0].x * _v.x +
                                     _m.cols[1].x * _v.y +
                                     _m.cols[2].x * _v.z +
                                     _m.cols[3].x * _v.w,
                                     _m.cols[0].y * _v.x +
                                     _m.cols[1].y * _v.y +
                                     _m.cols[2].y * _v.z +
                                     _m.cols[3].y * _v.w,
                                     _m.cols[0].z * _v.x +
                                     _m.cols[1].z * _v.y +
                                     _m.cols[2].z * _v.z +
                                     _m.cols[3].z * _v.w,
                                     _m.cols[0].w * _v.x +
                                     _m.cols[1].w * _v.y +
                                     _m.cols[2].w * _v.z +
                                     _m.cols[3].w * _v.w};
  return ret;
}

OZZ_INLINE ozz::math::Float4x4 operator*(
  const ozz::math::Float4x4& _a, const ozz::math::Float4x4& _b) {
  const ozz::math::Float4x4 ret = {{_a * _b.cols[0],
                                    _a * _b.cols[1],
                                    _a * _b.cols[2],
                                    _a * _b.cols[3]}};
  return ret;
}

OZZ_INLINE ozz::math::Float4x4 operator+(
  const ozz::math::Float4x4& _a, const ozz::math::Float4x4& _b) {
  const ozz::math::Float4x4 ret = {{{_a.cols[0].x + _b.cols[0].x,
                                     _a.cols[0].y + _b.cols[0].y,
                                     _a.cols[0].z + _b.cols[0].z,
                                     _a.cols[0].w + _b.cols[0].w},
                                    {_a.cols[1].x + _b.cols[1].x,
                                     _a.cols[1].y + _b.cols[1].y,
                                     _a.cols[1].z + _b.cols[1].z,
                                     _a.cols[1].w + _b.cols[1].w},
                                    {_a.cols[2].x + _b.cols[2].x,
                                     _a.cols[2].y + _b.cols[2].y,
                                     _a.cols[2].z + _b.cols[2].z,
                                     _a.cols[2].w + _b.cols[2].w},
                                    {_a.cols[3].x + _b.cols[3].x,
                                     _a.cols[3].y + _b.cols[3].y,
                                     _a.cols[3].z + _b.cols[3].z,
                                     _a.cols[3].w + _b.cols[3].w}}};
  return ret;
}

OZZ_INLINE ozz::math::Float4x4 operator-(
  const ozz::math::Float4x4& _a, const ozz::math::Float4x4& _b) {
  const ozz::math::Float4x4 ret = {{{_a.cols[0].x - _b.cols[0].x,
                                     _a.cols[0].y - _b.cols[0].y,
                                     _a.cols[0].z - _b.cols[0].z,
                                     _a.cols[0].w - _b.cols[0].w},
                                    {_a.cols[1].x - _b.cols[1].x,
                                     _a.cols[1].y - _b.cols[1].y,
                                     _a.cols[1].z - _b.cols[1].z,
                                     _a.cols[1].w - _b.cols[1].w},
                                    {_a.cols[2].x - _b.cols[2].x,
                                     _a.cols[2].y - _b.cols[2].y,
                                     _a.cols[2].z - _b.cols[2].z,
                                     _a.cols[2].w - _b.cols[2].w},
                                    {_a.cols[3].x - _b.cols[3].x,
                                     _a.cols[3].y - _b.cols[3].y,
                                     _a.cols[3].z - _b.cols[3].z,
                                     _a.cols[3].w - _b.cols[3].w}}};
  return ret;
}

#undef OZZ_RCP_EST
#undef OZZ_RSQRT_EST
#endif  // OZZ_OZZ_BASE_MATHS_INTERNAL_SIMD_MATH_REF_INL_H_
