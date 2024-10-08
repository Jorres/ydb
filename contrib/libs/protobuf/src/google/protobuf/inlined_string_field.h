// Protocol Buffers - Google's data interchange format
// Copyright 2008 Google Inc.  All rights reserved.
// https://developers.google.com/protocol-buffers/
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef GOOGLE_PROTOBUF_INLINED_STRING_FIELD_H__
#define GOOGLE_PROTOBUF_INLINED_STRING_FIELD_H__

#include <string>
#include <utility>

#include "google/protobuf/port.h"
#include "y_absl/log/absl_check.h"
#include "y_absl/strings/string_view.h"
#include "google/protobuf/arenastring.h"
#include "google/protobuf/message_lite.h"

// Must be included last.
#include "google/protobuf/port_def.inc"

#ifdef SWIG
#error "You cannot SWIG proto headers"
#endif

namespace google {
namespace protobuf {

class Arena;

namespace internal {

// InlinedStringField wraps a TProtoStringType instance and exposes an API similar to
// ArenaStringPtr's wrapping of a TProtoStringType* instance.
//
// default_value parameters are taken for consistency with ArenaStringPtr, but
// are not used for most methods. With inlining, these should be removed from
// the generated binary.
//
// InlinedStringField has a donating mechanism that allows string buffer
// allocated on arena. A string is donated means both the string container and
// the data buffer are on arena. The donating mechanism here is similar to the
// one in ArenaStringPtr with some differences:
//
// When an InlinedStringField is constructed, the donating state is true. This
// is because the string container is directly stored in the message on the
// arena:
//
//   Construction: donated=true
//   Arena:
//   +-----------------------+
//   |Message foo:           |
//   | +-------------------+ |
//   | |InlinedStringField:| |
//   | | +-----+           | |
//   | | | | | |           | |
//   | | +-----+           | |
//   | +-------------------+ |
//   +-----------------------+
//
// When lvalue Set is called, the donating state is still true. String data will
// be allocated on the arena:
//
//   Lvalue Set: donated=true
//   Arena:
//   +-----------------------+
//   |Message foo:           |
//   | +-------------------+ |
//   | |InlinedStringField:| |
//   | | +-----+           | |
//   | | | | | |           | |
//   | | +|----+           | |
//   | +--|----------------+ |
//   |    V                  |
//   |  +----------------+   |
//   |  |'f','o','o',... |   |
//   |  +----------------+   |
//   +-----------------------+
//
// Some operations will undonate a donated string, including: Mutable,
// SetAllocated, Rvalue Set, and Swap with a non-donated string.
//
// For more details of the donating states transitions, go/pd-inlined-string.
class PROTOBUF_EXPORT InlinedStringField {
 public:
  InlinedStringField() { Init(); }
  inline void Init() { new (get_mutable()) TProtoStringType(); }
  // Add the dummy parameter just to make InlinedStringField(nullptr)
  // unambiguous.
  constexpr InlinedStringField(
      const ExplicitlyConstructed<TProtoStringType>* /*default_value*/,
      bool /*dummy*/)
      : value_{} {}
  explicit InlinedStringField(const TProtoStringType& default_value);
  explicit InlinedStringField(Arena* arena);
  ~InlinedStringField() { Destruct(); }

  // Lvalue Set. To save space, we pack the donating states of multiple
  // InlinedStringFields into an arc_ui32 `donating_states`. The `mask`
  // indicates the position of the bit for this InlinedStringField. `donated` is
  // whether this field is donated.
  //
  // The caller should guarantee that:
  //
  //   `donated == ((donating_states & ~mask) != 0)`
  //
  // This method never changes the `donating_states`.
  void Set(y_absl::string_view value, Arena* arena, bool donated,
           arc_ui32* donating_states, arc_ui32 mask, MessageLite* msg);

  // Rvalue Set. If this field is donated, this method will undonate this field
  // by mutating the `donating_states` according to `mask`.
  void Set(TProtoStringType&& value, Arena* arena, bool donated,
           arc_ui32* donating_states, arc_ui32 mask, MessageLite* msg);

  void Set(const char* str, ::google::protobuf::Arena* arena, bool donated,
           arc_ui32* donating_states, arc_ui32 mask, MessageLite* msg);

