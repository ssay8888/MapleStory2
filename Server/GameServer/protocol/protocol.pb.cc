// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: protocol.proto

#include "protocol.pb.h"

#include <algorithm>

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/wire_format_lite.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>

PROTOBUF_PRAGMA_INIT_SEG
namespace Protocol {
constexpr C_LOGIN::C_LOGIN(
  ::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized){}
struct C_LOGINDefaultTypeInternal {
  constexpr C_LOGINDefaultTypeInternal()
    : _instance(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized{}) {}
  ~C_LOGINDefaultTypeInternal() {}
  union {
    C_LOGIN _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT C_LOGINDefaultTypeInternal _C_LOGIN_default_instance_;
constexpr S_LOGIN::S_LOGIN(
  ::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized)
  : success_(false){}
struct S_LOGINDefaultTypeInternal {
  constexpr S_LOGINDefaultTypeInternal()
    : _instance(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized{}) {}
  ~S_LOGINDefaultTypeInternal() {}
  union {
    S_LOGIN _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT S_LOGINDefaultTypeInternal _S_LOGIN_default_instance_;
}  // namespace Protocol
static ::PROTOBUF_NAMESPACE_ID::Metadata file_level_metadata_protocol_2eproto[2];
static constexpr ::PROTOBUF_NAMESPACE_ID::EnumDescriptor const** file_level_enum_descriptors_protocol_2eproto = nullptr;
static constexpr ::PROTOBUF_NAMESPACE_ID::ServiceDescriptor const** file_level_service_descriptors_protocol_2eproto = nullptr;

const ::PROTOBUF_NAMESPACE_ID::uint32 TableStruct_protocol_2eproto::offsets[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::Protocol::C_LOGIN, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::Protocol::S_LOGIN, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  PROTOBUF_FIELD_OFFSET(::Protocol::S_LOGIN, success_),
};
static const ::PROTOBUF_NAMESPACE_ID::internal::MigrationSchema schemas[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  { 0, -1, sizeof(::Protocol::C_LOGIN)},
  { 5, -1, sizeof(::Protocol::S_LOGIN)},
};

static ::PROTOBUF_NAMESPACE_ID::Message const * const file_default_instances[] = {
  reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Message*>(&::Protocol::_C_LOGIN_default_instance_),
  reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Message*>(&::Protocol::_S_LOGIN_default_instance_),
};

const char descriptor_table_protodef_protocol_2eproto[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) =
  "\n\016protocol.proto\022\010Protocol\032\nenum.proto\032\014"
  "struct.proto\"\t\n\007C_LOGIN\"\032\n\007S_LOGIN\022\017\n\007su"
  "ccess\030\001 \001(\010b\006proto3"
  ;
static const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable*const descriptor_table_protocol_2eproto_deps[2] = {
  &::descriptor_table_enum_2eproto,
  &::descriptor_table_struct_2eproto,
};
static ::PROTOBUF_NAMESPACE_ID::internal::once_flag descriptor_table_protocol_2eproto_once;
const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_protocol_2eproto = {
  false, false, 99, descriptor_table_protodef_protocol_2eproto, "protocol.proto", 
  &descriptor_table_protocol_2eproto_once, descriptor_table_protocol_2eproto_deps, 2, 2,
  schemas, file_default_instances, TableStruct_protocol_2eproto::offsets,
  file_level_metadata_protocol_2eproto, file_level_enum_descriptors_protocol_2eproto, file_level_service_descriptors_protocol_2eproto,
};
PROTOBUF_ATTRIBUTE_WEAK const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable* descriptor_table_protocol_2eproto_getter() {
  return &descriptor_table_protocol_2eproto;
}

// Force running AddDescriptors() at dynamic initialization time.
PROTOBUF_ATTRIBUTE_INIT_PRIORITY static ::PROTOBUF_NAMESPACE_ID::internal::AddDescriptorsRunner dynamic_init_dummy_protocol_2eproto(&descriptor_table_protocol_2eproto);
namespace Protocol {

// ===================================================================

class C_LOGIN::_Internal {
 public:
};

C_LOGIN::C_LOGIN(::PROTOBUF_NAMESPACE_ID::Arena* arena)
  : ::PROTOBUF_NAMESPACE_ID::Message(arena) {
  SharedCtor();
  RegisterArenaDtor(arena);
  // @@protoc_insertion_point(arena_constructor:Protocol.C_LOGIN)
}
C_LOGIN::C_LOGIN(const C_LOGIN& from)
  : ::PROTOBUF_NAMESPACE_ID::Message() {
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  // @@protoc_insertion_point(copy_constructor:Protocol.C_LOGIN)
}

void C_LOGIN::SharedCtor() {
}

C_LOGIN::~C_LOGIN() {
  // @@protoc_insertion_point(destructor:Protocol.C_LOGIN)
  SharedDtor();
  _internal_metadata_.Delete<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

void C_LOGIN::SharedDtor() {
  GOOGLE_DCHECK(GetArenaForAllocation() == nullptr);
}

void C_LOGIN::ArenaDtor(void* object) {
  C_LOGIN* _this = reinterpret_cast< C_LOGIN* >(object);
  (void)_this;
}
void C_LOGIN::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void C_LOGIN::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}

void C_LOGIN::Clear() {
// @@protoc_insertion_point(message_clear_start:Protocol.C_LOGIN)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  _internal_metadata_.Clear<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

const char* C_LOGIN::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
        if ((tag == 0) || ((tag & 7) == 4)) {
          CHK_(ptr);
          ctx->SetLastTag(tag);
          goto success;
        }
        ptr = UnknownFieldParse(tag,
            _internal_metadata_.mutable_unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(),
            ptr, ctx);
        CHK_(ptr != nullptr);
        continue;
  }  // while
success:
  return ptr;
failure:
  ptr = nullptr;
  goto success;
#undef CHK_
}

::PROTOBUF_NAMESPACE_ID::uint8* C_LOGIN::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:Protocol.C_LOGIN)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(::PROTOBUF_NAMESPACE_ID::UnknownFieldSet::default_instance), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:Protocol.C_LOGIN)
  return target;
}

size_t C_LOGIN::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:Protocol.C_LOGIN)
  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    return ::PROTOBUF_NAMESPACE_ID::internal::ComputeUnknownFieldsSize(
        _internal_metadata_, total_size, &_cached_size_);
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void C_LOGIN::MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:Protocol.C_LOGIN)
  GOOGLE_DCHECK_NE(&from, this);
  const C_LOGIN* source =
      ::PROTOBUF_NAMESPACE_ID::DynamicCastToGenerated<C_LOGIN>(
          &from);
  if (source == nullptr) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:Protocol.C_LOGIN)
    ::PROTOBUF_NAMESPACE_ID::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:Protocol.C_LOGIN)
    MergeFrom(*source);
  }
}

