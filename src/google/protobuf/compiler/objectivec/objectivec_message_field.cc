// Protocol Buffers - Google's data interchange format
// Copyright 2008 Google Inc.
// http://code.google.com/p/protobuf/
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Author: kenton@google.com (Kenton Varda)
//  Based on original Protocol Buffers design by
//  Sanjay Ghemawat, Jeff Dean, and others.

#include <map>
#include <string>

#include <google/protobuf/compiler/objectivec/objectivec_message_field.h>
#include <google/protobuf/compiler/objectivec/objectivec_helpers.h>
#include <google/protobuf/io/printer.h>
#include <google/protobuf/wire_format.h>
#include <google/protobuf/stubs/strutil.h>

namespace google {
namespace protobuf {
namespace compiler {
namespace objectivec {

namespace {
void SetMessageVariables(const FieldDescriptor* descriptor,
		map<string, string>* variables) {
	(*variables)["classname"] = ClassName(descriptor->containing_type());
	(*variables)["name"] = UnderscoresToCamelCase(descriptor);
	(*variables)["capitalized_name"] = UnderscoresToCapitalizedCamelCase(
			descriptor);
	(*variables)["number"] = SimpleItoa(descriptor->number());
	(*variables)["type"] = ClassName(descriptor->message_type());
	if (IsPrimitiveType(GetObjectiveCType(descriptor))) {
		(*variables)["storage_type"] = ClassName(descriptor->message_type());
	} else {
		(*variables)["storage_type"] = string(
				ClassName(descriptor->message_type())) + "*";
	}
	(*variables)["group_or_message"] =
			(descriptor->type() == FieldDescriptor::TYPE_GROUP) ?
					"Group" : "Message";

	(*variables)["boxed_value"] = BoxValue(descriptor, "_value");

	string unboxed_value = "_value";
	switch (GetObjectiveCType(descriptor)) {
	case OBJECTIVECTYPE_INT:
		unboxed_value = "[_value intValue]";
		break;
	case OBJECTIVECTYPE_LONG:
		unboxed_value = "[_value longLongValue]";
		break;
	case OBJECTIVECTYPE_FLOAT:
		unboxed_value = "[_value floatValue]";
		break;
	case OBJECTIVECTYPE_DOUBLE:
		unboxed_value = "[_value doubleValue]";
		break;
	case OBJECTIVECTYPE_BOOLEAN:
		unboxed_value = "[_value boolValue]";
		break;
	}

	(*variables)["unboxed_value"] = unboxed_value;
}
} // namespace

MessageFieldGenerator::MessageFieldGenerator(const FieldDescriptor* descriptor) :
		descriptor_(descriptor) {
	SetMessageVariables(descriptor, &variables_);
}

MessageFieldGenerator::~MessageFieldGenerator() {
}

void MessageFieldGenerator::GenerateHasFieldHeader(io::Printer* printer) const {
	printer->Print(variables_, "BOOL has$capitalized_name$_:1;\n");
}

void MessageFieldGenerator::GenerateFieldHeader(io::Printer* printer) const {
	printer->Print(variables_, "$storage_type$ $name$_;\n");
}

void MessageFieldGenerator::GenerateHasPropertyHeader(
		io::Printer* printer) const {
	printer->Print(variables_, "- (BOOL) has$capitalized_name$;\n");
}

void MessageFieldGenerator::GeneratePropertyHeader(io::Printer* printer) const {
	printer->Print(variables_,
			"@property (nonatomic, retain) $storage_type$ $name$;\n");
}

void MessageFieldGenerator::GenerateExtensionSource(
		io::Printer* printer) const {
}

void MessageFieldGenerator::GenerateMembersHeader(io::Printer* printer) const {
}

void MessageFieldGenerator::GenerateSynthesizeSource(
		io::Printer* printer) const {
	printer->Print(variables_, "- (BOOL) has$capitalized_name$ {\n"
			"  return !!has$capitalized_name$_;\n"
			"}\n"
			"- (void) setHas$capitalized_name$:(BOOL) _value {\n"
			"  has$capitalized_name$_ = !!_value;\n"
			"}\n"
			"@synthesize $name$=$name$_;\n"
			"- (void) set$capitalized_name$:($storage_type$) _value {\n"
			"  if ($name$_ == _value) return;\n"
			"  [$name$_ autorelease];\n"
			"  $name$_ = [_value retain];\n"
			"  if (_value != nil) self.has$capitalized_name$ = true;\n"
			"  else self.has$capitalized_name$ = false;\n"
			"}\n");
}

void MessageFieldGenerator::GenerateDeallocSource(io::Printer* printer) const {
	printer->Print(variables_, "self.$name$ = nil;\n");
}

void MessageFieldGenerator::GenerateInitializationSource(
		io::Printer* printer) const {
	printer->Print(variables_, "self.$name$ = [$type$ defaultInstance];\n");
}

void MessageFieldGenerator::GenerateBuilderMembersHeader(
		io::Printer* printer) const {
	printer->Print(variables_,
			"- (BOOL) has$capitalized_name$;\n"
					"- ($storage_type$) $name$;\n"
					"- ($classname$_Builder*) set$capitalized_name$:($storage_type$) _value;\n"
					"- ($classname$_Builder*) set$capitalized_name$Builder:($type$_Builder*) builderForValue;\n"
					"- ($classname$_Builder*) merge$capitalized_name$:($storage_type$) _value;\n"
					"- ($classname$_Builder*) clear$capitalized_name$;\n");
}

void MessageFieldGenerator::GenerateBuilderMembersSource(
		io::Printer* printer) const {
	printer->Print(variables_,
			"- (BOOL) has$capitalized_name$ {\n"
					"  return protobufBuilderResult.has$capitalized_name$;\n"
					"}\n"
					"- ($storage_type$) $name$ {\n"
					"  return protobufBuilderResult.$name$;\n"
					"}\n"
					"- ($classname$_Builder*) set$capitalized_name$:($storage_type$) _value {\n"
					"  protobufBuilderResult.has$capitalized_name$ = YES;\n"
					"  protobufBuilderResult.$name$ = _value;\n"
					"  return self;\n"
					"}\n"
					"- ($classname$_Builder*) set$capitalized_name$Builder:($type$_Builder*) builderForValue {\n"
					"  return [self set$capitalized_name$:[builderForValue build]];\n"
					"}\n"
					"- ($classname$_Builder*) merge$capitalized_name$:($storage_type$) _value {\n"
					"  if (protobufBuilderResult.has$capitalized_name$ &&\n"
					"      protobufBuilderResult.$name$ != [$type$ defaultInstance]) {\n"
					"    protobufBuilderResult.$name$ =\n"
					"      [[[$type$ builderWithPrototype:protobufBuilderResult.$name$] mergeFrom:_value] buildPartial];\n"
					"  } else {\n"
					"    protobufBuilderResult.$name$ = _value;\n"
					"  }\n"
					"  protobufBuilderResult.has$capitalized_name$ = YES;\n"
					"  return self;\n"
					"}\n"
					"- ($classname$_Builder*) clear$capitalized_name$ {\n"
					"  protobufBuilderResult.$name$ = nil;\n"
					"  return self;\n"
					"}\n");
}

void MessageFieldGenerator::GenerateMergingCodeHeader(
		io::Printer* printer) const {
}

void MessageFieldGenerator::GenerateMergingCodeSource(
		io::Printer* printer) const {
	printer->Print(variables_, "if (other.has$capitalized_name$) {\n"
			"  [self merge$capitalized_name$:other.$name$];\n"
			"}\n");
}

void MessageFieldGenerator::GenerateBuildingCodeHeader(
		io::Printer* printer) const {
}

void MessageFieldGenerator::GenerateBuildingCodeSource(
		io::Printer* printer) const {
}

void MessageFieldGenerator::GenerateParsingCodeHeader(
		io::Printer* printer) const {
}

void MessageFieldGenerator::GenerateParsingCodeSource(
		io::Printer* printer) const {
	printer->Print(variables_,
			"$type$_Builder* subBuilder = [$type$ builder];\n"
					"if (self.has$capitalized_name$) {\n"
					"  [subBuilder mergeFrom:self.$name$];\n"
					"}\n");

	if (descriptor_->type() == FieldDescriptor::TYPE_GROUP) {
		printer->Print(variables_,
				"[input readGroup:$number$ builder:subBuilder extensionRegistry:extensionRegistry];\n");
	} else {
		printer->Print(variables_,
				"[input readMessage:subBuilder extensionRegistry:extensionRegistry];\n");
	}

	printer->Print(variables_,
			"[self set$capitalized_name$:[subBuilder buildPartial]];\n");
}

void MessageFieldGenerator::GenerateSerializationCodeHeader(
		io::Printer* printer) const {
}

void MessageFieldGenerator::GenerateSerializationCodeSource(
		io::Printer* printer) const {
	printer->Print(variables_, "if (self.has$capitalized_name$) {\n"
			"  [output write$group_or_message$:$number$ value:self.$name$];\n"
			"}\n");
}

void MessageFieldGenerator::GenerateSerializedSizeCodeHeader(
		io::Printer* printer) const {
}

void MessageFieldGenerator::GenerateSerializedSizeCodeSource(
		io::Printer* printer) const {
	printer->Print(variables_, "if (self.has$capitalized_name$) {\n"
			"  size += compute$group_or_message$Size($number$, self.$name$);\n"
			"}\n");
}

void MessageFieldGenerator::GenerateMembersSource(io::Printer* printer) const {
}

string MessageFieldGenerator::GetBoxedType() const {
	return ClassName(descriptor_->message_type());
}

RepeatedMessageFieldGenerator::RepeatedMessageFieldGenerator(
		const FieldDescriptor* descriptor) :
		descriptor_(descriptor) {
	SetMessageVariables(descriptor, &variables_);
}

RepeatedMessageFieldGenerator::~RepeatedMessageFieldGenerator() {
}

void RepeatedMessageFieldGenerator::GenerateHasFieldHeader(
		io::Printer* printer) const {
}

void RepeatedMessageFieldGenerator::GenerateFieldHeader(
		io::Printer* printer) const {
	printer->Print(variables_, "NSMutableArray* $name$;\n");
}

void RepeatedMessageFieldGenerator::GenerateHasPropertyHeader(
		io::Printer* printer) const {
}

void RepeatedMessageFieldGenerator::GeneratePropertyHeader(
		io::Printer* printer) const {
}

void RepeatedMessageFieldGenerator::GenerateExtensionSource(
		io::Printer* printer) const {
}

void RepeatedMessageFieldGenerator::GenerateSynthesizeSource(
		io::Printer* printer) const {
	printer->Print(variables_, "@synthesize $name$;\n");
}

void RepeatedMessageFieldGenerator::GenerateDeallocSource(
		io::Printer* printer) const {
	printer->Print(variables_, "self.$name$ = nil;\n");
}

void RepeatedMessageFieldGenerator::GenerateInitializationSource(
		io::Printer* printer) const {
}

void RepeatedMessageFieldGenerator::GenerateMembersHeader(
		io::Printer* printer) const {
	printer->Print(variables_,
			"@property (nonatomic, retain) NSMutableArray* $name$;\n");
}

void RepeatedMessageFieldGenerator::GenerateMembersSource(
		io::Printer* printer) const {
	printer->Print(variables_, "- (NSArray*) $name$ {\n"
			"  return $name$;\n"
			"}\n"
			"- ($storage_type$) $name$AtIndex:(int32_t) index {\n"
			"  id _value = [$name$ objectAtIndex:index];\n"
			"  return $unboxed_value$;\n"
			"}\n");
}

void RepeatedMessageFieldGenerator::GenerateBuilderMembersHeader(
		io::Printer* printer) const {
	printer->Print(variables_,
			"- (NSArray*) $name$;\n"
					"- ($storage_type$) $name$AtIndex:(int32_t) index;\n"
					"- ($classname$_Builder*) replace$capitalized_name$AtIndex:(int32_t) index with:($storage_type$) _value;\n"
					"- ($classname$_Builder*) add$capitalized_name$:($storage_type$) _value;\n"
					"- ($classname$_Builder*) addAll$capitalized_name$:(NSArray*) values;\n"
					"- ($classname$_Builder*) clear$capitalized_name$List;\n");
}

void RepeatedMessageFieldGenerator::GenerateBuilderMembersSource(
		io::Printer* printer) const {
	printer->Print(variables_,
			"- (NSArray*) $name$ {\n"
					"  if (protobufBuilderResult.$name$ == nil) { return [NSArray array]; }\n"
					"  return protobufBuilderResult.$name$;\n"
					"}\n"
					"- ($storage_type$) $name$AtIndex:(int32_t) index {\n"
					"  return [protobufBuilderResult $name$AtIndex:index];\n"
					"}\n"
					"- ($classname$_Builder*) replace$capitalized_name$AtIndex:(int32_t) index with:($storage_type$) _value {\n"
					"  [protobufBuilderResult.$name$ replaceObjectAtIndex:index withObject:$boxed_value$];\n"
					"  return self;\n"
					"}\n"
					"- ($classname$_Builder*) addAll$capitalized_name$:(NSArray*) values {\n"
					"  if (protobufBuilderResult.$name$ == nil) {\n"
					"    protobufBuilderResult.$name$ = [NSMutableArray array];\n"
					"  }\n"
					"  [protobufBuilderResult.$name$ addObjectsFromArray:values];\n"
					"  return self;\n"
					"}\n"
					"- ($classname$_Builder*) clear$capitalized_name$List {\n"
					"  protobufBuilderResult.$name$ = nil;\n"
					"  return self;\n"
					"}\n");

	printer->Print(variables_,
			"- ($classname$_Builder*) add$capitalized_name$:($storage_type$) _value {\n"
					"  if (protobufBuilderResult.$name$ == nil) {\n"
					"    protobufBuilderResult.$name$ = [NSMutableArray array];\n"
					"  }\n"
					"  [protobufBuilderResult.$name$ addObject:$boxed_value$];\n"
					"  return self;\n"
					"}\n");
}

void RepeatedMessageFieldGenerator::GenerateMergingCodeHeader(
		io::Printer* printer) const {
}

void RepeatedMessageFieldGenerator::GenerateBuildingCodeHeader(
		io::Printer* printer) const {
}

void RepeatedMessageFieldGenerator::GenerateParsingCodeHeader(
		io::Printer* printer) const {
}

void RepeatedMessageFieldGenerator::GenerateSerializationCodeHeader(
		io::Printer* printer) const {
}

void RepeatedMessageFieldGenerator::GenerateSerializedSizeCodeHeader(
		io::Printer* printer) const {
}

void RepeatedMessageFieldGenerator::GenerateMergingCodeSource(
		io::Printer* printer) const {
	printer->Print(variables_,
			"if (other.$name$.count > 0) {\n"
					"  if (protobufBuilderResult.$name$ == nil) {\n"
					"    protobufBuilderResult.$name$ = [NSMutableArray array];\n"
					"  }\n"
					"  [protobufBuilderResult.$name$ addObjectsFromArray:other.$name$];\n"
					"}\n");
}

void RepeatedMessageFieldGenerator::GenerateBuildingCodeSource(
		io::Printer* printer) const {
}

void RepeatedMessageFieldGenerator::GenerateParsingCodeSource(
		io::Printer* printer) const {
	printer->Print(variables_,
			"$type$_Builder* subBuilder = [$type$ builder];\n");

	if (descriptor_->type() == FieldDescriptor::TYPE_GROUP) {
		printer->Print(variables_,
				"[input readGroup:$number$ builder:subBuilder extensionRegistry:extensionRegistry];\n");
	} else {
		printer->Print(variables_,
				"[input readMessage:subBuilder extensionRegistry:extensionRegistry];\n");
	}

	printer->Print(variables_,
			"[self add$capitalized_name$:[subBuilder buildPartial]];\n");
}

void RepeatedMessageFieldGenerator::GenerateSerializationCodeSource(
		io::Printer* printer) const {
	printer->Print(variables_, "for ($type$* element in self.$name$) {\n"
			"  [output write$group_or_message$:$number$ value:element];\n"
			"}\n");
}

void RepeatedMessageFieldGenerator::GenerateSerializedSizeCodeSource(
		io::Printer* printer) const {
	printer->Print(variables_, "for ($type$* element in self.$name$) {\n"
			"  size += compute$group_or_message$Size($number$, element);\n"
			"}\n");
}

string RepeatedMessageFieldGenerator::GetBoxedType() const {
	return ClassName(descriptor_->message_type());
}
} // namespace objectivec
} // namespace compiler
} // namespace protobuf
} // namespace google