  void Set(const char* str, size_t size, ::google::protobuf::Arena* arena, bool donated,
           arc_ui32* donating_states, arc_ui32 mask, MessageLite* msg);

  template <typename RefWrappedType>
  void Set(std::reference_wrapper<RefWrappedType> const_string_ref,
           ::google::protobuf::Arena* arena, bool donated, arc_ui32* donating_states,
           arc_ui32 mask, MessageLite* msg);

  void SetBytes(y_absl::string_view value, Arena* arena, bool donated,
                arc_ui32* donating_states, arc_ui32 mask, MessageLite* msg);

  void SetBytes(TProtoStringType&& value, Arena* arena, bool donated,
                arc_ui32* donating_states, arc_ui32 mask, MessageLite* msg);

  void SetBytes(const char* str, ::google::protobuf::Arena* arena, bool donated,
                arc_ui32* donating_states, arc_ui32 mask, MessageLite* msg);

  void SetBytes(const void* p, size_t size, ::google::protobuf::Arena* arena,
                bool donated, arc_ui32* donating_states, arc_ui32 mask,
                MessageLite* msg);

  template <typename RefWrappedType>
  void SetBytes(std::reference_wrapper<RefWrappedType> const_string_ref,
                ::google::protobuf::Arena* arena, bool donated, arc_ui32* donating_states,
                arc_ui32 mask, MessageLite* msg);

  PROTOBUF_NDEBUG_INLINE void SetNoArena(y_absl::string_view value);
  PROTOBUF_NDEBUG_INLINE void SetNoArena(TProtoStringType&& value);

  // Basic accessors.
  PROTOBUF_NDEBUG_INLINE const TProtoStringType& Get() const { return GetNoArena(); }
  PROTOBUF_NDEBUG_INLINE const TProtoStringType& GetNoArena() const;

  // Mutable returns a TProtoStringType* instance that is heap-allocated. If this
  // field is donated, this method undonates this field by mutating the
  // `donating_states` according to `mask`, and copies the content of the
  // original string to the returning string.
  TProtoStringType* Mutable(Arena* arena, bool donated, arc_ui32* donating_states,
                       arc_ui32 mask, MessageLite* msg);
  TProtoStringType* Mutable(const LazyString& default_value, Arena* arena,
                       bool donated, arc_ui32* donating_states, arc_ui32 mask,
                       MessageLite* msg);

  // Mutable(nullptr_t) is an overload to explicitly support Mutable(nullptr)
  // calls used by the internal parser logic. This provides API equivalence with
  // ArenaStringPtr, while still protecting against calls with arena pointers.
  TProtoStringType* Mutable(std::nullptr_t);
  TProtoStringType* MutableNoCopy(std::nullptr_t);

  // Takes a TProtoStringType that is heap-allocated, and takes ownership. The
  // TProtoStringType's destructor is registered with the arena. Used to implement
  // set_allocated_<field> in generated classes.
  //
  // If this field is donated, this method undonates this field by mutating the
  // `donating_states` according to `mask`.
  void SetAllocated(const TProtoStringType* default_value, TProtoStringType* value,
                    Arena* arena, bool donated, arc_ui32* donating_states,
                    arc_ui32 mask, MessageLite* msg);

  void SetAllocatedNoArena(const TProtoStringType* default_value,
                           TProtoStringType* value);

  // Release returns a TProtoStringType* instance that is heap-allocated and is not
  // Own()'d by any arena. If the field is not set, this returns nullptr. The
  // caller retains ownership. Clears this field back to nullptr state. Used to
  // implement release_<field>() methods on generated classes.
  PROTOBUF_NODISCARD TProtoStringType* Release(Arena* arena, bool donated);
  PROTOBUF_NODISCARD TProtoStringType* Release();

  // --------------------------------------------------------
  // Below functions will be removed in subsequent code change
  // --------------------------------------------------------
#ifdef DEPRECATED_METHODS_TO_BE_DELETED
  PROTOBUF_NODISCARD TProtoStringType* Release(const TProtoStringType*, Arena* arena,
                                          bool donated) {
    return Release(arena, donated);
  }

  PROTOBUF_NODISCARD TProtoStringType* ReleaseNonDefault(const TProtoStringType*,
                                                    Arena* arena) {
    return Release();
  }

