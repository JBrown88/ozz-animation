//----------------------------------------------------------------------------//
//                                                                            //
// ozz-animation is hosted at http://github.com/guillaumeblanc/ozz-animation  //
// and distributed under the MIT License (MIT).                               //
//                                                                            //
// Copyright (c) Guillaume Blanc                                              //
//                                                                            //
// Permission is hereby granted, free of charge, to any person obtaining a    //
// copy of this software and associated documentation files (the "Software"), //
// to deal in the Software without restriction, including without limitation  //
// the rights to use, copy, modify, merge, publish, distribute, sublicense,   //
// and/or sell copies of the Software, and to permit persons to whom the      //
// Software is furnished to do so, subject to the following conditions:       //
//                                                                            //
// The above copyright notice and this permission notice shall be included in //
// all copies or substantial portions of the Software.                        //
//                                                                            //
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR //
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   //
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    //
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER //
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING    //
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER        //
// DEALINGS IN THE SOFTWARE.                                                  //
//                                                                            //
//----------------------------------------------------------------------------//

#include "ozz/animation/runtime/sampling_job.h"

#include <cassert>

#include "ozz/animation/runtime/animation.h"
#include "ozz/base/maths/math_constant.h"
#include "ozz/base/maths/math_ex.h"
#include "ozz/base/maths/soa_transform.h"
#include "ozz/base/memory/allocator.h"

// Internal include file
#define OZZ_INCLUDE_PRIVATE_HEADER  // Allows to include private headers.
#include "animation/runtime/animation_keyframe.h"