void C_LOGIN::MergeFrom(const C_LOGIN& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:Protocol.C_LOGIN)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

}

void C_LOGIN::CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:Protocol.C_LOGIN)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void C_LOGIN::CopyFrom(const C_LOGIN& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:Protocol.C_LOGIN)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool C_LOGIN::IsInitialized() const {
  return true;
}

void C_LOGIN::InternalSwap(C_LOGIN* other) {
  using std::swap;
  _internal_metadata_.InternalSwap(&other->_internal_metadata_);
}

::PROTOBUF_NAMESPACE_ID::Metadata C_LOGIN::GetMetadata() const {
  return ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(
      &descriptor_table_protocol_2eproto_getter, &descriptor_table_protocol_2eproto_once,
      file_level_metadata_protocol_2eproto[0]);
}

// ===================================================================

class S_LOGIN::_Internal {
 public:
};

S_LOGIN::S_LOGIN(::PROTOBUF_NAMESPACE_ID::Arena* arena)
  : ::PROTOBUF_NAMESPACE_ID::Message(arena) {
  SharedCtor();
  RegisterArenaDtor(arena);
  // @@protoc_insertion_point(arena_constructor:Protocol.S_LOGIN)
}
S_LOGIN::S_LOGIN(const S_LOGIN& from)
  : ::PROTOBUF_NAMESPACE_ID::Message() {
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  success_ = from.success_;
  // @@protoc_insertion_point(copy_constructor:Protocol.S_LOGIN)
}

void S_LOGIN::SharedCtor() {
success_ = false;
}