  TProtoStringType* ReleaseNonDefaultNoArena(const TProtoStringType* default_value) {
    return Release();
  }

  void Set(const TProtoStringType*, y_absl::string_view value, Arena* arena,
           bool donated, arc_ui32* donating_states, arc_ui32 mask,
           MessageLite* msg) {
    Set(value, arena, donated, donating_states, mask, msg);
  }

  void Set(const TProtoStringType*, TProtoStringType&& value, Arena* arena, bool donated,
           arc_ui32* donating_states, arc_ui32 mask, MessageLite* msg) {
    Set(std::move(value), arena, donated, donating_states, mask, msg);
  }


  template <typename FirstParam>
  void Set(FirstParam, const char* str, ::google::protobuf::Arena* arena, bool donated,
           arc_ui32* donating_states, arc_ui32 mask, MessageLite* msg) {
    Set(str, arena, donated, donating_states, mask, msg);
  }

  template <typename FirstParam>
  void Set(FirstParam p1, const char* str, size_t size, ::google::protobuf::Arena* arena,
           bool donated, arc_ui32* donating_states, arc_ui32 mask,
           MessageLite* msg) {
    Set(str, size, arena, donated, donating_states, mask, msg);
  }

  template <typename FirstParam, typename RefWrappedType>
  void Set(FirstParam p1,
           std::reference_wrapper<RefWrappedType> const_string_ref,
           ::google::protobuf::Arena* arena, bool donated, arc_ui32* donating_states,
           arc_ui32 mask, MessageLite* msg) {
    Set(const_string_ref, arena, donated, donating_states, mask, msg);
  }

  void SetBytes(const TProtoStringType*, y_absl::string_view value, Arena* arena,
                bool donated, arc_ui32* donating_states, arc_ui32 mask,
                MessageLite* msg) {
    Set(value, arena, donated, donating_states, mask, msg);
  }


  void SetBytes(const TProtoStringType*, TProtoStringType&& value, Arena* arena,
                bool donated, arc_ui32* donating_states, arc_ui32 mask,
                MessageLite* msg) {
    Set(std::move(value), arena, donated, donating_states, mask, msg);
  }

  template <typename FirstParam>
  void SetBytes(FirstParam p1, const char* str, ::google::protobuf::Arena* arena,
                bool donated, arc_ui32* donating_states, arc_ui32 mask,
                MessageLite* msg) {
    SetBytes(str, arena, donated, donating_states, mask, msg);
  }

  template <typename FirstParam>
  void SetBytes(FirstParam p1, const void* p, size_t size,
                ::google::protobuf::Arena* arena, bool donated, arc_ui32* donating_states,
                arc_ui32 mask, MessageLite* msg) {
    SetBytes(p, size, arena, donated, donating_states, mask, msg);
  }

  template <typename FirstParam, typename RefWrappedType>
  void SetBytes(FirstParam p1,
                std::reference_wrapper<RefWrappedType> const_string_ref,
                ::google::protobuf::Arena* arena, bool donated, arc_ui32* donating_states,
                arc_ui32 mask, MessageLite* msg) {
    SetBytes(const_string_ref.get(), arena, donated, donating_states, mask,
             msg);
  }

  void SetNoArena(const TProtoStringType*, y_absl::string_view value) {
    SetNoArena(value);
  }
  void SetNoArena(const TProtoStringType*, TProtoStringType&& value) {
    SetNoArena(std::move(value));
  }

  TProtoStringType* Mutable(ArenaStringPtr::EmptyDefault, Arena* arena, bool donated,
                       arc_ui32* donating_states, arc_ui32 mask,
                       MessageLite* msg) {
    return Mutable(arena, donated, donating_states, mask, msg);
  }

  PROTOBUF_NDEBUG_INLINE TProtoStringType* MutableNoArenaNoDefault(
      const TProtoStringType* /*default_value*/) {
    return MutableNoCopy(nullptr);
  }

#endif  // DEPRECATED_METHODS_TO_BE_DELETED

  // Arena-safety semantics: this is guarded by the logic in
  // Swap()/UnsafeArenaSwap() at the message level, so this method is
  // 'unsafe' if called directly.
  inline PROTOBUF_NDEBUG_INLINE static void InternalSwap(
      InlinedStringField* lhs, Arena* lhs_arena, bool lhs_arena_dtor_registered,
      MessageLite* lhs_msg,  //
      InlinedStringField* rhs, Arena* rhs_arena, bool rhs_arena_dtor_registered,
      MessageLite* rhs_msg);

