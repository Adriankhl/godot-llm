#include "llm_db.hpp"
#include "sqlite3.h"
#include "sqlite-vec.h"
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

    ClassDB::bind_method(D_METHOD("get_db_dir"), &LlmDB::get_db_dir);
    ClassDB::bind_method(D_METHOD("set_db_dir", "p_db_dir"), &LlmDB::set_db_dir);
    ClassDB::add_property("LlmDB", PropertyInfo(Variant::STRING, "db_dir", PROPERTY_HINT_GLOBAL_FILE), "set_db_dir", "get_db_dir");

    ClassDB::bind_method(D_METHOD("get_db_file"), &LlmDB::get_db_file);
    ClassDB::bind_method(D_METHOD("set_db_file", "p_db_file"), &LlmDB::set_db_file);
    ClassDB::add_property("LlmDB", PropertyInfo(Variant::STRING, "db_file", PROPERTY_HINT_GLOBAL_FILE), "set_db_file", "get_db_file");

    ClassDB::bind_method(D_METHOD("open_db"), &LlmDB::open_db);
    ClassDB::bind_method(D_METHOD("close_db"), &LlmDB::close_db);
}

LlmDB::LlmDB() : db_dir {"."},
    db_file("llm.db")
{
    int rc = SQLITE_OK;
    rc = sqlite3_auto_extension((void (*)())sqlite3_vec_init);
    if (rc != SQLITE_OK) {
        UtilityFunctions::printerr("Unable to load sqlite3_vec extension");
    }
}

LlmDB::~LlmDB() {}

TypedArray<LlmDBSchemaData> LlmDB::get_schema() const {
    return schema;
}

void LlmDB::set_schema(const TypedArray<LlmDBSchemaData> p_schema) {
    schema = p_schema;
}

String LlmDB::get_db_dir() const {
    return db_dir;
}

void LlmDB::set_db_dir(const String p_db_dir) {
    db_dir = p_db_dir;
}

String LlmDB::get_db_file() const {
    return db_file;
}

void LlmDB::set_db_file(const String p_db_file) {
    db_file = p_db_file;
}

void LlmDB::open_db() {
    if (db != nullptr) {
        UtilityFunctions::printerr("Cannot open_db when db is already openned");
        return;
    }
    int rc = SQLITE_OK;

    String db_path = db_dir + "/" + db_file;

    rc = sqlite3_open_v2(db_path.utf8().get_data(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_URI, nullptr);

    if (rc != SQLITE_OK) {
        UtilityFunctions::printerr("Failed to open database: " + String::utf8(sqlite3_errmsg(db)));
    }
}

void LlmDB::close_db() {
    if (db != nullptr) {
        int rc = SQLITE_OK;
        rc = sqlite3_close_v2(db);
        if (rc != SQLITE_OK) {
            UtilityFunctions::printerr("Failed to close database");
        }
    } else {
        UtilityFunctions::printerr("Cannot close_db when no db is opened");
    }
}

} // namespace godot
