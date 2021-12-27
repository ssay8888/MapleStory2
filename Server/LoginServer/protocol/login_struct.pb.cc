// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: login_struct.proto

#include "login_struct.pb.h"

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
constexpr ViewCharacter::ViewCharacter(
  ::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized)
  : eqp_items_()
  , _eqp_items_cached_byte_size_()
  , name_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , characterid_(int64_t{0})
  , gender_(false){}
struct ViewCharacterDefaultTypeInternal {
  constexpr ViewCharacterDefaultTypeInternal()
    : _instance(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized{}) {}
  ~ViewCharacterDefaultTypeInternal() {}
  union {
    ViewCharacter _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT ViewCharacterDefaultTypeInternal _ViewCharacter_default_instance_;
}  // namespace Protocol
static ::PROTOBUF_NAMESPACE_ID::Metadata file_level_metadata_login_5fstruct_2eproto[1];
static constexpr ::PROTOBUF_NAMESPACE_ID::EnumDescriptor const** file_level_enum_descriptors_login_5fstruct_2eproto = nullptr;
static constexpr ::PROTOBUF_NAMESPACE_ID::ServiceDescriptor const** file_level_service_descriptors_login_5fstruct_2eproto = nullptr;

const ::PROTOBUF_NAMESPACE_ID::uint32 TableStruct_login_5fstruct_2eproto::offsets[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::Protocol::ViewCharacter, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  PROTOBUF_FIELD_OFFSET(::Protocol::ViewCharacter, characterid_),
  PROTOBUF_FIELD_OFFSET(::Protocol::ViewCharacter, name_),
  PROTOBUF_FIELD_OFFSET(::Protocol::ViewCharacter, gender_),
  PROTOBUF_FIELD_OFFSET(::Protocol::ViewCharacter, eqp_items_),
};
static const ::PROTOBUF_NAMESPACE_ID::internal::MigrationSchema schemas[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  { 0, -1, sizeof(::Protocol::ViewCharacter)},
};

static ::PROTOBUF_NAMESPACE_ID::Message const * const file_default_instances[] = {
  reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Message*>(&::Protocol::_ViewCharacter_default_instance_),
};

const char descriptor_table_protodef_login_5fstruct_2eproto[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) =
  "\n\022login_struct.proto\022\010Protocol\032\020login_en"
  "um.proto\"U\n\rViewCharacter\022\023\n\013characterid"
  "\030\001 \001(\003\022\014\n\004name\030\002 \001(\014\022\016\n\006gender\030\003 \001(\010\022\021\n\t"
  "eqp_items\030\004 \003(\005b\006proto3"
  ;
static const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable*const descriptor_table_login_5fstruct_2eproto_deps[1] = {
  &::descriptor_table_login_5fenum_2eproto,
};
static ::PROTOBUF_NAMESPACE_ID::internal::once_flag descriptor_table_login_5fstruct_2eproto_once;
const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_login_5fstruct_2eproto = {
  false, false, 143, descriptor_table_protodef_login_5fstruct_2eproto, "login_struct.proto", 
  &descriptor_table_login_5fstruct_2eproto_once, descriptor_table_login_5fstruct_2eproto_deps, 1, 1,
  schemas, file_default_instances, TableStruct_login_5fstruct_2eproto::offsets,
  file_level_metadata_login_5fstruct_2eproto, file_level_enum_descriptors_login_5fstruct_2eproto, file_level_service_descriptors_login_5fstruct_2eproto,
};
PROTOBUF_ATTRIBUTE_WEAK const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable* descriptor_table_login_5fstruct_2eproto_getter() {
  return &descriptor_table_login_5fstruct_2eproto;
}

// Force running AddDescriptors() at dynamic initialization time.
PROTOBUF_ATTRIBUTE_INIT_PRIORITY static ::PROTOBUF_NAMESPACE_ID::internal::AddDescriptorsRunner dynamic_init_dummy_login_5fstruct_2eproto(&descriptor_table_login_5fstruct_2eproto);
namespace Protocol {

// ===================================================================

class ViewCharacter::_Internal {
 public:
};

ViewCharacter::ViewCharacter(::PROTOBUF_NAMESPACE_ID::Arena* arena)
  : ::PROTOBUF_NAMESPACE_ID::Message(arena),
  eqp_items_(arena) {
  SharedCtor();
  RegisterArenaDtor(arena);
  // @@protoc_insertion_point(arena_constructor:Protocol.ViewCharacter)
}
ViewCharacter::ViewCharacter(const ViewCharacter& from)
  : ::PROTOBUF_NAMESPACE_ID::Message(),
      eqp_items_(from.eqp_items_) {
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  name_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (!from._internal_name().empty()) {
    name_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_name(), 
      GetArenaForAllocation());
  }
  ::memcpy(&characterid_, &from.characterid_,
    static_cast<size_t>(reinterpret_cast<char*>(&gender_) -
    reinterpret_cast<char*>(&characterid_)) + sizeof(gender_));
  // @@protoc_insertion_point(copy_constructor:Protocol.ViewCharacter)
}

void ViewCharacter::SharedCtor() {
name_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
::memset(reinterpret_cast<char*>(this) + static_cast<size_t>(
    reinterpret_cast<char*>(&characterid_) - reinterpret_cast<char*>(this)),
    0, static_cast<size_t>(reinterpret_cast<char*>(&gender_) -
    reinterpret_cast<char*>(&characterid_)) + sizeof(gender_));
}

ViewCharacter::~ViewCharacter() {
  // @@protoc_insertion_point(destructor:Protocol.ViewCharacter)
  SharedDtor();
  _internal_metadata_.Delete<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

void ViewCharacter::SharedDtor() {
  GOOGLE_DCHECK(GetArenaForAllocation() == nullptr);
  name_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}

void ViewCharacter::ArenaDtor(void* object) {
  ViewCharacter* _this = reinterpret_cast< ViewCharacter* >(object);
  (void)_this;
}
void ViewCharacter::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void ViewCharacter::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}

void ViewCharacter::Clear() {
// @@protoc_insertion_point(message_clear_start:Protocol.ViewCharacter)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  eqp_items_.Clear();
  name_.ClearToEmpty();
  ::memset(&characterid_, 0, static_cast<size_t>(
      reinterpret_cast<char*>(&gender_) -
      reinterpret_cast<char*>(&characterid_)) + sizeof(gender_));
  _internal_metadata_.Clear<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

const char* ViewCharacter::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    switch (tag >> 3) {
      // int64 characterid = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 8)) {
          characterid_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // bytes name = 2;
      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 18)) {
          auto str = _internal_mutable_name();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // bool gender = 3;
      case 3:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 24)) {
          gender_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // repeated int32 eqp_items = 4;
      case 4:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 34)) {
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::PackedInt32Parser(_internal_mutable_eqp_items(), ptr, ctx);
          CHK_(ptr);
        } else if (static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 32) {
          _internal_add_eqp_items(::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr));
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