  // Frees storage (if not on an arena).
  PROTOBUF_NDEBUG_INLINE void Destroy(const TProtoStringType* default_value,
                                      Arena* arena) {
    if (arena == nullptr) {
      DestroyNoArena(default_value);
    }
  }
  PROTOBUF_NDEBUG_INLINE void DestroyNoArena(const TProtoStringType* default_value);

  // Clears content, but keeps allocated TProtoStringType, to avoid the overhead of
  // heap operations. After this returns, the content (as seen by the user) will
  // always be the empty TProtoStringType.
  PROTOBUF_NDEBUG_INLINE void ClearToEmpty() { ClearNonDefaultToEmpty(); }
  PROTOBUF_NDEBUG_INLINE void ClearNonDefaultToEmpty() {
    get_mutable()->clear();
  }

  // Clears content, but keeps allocated TProtoStringType if arena != nullptr, to
  // avoid the overhead of heap operations. After this returns, the content (as
  // seen by the user) will always be equal to |default_value|.
  void ClearToDefault(const LazyString& default_value, Arena* arena,
                      bool donated);

  // Generated code / reflection only! Returns a mutable pointer to the string.
  PROTOBUF_NDEBUG_INLINE TProtoStringType* UnsafeMutablePointer();

  // InlinedStringField doesn't have things like the `default_value` pointer in
  // ArenaStringPtr.
  static constexpr bool IsDefault() { return false; }
  static constexpr bool IsDefault(const TProtoStringType*) { return false; }

 private:
  void Destruct() { get_mutable()->~TBasicString(); }

  PROTOBUF_NDEBUG_INLINE TProtoStringType* get_mutable();
  PROTOBUF_NDEBUG_INLINE const TProtoStringType* get_const() const;

  alignas(TProtoStringType) char value_[sizeof(TProtoStringType)];

  TProtoStringType* MutableSlow(::google::protobuf::Arena* arena, bool donated,
                           arc_ui32* donating_states, arc_ui32 mask,
                           MessageLite* msg);