S_LOGIN::~S_LOGIN() {
  // @@protoc_insertion_point(destructor:Protocol.S_LOGIN)
  SharedDtor();
  _internal_metadata_.Delete<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

void S_LOGIN::SharedDtor() {
  GOOGLE_DCHECK(GetArenaForAllocation() == nullptr);
}

void S_LOGIN::ArenaDtor(void* object) {
  S_LOGIN* _this = reinterpret_cast< S_LOGIN* >(object);
  (void)_this;
}
void S_LOGIN::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void S_LOGIN::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}

void S_LOGIN::Clear() {
// @@protoc_insertion_point(message_clear_start:Protocol.S_LOGIN)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  success_ = false;
  _internal_metadata_.Clear<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

const char* S_LOGIN::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    switch (tag >> 3) {
      // bool success = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 8)) {
          success_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      default: {
      handle_unusual:
        if ((tag == 0) || ((tag & 7) == 4)) {
          CHK_(ptr);
          ctx->SetLastTag(tag);
          goto success;
        }
        ptr = UnknownFieldParse(tag,
            _internal_metadata_.mutable_unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(),
            ptr, ctx);
        CHK_(ptr != nullptr);
        continue;
      }
    }  // switch
  }  // while
success:
  return ptr;
failure:
  ptr = nullptr;
  goto success;
#undef CHK_
}

::PROTOBUF_NAMESPACE_ID::uint8* S_LOGIN::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:Protocol.S_LOGIN)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // bool success = 1;
  if (this->success() != 0) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteBoolToArray(1, this->_internal_success(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(::PROTOBUF_NAMESPACE_ID::UnknownFieldSet::default_instance), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:Protocol.S_LOGIN)
  return target;
}

size_t S_LOGIN::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:Protocol.S_LOGIN)
  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // bool success = 1;
  if (this->success() != 0) {
    total_size += 1 + 1;
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    return ::PROTOBUF_NAMESPACE_ID::internal::ComputeUnknownFieldsSize(
        _internal_metadata_, total_size, &_cached_size_);
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void S_LOGIN::MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:Protocol.S_LOGIN)
  GOOGLE_DCHECK_NE(&from, this);
  const S_LOGIN* source =
      ::PROTOBUF_NAMESPACE_ID::DynamicCastToGenerated<S_LOGIN>(
          &from);
  if (source == nullptr) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:Protocol.S_LOGIN)
    ::PROTOBUF_NAMESPACE_ID::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:Protocol.S_LOGIN)
    MergeFrom(*source);
  }
}

void S_LOGIN::MergeFrom(const S_LOGIN& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:Protocol.S_LOGIN)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  if (from.success() != 0) {
    _internal_set_success(from._internal_success());
  }
}

void S_LOGIN::CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:Protocol.S_LOGIN)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void S_LOGIN::CopyFrom(const S_LOGIN& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:Protocol.S_LOGIN)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool S_LOGIN::IsInitialized() const {
  return true;
}

void S_LOGIN::InternalSwap(S_LOGIN* other) {
  using std::swap;
  _internal_metadata_.InternalSwap(&other->_internal_metadata_);
  swap(success_, other->success_);
}

::PROTOBUF_NAMESPACE_ID::Metadata S_LOGIN::GetMetadata() const {
  return ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(
      &descriptor_table_protocol_2eproto_getter, &descriptor_table_protocol_2eproto_once,
      file_level_metadata_protocol_2eproto[1]);
}

// @@protoc_insertion_point(namespace_scope)
}  // namespace Protocol
PROTOBUF_NAMESPACE_OPEN
template<> PROTOBUF_NOINLINE ::Protocol::C_LOGIN* Arena::CreateMaybeMessage< ::Protocol::C_LOGIN >(Arena* arena) {
  return Arena::CreateMessageInternal< ::Protocol::C_LOGIN >(arena);
}
template<> PROTOBUF_NOINLINE ::Protocol::S_LOGIN* Arena::CreateMaybeMessage< ::Protocol::S_LOGIN >(Arena* arena) {
  return Arena::CreateMessageInternal< ::Protocol::S_LOGIN >(arena);
}
PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)
#include <google/protobuf/port_undef.inc>