::PROTOBUF_NAMESPACE_ID::uint8* ViewCharacter::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:Protocol.ViewCharacter)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // int64 characterid = 1;
  if (this->characterid() != 0) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteInt64ToArray(1, this->_internal_characterid(), target);
  }

  // bytes name = 2;
  if (!this->name().empty()) {
    target = stream->WriteBytesMaybeAliased(
        2, this->_internal_name(), target);
  }

  // bool gender = 3;
  if (this->gender() != 0) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteBoolToArray(3, this->_internal_gender(), target);
  }

  // repeated int32 eqp_items = 4;
  {
    int byte_size = _eqp_items_cached_byte_size_.load(std::memory_order_relaxed);
    if (byte_size > 0) {
      target = stream->WriteInt32Packed(
          4, _internal_eqp_items(), byte_size, target);
    }
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(::PROTOBUF_NAMESPACE_ID::UnknownFieldSet::default_instance), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:Protocol.ViewCharacter)
  return target;
}

size_t ViewCharacter::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:Protocol.ViewCharacter)
  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // repeated int32 eqp_items = 4;
  {
    size_t data_size = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::
      Int32Size(this->eqp_items_);
    if (data_size > 0) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::Int32Size(
            static_cast<::PROTOBUF_NAMESPACE_ID::int32>(data_size));
    }
    int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(data_size);
    _eqp_items_cached_byte_size_.store(cached_size,
                                    std::memory_order_relaxed);
    total_size += data_size;
  }

  // bytes name = 2;
  if (!this->name().empty()) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::BytesSize(
        this->_internal_name());
  }

  // int64 characterid = 1;
  if (this->characterid() != 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::Int64Size(
        this->_internal_characterid());
  }

  // bool gender = 3;
  if (this->gender() != 0) {
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

void ViewCharacter::MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:Protocol.ViewCharacter)
  GOOGLE_DCHECK_NE(&from, this);
  const ViewCharacter* source =
      ::PROTOBUF_NAMESPACE_ID::DynamicCastToGenerated<ViewCharacter>(
          &from);
  if (source == nullptr) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:Protocol.ViewCharacter)
    ::PROTOBUF_NAMESPACE_ID::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:Protocol.ViewCharacter)
    MergeFrom(*source);
  }
}

void ViewCharacter::MergeFrom(const ViewCharacter& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:Protocol.ViewCharacter)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  eqp_items_.MergeFrom(from.eqp_items_);
  if (!from.name().empty()) {
    _internal_set_name(from._internal_name());
  }
  if (from.characterid() != 0) {
    _internal_set_characterid(from._internal_characterid());
  }
  if (from.gender() != 0) {
    _internal_set_gender(from._internal_gender());
  }
}

void ViewCharacter::CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:Protocol.ViewCharacter)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void ViewCharacter::CopyFrom(const ViewCharacter& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:Protocol.ViewCharacter)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool ViewCharacter::IsInitialized() const {
  return true;
}

void ViewCharacter::InternalSwap(ViewCharacter* other) {
  using std::swap;
  _internal_metadata_.InternalSwap(&other->_internal_metadata_);
  eqp_items_.InternalSwap(&other->eqp_items_);
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::InternalSwap(
      &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(),
      &name_, GetArenaForAllocation(),
      &other->name_, other->GetArenaForAllocation()
  );
  ::PROTOBUF_NAMESPACE_ID::internal::memswap<
      PROTOBUF_FIELD_OFFSET(ViewCharacter, gender_)
      + sizeof(ViewCharacter::gender_)
      - PROTOBUF_FIELD_OFFSET(ViewCharacter, characterid_)>(
          reinterpret_cast<char*>(&characterid_),
          reinterpret_cast<char*>(&other->characterid_));
}

::PROTOBUF_NAMESPACE_ID::Metadata ViewCharacter::GetMetadata() const {
  return ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(
      &descriptor_table_login_5fstruct_2eproto_getter, &descriptor_table_login_5fstruct_2eproto_once,
      file_level_metadata_login_5fstruct_2eproto[0]);
}

// @@protoc_insertion_point(namespace_scope)
}  // namespace Protocol
PROTOBUF_NAMESPACE_OPEN
template<> PROTOBUF_NOINLINE ::Protocol::ViewCharacter* Arena::CreateMaybeMessage< ::Protocol::ViewCharacter >(Arena* arena) {
  return Arena::CreateMessageInternal< ::Protocol::ViewCharacter >(arena);
}
PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)
#include <google/protobuf/port_undef.inc>