  // When constructed in an Arena, we want our destructor to be skipped.
  friend class ::google::protobuf::Arena;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
};

inline TProtoStringType* InlinedStringField::get_mutable() {
  return reinterpret_cast<TProtoStringType*>(&value_);
}

inline const TProtoStringType* InlinedStringField::get_const() const {
  return reinterpret_cast<const TProtoStringType*>(&value_);
}

inline InlinedStringField::InlinedStringField(
    const TProtoStringType& default_value) {
  new (get_mutable()) TProtoStringType(default_value);
}


inline InlinedStringField::InlinedStringField(Arena* /*arena*/) { Init(); }

inline const TProtoStringType& InlinedStringField::GetNoArena() const {
  return *get_const();
}

inline void InlinedStringField::SetAllocatedNoArena(
    const TProtoStringType* /*default_value*/, TProtoStringType* value) {
  if (value == nullptr) {
    // Currently, inlined string field can't have non empty default.
    get_mutable()->clear();
  } else {
    get_mutable()->assign(std::move(*value));
    delete value;
  }
}

inline void InlinedStringField::DestroyNoArena(const TProtoStringType*) {
  // This is invoked from the generated message's ArenaDtor, which is used to
  // clean up objects not allocated on the Arena.
  this->~InlinedStringField();
}

inline void InlinedStringField::SetNoArena(y_absl::string_view value) {
  get_mutable()->assign(value.data(), value.length());
}

inline void InlinedStringField::SetNoArena(TProtoStringType&& value) {
  get_mutable()->assign(std::move(value));
}

// Caller should make sure rhs_arena allocated rhs, and lhs_arena allocated lhs.
inline PROTOBUF_NDEBUG_INLINE void InlinedStringField::InternalSwap(
    InlinedStringField* lhs, Arena* lhs_arena, bool lhs_arena_dtor_registered,
    MessageLite* lhs_msg,  //
    InlinedStringField* rhs, Arena* rhs_arena, bool rhs_arena_dtor_registered,
    MessageLite* rhs_msg) {
#ifdef GOOGLE_PROTOBUF_INTERNAL_DONATE_STEAL_INLINE
  lhs->get_mutable()->swap(*rhs->get_mutable());
  if (!lhs_arena_dtor_registered && rhs_arena_dtor_registered) {
    lhs_msg->OnDemandRegisterArenaDtor(lhs_arena);
  } else if (lhs_arena_dtor_registered && !rhs_arena_dtor_registered) {
    rhs_msg->OnDemandRegisterArenaDtor(rhs_arena);
  }
#else
  (void)lhs_arena;
  (void)rhs_arena;
  (void)lhs_arena_dtor_registered;
  (void)rhs_arena_dtor_registered;
  (void)lhs_msg;
  (void)rhs_msg;
  lhs->get_mutable()->swap(*rhs->get_mutable());
#endif
}

inline void InlinedStringField::Set(y_absl::string_view value, Arena* arena,
                                    bool donated, arc_ui32* /*donating_states*/,
                                    arc_ui32 /*mask*/, MessageLite* /*msg*/) {
  (void)arena;
  (void)donated;
  SetNoArena(value);
}

inline void InlinedStringField::Set(const char* str, ::google::protobuf::Arena* arena,
                                    bool donated, arc_ui32* donating_states,
                                    arc_ui32 mask, MessageLite* msg) {
  Set(y_absl::string_view(str), arena, donated, donating_states, mask, msg);
}

inline void InlinedStringField::Set(const char* str, size_t size,
                                    ::google::protobuf::Arena* arena, bool donated,
                                    arc_ui32* donating_states, arc_ui32 mask,
                                    MessageLite* msg) {
  Set(y_absl::string_view{str, size}, arena, donated, donating_states, mask, msg);
}

inline void InlinedStringField::SetBytes(y_absl::string_view value, Arena* arena,
                                         bool donated,
                                         arc_ui32* donating_states,
                                         arc_ui32 mask, MessageLite* msg) {
  Set(value, arena, donated, donating_states, mask, msg);
}

inline void InlinedStringField::SetBytes(TProtoStringType&& value, Arena* arena,
                                         bool donated,
                                         arc_ui32* donating_states,
                                         arc_ui32 mask, MessageLite* msg) {
  Set(std::move(value), arena, donated, donating_states, mask, msg);
}

inline void InlinedStringField::SetBytes(const char* str,
                                         ::google::protobuf::Arena* arena, bool donated,
                                         arc_ui32* donating_states,
                                         arc_ui32 mask, MessageLite* msg) {
  Set(str, arena, donated, donating_states, mask, msg);
}

inline void InlinedStringField::SetBytes(const void* p, size_t size,
                                         ::google::protobuf::Arena* arena, bool donated,
                                         arc_ui32* donating_states,
                                         arc_ui32 mask, MessageLite* msg) {
  Set(static_cast<const char*>(p), size, arena, donated, donating_states, mask,
      msg);
}

template <typename RefWrappedType>
inline void InlinedStringField::Set(
    std::reference_wrapper<RefWrappedType> const_string_ref,
    ::google::protobuf::Arena* arena, bool donated, arc_ui32* donating_states,
    arc_ui32 mask, MessageLite* msg) {
  Set(const_string_ref.get(), arena, donated, donating_states, mask, msg);
}

template <typename RefWrappedType>
inline void InlinedStringField::SetBytes(
    std::reference_wrapper<RefWrappedType> const_string_ref,
    ::google::protobuf::Arena* arena, bool donated, arc_ui32* donating_states,
    arc_ui32 mask, MessageLite* msg) {
  Set(const_string_ref.get(), arena, donated, donating_states, mask, msg);
}

inline TProtoStringType* InlinedStringField::UnsafeMutablePointer() {
  return get_mutable();
}

inline TProtoStringType* InlinedStringField::Mutable(std::nullptr_t) {
  return get_mutable();
}

inline TProtoStringType* InlinedStringField::MutableNoCopy(std::nullptr_t) {
  return get_mutable();
}

}  // namespace internal
}  // namespace protobuf
}  // namespace google

#include "google/protobuf/port_undef.inc"

#endif  // GOOGLE_PROTOBUF_INLINED_STRING_FIELD_H__