namespace ozz {
namespace animation {

namespace internal {
struct InterpSoaFloat3 {
  math::SimdFloat4 ratio[2];
  math::SoaFloat3 value[2];
};
struct InterpSoaQuaternion {
  math::SimdFloat4 ratio[2];
  math::SoaQuaternion value[2];
};
}  // namespace internal

bool SamplingJob::Validate() const {
  // Don't need any early out, as jobs are valid in most of the performance
  // critical cases.
  // Tests are written in multiple lines in order to avoid branches.
  bool valid = true;

  // Test for nullptr pointers.
  if (!animation || !context) {
    return false;
  }
  valid &= !output.empty();

  const int num_soa_tracks = animation->num_soa_tracks();
  valid &= output.size() >= static_cast<size_t>(num_soa_tracks);

  // Tests context size.
  valid &= context->max_soa_tracks() >= num_soa_tracks;

  return valid;
}

namespace {
template <typename _Key>
inline int TrackForward(int* _cache, const ozz::span<const _Key>& _keys,
                        int _key, int _last_track, int _num_tracks) {
  assert(_key < static_cast<int>(_keys.size()));
  assert(_last_track >= 0 && _last_track < _num_tracks);

  const int target = _key - _keys[_key].previous;
  for (int entry = _last_track + _num_tracks; entry < _num_tracks * 2;
       ++entry) {
    if (_cache[entry] == target) {
      return entry - _num_tracks;
    }
  }
  for (int entry = _num_tracks;; ++entry) {
    if (_cache[entry] == target) {
      return entry - _num_tracks;
    }
    assert(entry < _last_track * _num_tracks &&
           "Previous track should be in cache");
  }
}

inline int TrackBackward(int* _cache, int _target, int _last_track,
                         int _num_tracks) {
  assert(_last_track >= 0 && _last_track < _num_tracks);

  for (int entry = _last_track + _num_tracks; entry >= _num_tracks; --entry) {
    if (_cache[entry] == _target) {
      return entry - _num_tracks;
    }
  }
  for (int entry = _num_tracks * 2 - 1;; --entry) {
    if (_cache[entry] == _target) {
      return entry - _num_tracks;
    }
    assert(entry > _last_track + _num_tracks &&
           "Previous track should be in cache");
  }
}

// Loops through the sorted key frames and update cache structure.
template <typename _Key>
inline void UpdateCacheCursor(float _ratio, int _num_soa_tracks,
                              const ozz::span<const _Key>& _keys, int* _cursor,
                              int* _cache, unsigned char* _outdated) {
  assert(_num_soa_tracks >= 1);
  const int num_tracks = _num_soa_tracks * 4;
  const int num_keys = static_cast<int>(_keys.size());
  assert(_keys.begin() + num_tracks * 2 <= _keys.end());

  int cursor = *_cursor;
  if (!cursor) {
    // Initializes cache entries with the first 2 sets of key frames.
    // The sorting algorithm ensures that the first 2 key frames of a track
    // are consecutive.
    for (; cursor < num_tracks * 2; ++cursor) {
      _cache[cursor] = cursor;
    }

    // All entries are outdated. It cares to only flag valid soa entries as
    // this is the exit condition of other algorithms.
    const int num_outdated_flags = (_num_soa_tracks + 7) / 8;
    for (int i = 0; i < num_outdated_flags - 1; ++i) {
      _outdated[i] = 0xff;
    }
    _outdated[num_outdated_flags - 1] =
        0xff >> (num_outdated_flags * 8 - _num_soa_tracks);
  }
  assert(cursor >= num_tracks * 2 && cursor <= num_keys);

  // Reading forward.
  // Iterates while the cache is not updated with previous and penultimate keys
  // required for interpolation at time ratio _ratio. Thanks to the keyframe
  // sorting, the loop can end as soon as it finds a key greater that _ratio. It
  // will mean that all the keys lower than _ratio have been processed, meaning
  // all cache entries are up to date.
  int track = 0;
  for (; cursor < num_keys &&
         _keys[cursor - _keys[cursor].previous].ratio <= _ratio;
       ++cursor) {
    // Finds track index.
    track = TrackForward(_cache, _keys, cursor, track, num_tracks);

    // Flag this soa entry as outdated.
    _outdated[track / 32] |= 1 << ((track & 0x1f) / 4);
    // Updates cache.
    const int penultimate = track;
    const int last = track + num_tracks;
    assert(_cache[last] == cursor - _keys[cursor].previous &&
           "Wrong cache entry.");
    _cache[penultimate] = _cache[last];
    _cache[last] = cursor;
  }

  // Rewinds.
  // Checks if the time of the penultimate key is greater than _ratio, in which
  // case we need to rewind.
  for (; _keys[(cursor - 1) - _keys[cursor - 1].previous].ratio > _ratio;
       --cursor) {
    assert(cursor - 1 >= num_tracks * 2);

    // Finds track index.
    track = TrackBackward(_cache, cursor - 1, track, num_tracks);

    // Flag this soa entry as outdated.
    _outdated[track / 32] |= 1 << ((track & 0x1f) / 4);

    // Updates cache.
    const int penultimate = track;
    const int last = track + num_tracks;
    assert(_cache[last] == cursor - 1 && "Wrong cache entry.");
    _cache[last] = _cache[penultimate];
    const int previous = _keys[_cache[penultimate]].previous;
    assert(_cache[penultimate] >= previous);
    _cache[penultimate] -= previous;
  }

  // Updates cursor output.
  assert(cursor >= num_tracks * 2 && cursor <= num_keys);
  *_cursor = cursor;
}

template <typename _Key, typename _InterpKey, typename _Decompress>
inline void UpdateInterpKeyframes(int _num_soa_tracks,
                                  const ozz::span<const _Key>& _keys,
                                  const int* _cache, uint8_t* _outdated,
                                  _InterpKey* _interp_keys,
                                  const _Decompress& _decompress) {
  const int num_outdated_flags = (_num_soa_tracks + 7) / 8;
  for (int j = 0; j < num_outdated_flags; ++j) {
    uint8_t outdated = _outdated[j];
    _outdated[j] = 0;  // Reset outdated entries as all will be processed.
    for (int i = j * 8; outdated; ++i, outdated >>= 1) {
      if (!(outdated & 1)) {
        continue;
      }

      const int penultimates = i * 4;  // * soa size
      // Decompress left side keyframes and store them in soa structures.
      const _Key& k00 = _keys[_cache[penultimates + 0]];
      const _Key& k10 = _keys[_cache[penultimates + 1]];
      const _Key& k20 = _keys[_cache[penultimates + 2]];
      const _Key& k30 = _keys[_cache[penultimates + 3]];
      _interp_keys[i].ratio[0] =
          math::simd_float4::Load(k00.ratio, k10.ratio, k20.ratio, k30.ratio);
      _decompress(k00, k10, k20, k30, &_interp_keys[i].value[0]);

      // Decompress right side keyframes and store them in soa structures.
      const int lasts = (i + _num_soa_tracks) * 4;  // * soa size
      const _Key& k01 = _keys[_cache[lasts + 0]];
      const _Key& k11 = _keys[_cache[lasts + 1]];
      const _Key& k21 = _keys[_cache[lasts + 2]];
      const _Key& k31 = _keys[_cache[lasts + 3]];
      _interp_keys[i].ratio[1] =
          math::simd_float4::Load(k01.ratio, k11.ratio, k21.ratio, k31.ratio);
      _decompress(k01, k11, k21, k31, &_interp_keys[i].value[1]);
    }
  }
}

inline void DecompressFloat3(const Float3Key& _k0, const Float3Key& _k1,
                             const Float3Key& _k2, const Float3Key& _k3,
                             math::SoaFloat3* _soa_float3) {
  _soa_float3->x = math::HalfToFloat(math::simd_int4::Load(
      _k0.value[0], _k1.value[0], _k2.value[0], _k3.value[0]));
  _soa_float3->y = math::HalfToFloat(math::simd_int4::Load(
      _k0.value[1], _k1.value[1], _k2.value[1], _k3.value[1]));
  _soa_float3->z = math::HalfToFloat(math::simd_int4::Load(
      _k0.value[2], _k1.value[2], _k2.value[2], _k3.value[2]));
}

// Defines a mapping table that defines components assignation in the output
// quaternion.
static constexpr int kCpntMapping[4][4] = {
    {0, 0, 1, 2}, {0, 0, 1, 2}, {0, 1, 0, 2}, {0, 1, 2, 0}};

inline void DecompressQuaternion(const QuaternionKey& _k0,
                                 const QuaternionKey& _k1,
                                 const QuaternionKey& _k2,
                                 const QuaternionKey& _k3,
                                 math::SoaQuaternion* _quaternion) {
  // Selects proper mapping for each key.
  const int* m0 = kCpntMapping[_k0.largest];
  const int* m1 = kCpntMapping[_k1.largest];
  const int* m2 = kCpntMapping[_k2.largest];
  const int* m3 = kCpntMapping[_k3.largest];

  // Prepares an array of input values, according to the mapping required to
  // restore quaternion largest component.
  alignas(16) int cmp_keys[4][4] = {
      {_k0.value[m0[0]], _k1.value[m1[0]], _k2.value[m2[0]], _k3.value[m3[0]]},
      {_k0.value[m0[1]], _k1.value[m1[1]], _k2.value[m2[1]], _k3.value[m3[1]]},
      {_k0.value[m0[2]], _k1.value[m1[2]], _k2.value[m2[2]], _k3.value[m3[2]]},
      {_k0.value[m0[3]], _k1.value[m1[3]], _k2.value[m2[3]], _k3.value[m3[3]]},
  };

  // Resets largest component to 0. Overwritting here avoids 16 branchings
  // above.
  cmp_keys[_k0.largest][0] = 0;
  cmp_keys[_k1.largest][1] = 0;
  cmp_keys[_k2.largest][2] = 0;
  cmp_keys[_k3.largest][3] = 0;

  // Rebuilds quaternion from quantized values.
  const math::SimdFloat4 kInt2Float =
      math::simd_float4::Load1(1.f / (32767.f * math::kSqrt2));
  math::SimdFloat4 cpnt[4] = {
      kInt2Float *
          math::simd_float4::FromInt(math::simd_int4::LoadPtr(cmp_keys[0])),
      kInt2Float *
          math::simd_float4::FromInt(math::simd_int4::LoadPtr(cmp_keys[1])),
      kInt2Float *
          math::simd_float4::FromInt(math::simd_int4::LoadPtr(cmp_keys[2])),
      kInt2Float *
          math::simd_float4::FromInt(math::simd_int4::LoadPtr(cmp_keys[3])),
  };

  // Get back length of 4th component. Favors performance over accuracy by using
  // x * RSqrtEst(x) instead of Sqrt(x).
  // ww0 cannot be 0 because we 're recomputing the largest component.
  const math::SimdFloat4 dot = cpnt[0] * cpnt[0] + cpnt[1] * cpnt[1] +
                               cpnt[2] * cpnt[2] + cpnt[3] * cpnt[3];
  const math::SimdFloat4 ww0 = math::Max(math::simd_float4::Load1(1e-16f),
                                         math::simd_float4::one() - dot);
  const math::SimdFloat4 w0 = ww0 * math::RSqrtEst(ww0);
  // Re-applies 4th component' s sign.
  const math::SimdInt4 sign = math::ShiftL(
      math::simd_int4::Load(_k0.sign, _k1.sign, _k2.sign, _k3.sign), 31);
  const math::SimdFloat4 restored = math::Or(w0, sign);

  // Re-injects the largest component inside the SoA structure.
  cpnt[_k0.largest] = math::Or(
      cpnt[_k0.largest], math::And(restored, math::simd_int4::mask_f000()));
  cpnt[_k1.largest] = math::Or(
      cpnt[_k1.largest], math::And(restored, math::simd_int4::mask_0f00()));
  cpnt[_k2.largest] = math::Or(
      cpnt[_k2.largest], math::And(restored, math::simd_int4::mask_00f0()));
  cpnt[_k3.largest] = math::Or(
      cpnt[_k3.largest], math::And(restored, math::simd_int4::mask_000f()));

  // Stores result.
  _quaternion->x = cpnt[0];
  _quaternion->y = cpnt[1];
  _quaternion->z = cpnt[2];
  _quaternion->w = cpnt[3];
}

void Interpolates(float _anim_ratio, int _num_soa_tracks,
                  const internal::InterpSoaFloat3* _translations,
                  const internal::InterpSoaQuaternion* _rotations,
                  const internal::InterpSoaFloat3* _scales,
                  math::SoaTransform* _output) {
  const math::SimdFloat4 anim_ratio = math::simd_float4::Load1(_anim_ratio);
  for (int i = 0; i < _num_soa_tracks; ++i) {
    // Prepares interpolation coefficients.
    const math::SimdFloat4 interp_t_ratio =
        (anim_ratio - _translations[i].ratio[0]) *
        math::RcpEst(_translations[i].ratio[1] - _translations[i].ratio[0]);
    const math::SimdFloat4 interp_r_ratio =
        (anim_ratio - _rotations[i].ratio[0]) *
        math::RcpEst(_rotations[i].ratio[1] - _rotations[i].ratio[0]);
    const math::SimdFloat4 interp_s_ratio =
        (anim_ratio - _scales[i].ratio[0]) *
        math::RcpEst(_scales[i].ratio[1] - _scales[i].ratio[0]);

    // Processes interpolations.
    // The lerp of the rotation uses the shortest path, because opposed
    // quaternions were negated during animation build stage (AnimationBuilder).
    _output[i].translation = Lerp(_translations[i].value[0],
                                  _translations[i].value[1], interp_t_ratio);
    _output[i].rotation = NLerpEst(_rotations[i].value[0],
                                   _rotations[i].value[1], interp_r_ratio);
    _output[i].scale =
        Lerp(_scales[i].value[0], _scales[i].value[1], interp_s_ratio);
  }
}
}  // namespace

SamplingJob::SamplingJob() : ratio(0.f), animation(nullptr), context(nullptr) {}

bool SamplingJob::Run() const {
  if (!Validate()) {
    return false;
  }

  const int num_soa_tracks = animation->num_soa_tracks();
  if (num_soa_tracks == 0) {  // Early out if animation contains no joint.
    return true;
  }

  // Clamps ratio in range [0,duration].
  const float anim_ratio = math::Clamp(0.f, ratio, 1.f);

  // Step the context to this potentially new animation and ratio.
  assert(context->max_soa_tracks() >= num_soa_tracks);
  context->Step(*animation, anim_ratio);

  // Fetch key frames from the animation to the context a r = anim_ratio.
  // Then updates outdated soa hot values.
  UpdateCacheCursor(anim_ratio, num_soa_tracks, animation->translations(),
                    &context->translation_cursor_, context->translation_cache_,
                    context->outdated_translations_);
  UpdateInterpKeyframes(num_soa_tracks, animation->translations(),
                        context->translation_cache_,
                        context->outdated_translations_, context->soa_translations_,
                        &DecompressFloat3);

  UpdateCacheCursor(anim_ratio, num_soa_tracks, animation->rotations(),
                    &context->rotation_cursor_, context->rotation_cache_,
                    context->outdated_rotations_);
  UpdateInterpKeyframes(num_soa_tracks, animation->rotations(),
                        context->rotation_cache_, context->outdated_rotations_,
                        context->soa_rotations_, &DecompressQuaternion);

  UpdateCacheCursor(anim_ratio, num_soa_tracks, animation->scales(),
                    &context->scale_cursor_, context->scale_cache_,
                    context->outdated_scales_);
  UpdateInterpKeyframes(num_soa_tracks, animation->scales(),
                        context->scale_cache_, context->outdated_scales_,
                        context->soa_scales_, &DecompressFloat3);

  // Interpolates soa hot data.
  Interpolates(anim_ratio, num_soa_tracks, context->soa_translations_,
               context->soa_rotations_, context->soa_scales_, output.begin());

  return true;
}

SamplingJob::Context::Context()
    : max_soa_tracks_(0),
      soa_translations_(
          nullptr) {  // soa_translations_ is the allocation pointer.
  Invalidate();
}

SamplingJob::Context::Context(int _max_tracks)
    : max_soa_tracks_(0),
      soa_translations_(
          nullptr) {  // soa_translations_ is the allocation pointer.
  Resize(_max_tracks);
}

SamplingJob::Context::~Context() {
  // Deallocates everything at once.
  memory::default_allocator()->Deallocate(soa_translations_);
}

void SamplingJob::Context::Resize(int _max_tracks) {
  using internal::InterpSoaFloat3;
  using internal::InterpSoaQuaternion;

  // Reset existing data.
  Invalidate();
  memory::default_allocator()->Deallocate(soa_translations_);

  // Updates maximum supported soa tracks.
  max_soa_tracks_ = (_max_tracks + 3) / 4;

  // Allocate all context data at once in a single allocation.
  // Alignment is guaranteed because memory is dispatch from the highest
  // alignment requirement (Soa data: SimdFloat4) to the lowest (outdated
  // flag: unsigned char).

  // Computes allocation size.
  const size_t max_tracks = max_soa_tracks_ * 4;
  const size_t num_outdated = (max_soa_tracks_ + 7) / 8;
  const size_t size =
      sizeof(InterpSoaFloat3) * max_soa_tracks_ +
      sizeof(InterpSoaQuaternion) * max_soa_tracks_ +
      sizeof(InterpSoaFloat3) * max_soa_tracks_ +
      sizeof(int) * max_tracks * 2 * 3 +  // 2 keys * (trans + rot + scale).
      sizeof(uint8_t) * 3 * num_outdated;

  // Allocates all at once.
  memory::Allocator* allocator = memory::default_allocator();
  char* alloc_begin = reinterpret_cast<char*>(
      allocator->Allocate(size, alignof(InterpSoaFloat3)));
  char* alloc_cursor = alloc_begin;

  // Distributes buffer memory while ensuring proper alignment (serves larger
  // alignment values first).
  static_assert(alignof(InterpSoaFloat3) >= alignof(InterpSoaQuaternion) &&
                    alignof(InterpSoaQuaternion) >= alignof(InterpSoaFloat3) &&
                    alignof(InterpSoaFloat3) >= alignof(int) &&
                    alignof(int) >= alignof(uint8_t),
                "Must serve larger alignment values first)");

  soa_translations_ = reinterpret_cast<InterpSoaFloat3*>(alloc_cursor);
  assert(IsAligned(soa_translations_, alignof(InterpSoaFloat3)));
  alloc_cursor += sizeof(InterpSoaFloat3) * max_soa_tracks_;
  soa_rotations_ = reinterpret_cast<InterpSoaQuaternion*>(alloc_cursor);
  assert(IsAligned(soa_rotations_, alignof(InterpSoaQuaternion)));
  alloc_cursor += sizeof(InterpSoaQuaternion) * max_soa_tracks_;
  soa_scales_ = reinterpret_cast<InterpSoaFloat3*>(alloc_cursor);
  assert(IsAligned(soa_scales_, alignof(InterpSoaFloat3)));
  alloc_cursor += sizeof(InterpSoaFloat3) * max_soa_tracks_;

  translation_cache_ = reinterpret_cast<int*>(alloc_cursor);
  assert(IsAligned(translation_cache_, alignof(int)));
  alloc_cursor += sizeof(int) * max_tracks * 2;
  rotation_cache_ = reinterpret_cast<int*>(alloc_cursor);
  alloc_cursor += sizeof(int) * max_tracks * 2;
  scale_cache_ = reinterpret_cast<int*>(alloc_cursor);
  alloc_cursor += sizeof(int) * max_tracks * 2;

  outdated_translations_ = reinterpret_cast<uint8_t*>(alloc_cursor);
  assert(IsAligned(outdated_translations_, alignof(uint8_t)));
  alloc_cursor += sizeof(uint8_t) * num_outdated;
  outdated_rotations_ = reinterpret_cast<uint8_t*>(alloc_cursor);
  alloc_cursor += sizeof(uint8_t) * num_outdated;
  outdated_scales_ = reinterpret_cast<uint8_t*>(alloc_cursor);
  alloc_cursor += sizeof(uint8_t) * num_outdated;

  assert(alloc_cursor == alloc_begin + size);
}

void SamplingJob::Context::Step(const Animation& _animation, float _ratio) {
  // The cache is invalidated if animation has changed...
  const bool changed = animation_ != &_animation;

  // ... or if it is rewinding.
  // Note that if current state is below kRestartOverhead treshold, then better
  // rewind than restart as it won't trash the cached keyframes.
  const float kRestartOverhead =
      .05f;  // Restart is worth 5% of seek time (just a guess).
  const bool restart = ratio_ > kRestartOverhead && ratio_ - _ratio > _ratio;

  if (changed || restart) {
    animation_ = &_animation;
    translation_cursor_ = 0;
    rotation_cursor_ = 0;
    scale_cursor_ = 0;
  }
  ratio_ = _ratio;
}

void SamplingJob::Context::Invalidate() {
  animation_ = nullptr;
  ratio_ = 0.f;
  translation_cursor_ = 0;
  rotation_cursor_ = 0;
  scale_cursor_ = 0;
}
}  // namespace animation
}  // namespace ozz
