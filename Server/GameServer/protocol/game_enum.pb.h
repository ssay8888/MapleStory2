// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: game_enum.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_game_5fenum_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_game_5fenum_2eproto

#include <limits>
#include <string>

#include <google/protobuf/port_def.inc>
#if PROTOBUF_VERSION < 3017000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers. Please update
#error your headers.
#endif
#if 3017001 < PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers. Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/port_undef.inc>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_table_driven.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/metadata_lite.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/generated_enum_reflection.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
#define PROTOBUF_INTERNAL_EXPORT_game_5fenum_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_game_5fenum_2eproto {
  static const ::PROTOBUF_NAMESPACE_ID::internal::ParseTableField entries[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::AuxiliaryParseTableField aux[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::ParseTable schema[1]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::FieldMetadata field_metadata[];
  static const ::PROTOBUF_NAMESPACE_ID::internal::SerializationTable serialization_table[];
  static const ::PROTOBUF_NAMESPACE_ID::uint32 offsets[];
};
extern const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_game_5fenum_2eproto;
PROTOBUF_NAMESPACE_OPEN
PROTOBUF_NAMESPACE_CLOSE
namespace Protocol {

enum kInventoryType : int {
  kInventoryEquipped = 0,
  kInventoryEqp = 1,
  kInventoryEtc = 2,
  kInventoryEnd = 3,
  kInventoryType_INT_MIN_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::min(),
  kInventoryType_INT_MAX_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::max()
};
bool kInventoryType_IsValid(int value);
constexpr kInventoryType kInventoryType_MIN = kInventoryEquipped;
constexpr kInventoryType kInventoryType_MAX = kInventoryEnd;
constexpr int kInventoryType_ARRAYSIZE = kInventoryType_MAX + 1;

const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* kInventoryType_descriptor();
template<typename T>
inline const std::string& kInventoryType_Name(T enum_t_value) {
  static_assert(::std::is_same<T, kInventoryType>::value ||
    ::std::is_integral<T>::value,
    "Incorrect type passed to function kInventoryType_Name.");
  return ::PROTOBUF_NAMESPACE_ID::internal::NameOfEnum(
    kInventoryType_descriptor(), enum_t_value);
}
inline bool kInventoryType_Parse(
    ::PROTOBUF_NAMESPACE_ID::ConstStringParam name, kInventoryType* value) {
  return ::PROTOBUF_NAMESPACE_ID::internal::ParseNamedEnum<kInventoryType>(
    kInventoryType_descriptor(), name, value);
}
enum kLoadState : int {
  kLoadSuccess = 0,
  kLoadFailed = 1,
  kLoadState_INT_MIN_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::min(),
  kLoadState_INT_MAX_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::max()
};
bool kLoadState_IsValid(int value);
constexpr kLoadState kLoadState_MIN = kLoadSuccess;
constexpr kLoadState kLoadState_MAX = kLoadFailed;
constexpr int kLoadState_ARRAYSIZE = kLoadState_MAX + 1;

const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* kLoadState_descriptor();
template<typename T>
inline const std::string& kLoadState_Name(T enum_t_value) {
  static_assert(::std::is_same<T, kLoadState>::value ||
    ::std::is_integral<T>::value,
    "Incorrect type passed to function kLoadState_Name.");
  return ::PROTOBUF_NAMESPACE_ID::internal::NameOfEnum(
    kLoadState_descriptor(), enum_t_value);
}
inline bool kLoadState_Parse(
    ::PROTOBUF_NAMESPACE_ID::ConstStringParam name, kLoadState* value) {
  return ::PROTOBUF_NAMESPACE_ID::internal::ParseNamedEnum<kLoadState>(
    kLoadState_descriptor(), name, value);
}
enum kMonsterState : int {
  kIdleA = 0,
  kWalkA = 1,
  kBoreA = 2,
  kRegenA = 3,
  kRunA = 4,
  kDeadA = 5,
  kDeadB = 6,
  kAttack1A = 7,
  kAttack2A = 8,
  kAttack3A = 9,
  kAttack1B = 10,
  kAttack2B = 11,
  kAttack3B = 12,
  kAttack1C = 13,
  kAttack2C = 14,
  kAttack3C = 15,
  kDamgA = 16,
  kDamgB = 17,
  kDamgC = 18,
  kMonsterState_INT_MIN_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::min(),
  kMonsterState_INT_MAX_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::max()
};
bool kMonsterState_IsValid(int value);
constexpr kMonsterState kMonsterState_MIN = kIdleA;
constexpr kMonsterState kMonsterState_MAX = kDamgC;
constexpr int kMonsterState_ARRAYSIZE = kMonsterState_MAX + 1;

const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* kMonsterState_descriptor();
template<typename T>
inline const std::string& kMonsterState_Name(T enum_t_value) {
  static_assert(::std::is_same<T, kMonsterState>::value ||
    ::std::is_integral<T>::value,
    "Incorrect type passed to function kMonsterState_Name.");
  return ::PROTOBUF_NAMESPACE_ID::internal::NameOfEnum(
    kMonsterState_descriptor(), enum_t_value);
}
inline bool kMonsterState_Parse(
    ::PROTOBUF_NAMESPACE_ID::ConstStringParam name, kMonsterState* value) {
  return ::PROTOBUF_NAMESPACE_ID::internal::ParseNamedEnum<kMonsterState>(
    kMonsterState_descriptor(), name, value);
}
enum kPlayerStatType : int {
  kHp = 0,
  kMp = 1,
  kExp = 2,
  kPlayerStatType_INT_MIN_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::min(),
  kPlayerStatType_INT_MAX_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::max()
};
bool kPlayerStatType_IsValid(int value);
constexpr kPlayerStatType kPlayerStatType_MIN = kHp;
constexpr kPlayerStatType kPlayerStatType_MAX = kExp;
constexpr int kPlayerStatType_ARRAYSIZE = kPlayerStatType_MAX + 1;

const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* kPlayerStatType_descriptor();
template<typename T>
inline const std::string& kPlayerStatType_Name(T enum_t_value) {
  static_assert(::std::is_same<T, kPlayerStatType>::value ||
    ::std::is_integral<T>::value,
    "Incorrect type passed to function kPlayerStatType_Name.");
  return ::PROTOBUF_NAMESPACE_ID::internal::NameOfEnum(
    kPlayerStatType_descriptor(), enum_t_value);
}
inline bool kPlayerStatType_Parse(
    ::PROTOBUF_NAMESPACE_ID::ConstStringParam name, kPlayerStatType* value) {
  return ::PROTOBUF_NAMESPACE_ID::internal::ParseNamedEnum<kPlayerStatType>(
    kPlayerStatType_descriptor(), name, value);
}
enum kMonsterStatType : int {
  kMonsterHp = 0,
  kMonsterMp = 1,
  kMonsterStatType_INT_MIN_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::min(),
  kMonsterStatType_INT_MAX_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::max()
};
bool kMonsterStatType_IsValid(int value);
constexpr kMonsterStatType kMonsterStatType_MIN = kMonsterHp;
constexpr kMonsterStatType kMonsterStatType_MAX = kMonsterMp;
constexpr int kMonsterStatType_ARRAYSIZE = kMonsterStatType_MAX + 1;

const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* kMonsterStatType_descriptor();
template<typename T>
inline const std::string& kMonsterStatType_Name(T enum_t_value) {
  static_assert(::std::is_same<T, kMonsterStatType>::value ||
    ::std::is_integral<T>::value,
    "Incorrect type passed to function kMonsterStatType_Name.");
  return ::PROTOBUF_NAMESPACE_ID::internal::NameOfEnum(
    kMonsterStatType_descriptor(), enum_t_value);
}
inline bool kMonsterStatType_Parse(
    ::PROTOBUF_NAMESPACE_ID::ConstStringParam name, kMonsterStatType* value) {
  return ::PROTOBUF_NAMESPACE_ID::internal::ParseNamedEnum<kMonsterStatType>(
    kMonsterStatType_descriptor(), name, value);
}
// ===================================================================


// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__

// @@protoc_insertion_point(namespace_scope)

}  // namespace Protocol

PROTOBUF_NAMESPACE_OPEN

template <> struct is_proto_enum< ::Protocol::kInventoryType> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::Protocol::kInventoryType>() {
  return ::Protocol::kInventoryType_descriptor();
}
template <> struct is_proto_enum< ::Protocol::kLoadState> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::Protocol::kLoadState>() {
  return ::Protocol::kLoadState_descriptor();
}
template <> struct is_proto_enum< ::Protocol::kMonsterState> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::Protocol::kMonsterState>() {
  return ::Protocol::kMonsterState_descriptor();
}
template <> struct is_proto_enum< ::Protocol::kPlayerStatType> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::Protocol::kPlayerStatType>() {
  return ::Protocol::kPlayerStatType_descriptor();
}
template <> struct is_proto_enum< ::Protocol::kMonsterStatType> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::Protocol::kMonsterStatType>() {
  return ::Protocol::kMonsterStatType_descriptor();
}

PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_game_5fenum_2eproto
