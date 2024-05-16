#include "llm_db.hpp"
#include <gdextension_interface.h>
#include <godot_cpp/classes/global_constants.hpp>
#include <godot_cpp/core/binder_common.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/memory.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/typed_array.hpp>
#include <godot_cpp/variant/utility_functions.hpp>


namespace godot {

void LlmDBSchemaData::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_name"), &LlmDBSchemaData::get_name);
    ClassDB::bind_method(D_METHOD("set_name", "p_name"), &LlmDBSchemaData::set_name);
    ClassDB::add_property("LlmDBSchemaData", PropertyInfo(Variant::STRING, "name", PROPERTY_HINT_NONE), "set_name", "get_name");

    ClassDB::bind_method(D_METHOD("get_type"), &LlmDBSchemaData::get_type);
    ClassDB::bind_method(D_METHOD("set_type", "p_type"), &LlmDBSchemaData::set_type);
    ClassDB::add_property("LlmDBSchemaData", PropertyInfo(Variant::INT, "type", PROPERTY_HINT_ENUM, "INTEGER, REAL, TEXT, BLOB"), "set_type", "get_type");

    ClassDB::bind_static_method("LlmDBSchemaData", D_METHOD("create_int"), &LlmDBSchemaData::create_int);
    ClassDB::bind_static_method("LlmDBSchemaData", D_METHOD("create_real"), &LlmDBSchemaData::create_real);
    ClassDB::bind_static_method("LlmDBSchemaData", D_METHOD("create_text"), &LlmDBSchemaData::create_text);
    ClassDB::bind_static_method("LlmDBSchemaData", D_METHOD("create_blob"), &LlmDBSchemaData::create_blob);
}

LlmDBSchemaData::LlmDBSchemaData() : name {"default_name"},
    type {0}
{}

LlmDBSchemaData::~LlmDBSchemaData() {}

LlmDBSchemaData* LlmDBSchemaData::create_int(String name) {
    LlmDBSchemaData* data = memnew(LlmDBSchemaData());
    data->set_name(name);
    data->set_type(0);
    return data;
}

LlmDBSchemaData* LlmDBSchemaData::create_real(String name) {
    LlmDBSchemaData* data = memnew(LlmDBSchemaData());
    data->set_name(name);
    data->set_type(1);
    return data;
}

LlmDBSchemaData* LlmDBSchemaData::create_text(String name) {
    LlmDBSchemaData* data = memnew(LlmDBSchemaData());
    data->set_name(name);
    data->set_type(2);
    return data;
}

LlmDBSchemaData* LlmDBSchemaData::create_blob(String name) {
    LlmDBSchemaData* data = memnew(LlmDBSchemaData());
    data->set_name(name);
    data->set_type(2);
    return data;
}

String LlmDBSchemaData::get_name() const {
    return name;
};

void LlmDBSchemaData::set_name(const String p_name) {
    name = p_name;
};

int LlmDBSchemaData::get_type() const {
    return type;
}

void LlmDBSchemaData::set_type(const int p_type) {
    type = p_type;
}

void LlmDB::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_schema"), &LlmDB::get_schema);
    ClassDB::bind_method(D_METHOD("set_schema", "p_schema"), &LlmDB::set_schema);
    ClassDB::add_property("LlmDB", PropertyInfo(Variant::ARRAY, "schema", PROPERTY_HINT_ARRAY_TYPE, "LlmDBSchemaData"), "set_schema", "get_schema");
}

LlmDB::LlmDB() {}

LlmDB::~LlmDB() {}

TypedArray<LlmDBSchemaData> LlmDB::get_schema() const {
    return schema;
}

void LlmDB::set_schema(const TypedArray<LlmDBSchemaData> p_schema) {
    schema = p_schema;
}

} // namespace godot
