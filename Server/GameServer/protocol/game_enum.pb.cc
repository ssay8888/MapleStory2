// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: game_enum.proto

#include "game_enum.pb.h"

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
}  // namespace Protocol
static const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* file_level_enum_descriptors_game_5fenum_2eproto[2];
static constexpr ::PROTOBUF_NAMESPACE_ID::ServiceDescriptor const** file_level_service_descriptors_game_5fenum_2eproto = nullptr;
const ::PROTOBUF_NAMESPACE_ID::uint32 TableStruct_game_5fenum_2eproto::offsets[1] = {};
static constexpr ::PROTOBUF_NAMESPACE_ID::internal::MigrationSchema* schemas = nullptr;
static constexpr ::PROTOBUF_NAMESPACE_ID::Message* const* file_default_instances = nullptr;

const char descriptor_table_protodef_game_5fenum_2eproto[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) =
  "\n\017game_enum.proto\022\010Protocol*a\n\016kInventor"
  "yType\022\026\n\022kInventoryEquipped\020\000\022\021\n\rkInvent"
  "oryEqp\020\001\022\021\n\rkInventoryEtc\020\002\022\021\n\rkInventor"
  "yEnd\020\003*/\n\nkLoadState\022\020\n\014kLoadSuccess\020\000\022\017"
  "\n\013kLoadFailed\020\001b\006proto3"
  ;
static ::PROTOBUF_NAMESPACE_ID::internal::once_flag descriptor_table_game_5fenum_2eproto_once;
const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_game_5fenum_2eproto = {
  false, false, 183, descriptor_table_protodef_game_5fenum_2eproto, "game_enum.proto", 
  &descriptor_table_game_5fenum_2eproto_once, nullptr, 0, 0,
  schemas, file_default_instances, TableStruct_game_5fenum_2eproto::offsets,
  nullptr, file_level_enum_descriptors_game_5fenum_2eproto, file_level_service_descriptors_game_5fenum_2eproto,
};
PROTOBUF_ATTRIBUTE_WEAK const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable* descriptor_table_game_5fenum_2eproto_getter() {
  return &descriptor_table_game_5fenum_2eproto;
}

// Force running AddDescriptors() at dynamic initialization time.
PROTOBUF_ATTRIBUTE_INIT_PRIORITY static ::PROTOBUF_NAMESPACE_ID::internal::AddDescriptorsRunner dynamic_init_dummy_game_5fenum_2eproto(&descriptor_table_game_5fenum_2eproto);
namespace Protocol {
const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* kInventoryType_descriptor() {
  ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&descriptor_table_game_5fenum_2eproto);
  return file_level_enum_descriptors_game_5fenum_2eproto[0];
}
bool kInventoryType_IsValid(int value) {
  switch (value) {
    case 0:
    case 1:
    case 2:
    case 3:
      return true;
    default:
      return false;
  }
}

const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* kLoadState_descriptor() {
  ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&descriptor_table_game_5fenum_2eproto);
  return file_level_enum_descriptors_game_5fenum_2eproto[1];
}
bool kLoadState_IsValid(int value) {
  switch (value) {
    case 0:
    case 1:
      return true;
    default:
      return false;
  }
}


// @@protoc_insertion_point(namespace_scope)
}  // namespace Protocol
PROTOBUF_NAMESPACE_OPEN
PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)
#include <google/protobuf/port_undef.inc>
