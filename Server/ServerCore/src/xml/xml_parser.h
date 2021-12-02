#pragma once
#include "Types.h"
#include "src/memory/container.h"
#include "rapidxml.hpp"
using namespace rapidxml;

/*-------------
	XmlNode
--------------*/

using XmlNodeType = xml_node<WCHAR>;
using XmlDocumentType = xml_document<WCHAR>;
using XmlAttributeType = xml_attribute<WCHAR>;

class XmlNode
{
public:
	XmlNode(XmlNodeType* node = nullptr) : _node(node) { }

	bool				IsValid() { return _node != nullptr; }

	bool				GetBoolAttr(const WCHAR* key, bool defaultValue = false);
	int8_t				GetInt8Attr(const WCHAR* key, int8_t defaultValue = 0);
	int16_t				GetInt16Attr(const WCHAR* key, int16_t defaultValue = 0);
	int32_t				GetInt32Attr(const WCHAR* key, int32_t defaultValue = 0);
	int64_t				GetInt64Attr(const WCHAR* key, int64_t defaultValue = 0);
	float				GetFloatAttr(const WCHAR* key, float defaultValue = 0.0f);
	double				GetDoubleAttr(const WCHAR* key, double defaultValue = 0.0);
	const WCHAR*		GetStringAttr(const WCHAR* key, const WCHAR* defaultValue = L"");

	bool				GetBoolValue(bool defaultValue = false);
	int8_t				GetInt8Value(int8_t defaultValue = 0);
	int16_t				GetInt16Value(int16_t defaultValue = 0);
	int32_t				GetInt32Value(int32_t defaultValue = 0);
	int64_t				GetInt64Value(int64_t defaultValue = 0);
	float				GetFloatValue(float defaultValue = 0.0f);
	double				GetDoubleValue(double defaultValue = 0.0);
	const WCHAR*		GetStringValue(const WCHAR* defaultValue = L"");

	XmlNode				FindChild(const WCHAR* key);
	Vector<XmlNode>		FindChildren(const WCHAR* key);

private:
	XmlNodeType*		_node = nullptr;
};

/*---------------
	XmlParser
----------------*/

class XmlParser
{
public:
	bool ParseFromFile(const WCHAR* path, OUT XmlNode& root);

private:
	std::shared_ptr<XmlDocumentType>		_document = nullptr;
	String									_data;
};