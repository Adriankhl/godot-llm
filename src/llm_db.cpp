#include "llm_db.hpp"
#include "sqlite3.h"
#include "sqlite-vec.h"
#include <gdextension_interface.h>
#include <godot_cpp/classes/global_constants.hpp>
#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/core/binder_common.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/memory.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/string.hpp>
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

    ClassDB::bind_method(D_METHOD("get_table_name"), &LlmDB::get_table_name);
    ClassDB::bind_method(D_METHOD("set_table_name", "p_table_name"), &LlmDB::set_table_name);
    ClassDB::add_property("LlmDB", PropertyInfo(Variant::STRING, "table_name", PROPERTY_HINT_GLOBAL_FILE), "set_table_name", "get_table_name");

    ClassDB::bind_method(D_METHOD("open_db"), &LlmDB::open_db);
    ClassDB::bind_method(D_METHOD("close_db"), &LlmDB::close_db);
    ClassDB::bind_method(D_METHOD("create_table"), &LlmDB::create_table);
    ClassDB::bind_method(D_METHOD("execute", "statement"), &LlmDB::execute);
}

LlmDB::LlmDB() : db_dir {"."},
    db_file {"llm.db"},
    table_name {"llm_table"}
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

String LlmDB::get_table_name() const {
    return table_name;
}

void LlmDB::set_table_name(const String p_table_name) {
    table_name = p_table_name;
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
        db = nullptr;
        if (rc != SQLITE_OK) {
            UtilityFunctions::printerr("Failed to close database");
        }
    } else {
        UtilityFunctions::printerr("Cannot close_db when no db is opened");
    }
}

int print_all_callback(void *unused, int count, char **data, char **columns) {

    UtilityFunctions::print_verbose("print_all_callback called");

    UtilityFunctions::print_verbose("There are " + String::num_int64(count) + " column(s)");

    for (int idx = 0; idx < count; idx++) {
        UtilityFunctions::print_verbose("The data in column" + String::utf8(columns[idx]) + " is: " + String::utf8(data[idx]));
    }

    return 0;
}

void LlmDB::execute_internal(String statement, int (*callback)(void*,int,char**,char**), void* params) {
    int rc = SQLITE_OK;
    char* errmsg;
    sqlite3_exec(db, statement.utf8().get_data(), callback, params, &errmsg);
    if (rc != SQLITE_OK) {
        UtilityFunctions::printerr("LlmDB execute fail: " + String(errmsg));
    }
}

void LlmDB::execute(String statement) {
    execute_internal(statement, print_all_callback, nullptr);
}

void LlmDB::create_table() {
    UtilityFunctions::print_verbose("create_table " + table_name);
    String statement = "CREATE TABLE IF NOT EXISTS " + table_name + " (";
    for (int i = 0; i < schema.size(); i++) {
        LlmDBSchemaData* sd = Object::cast_to<LlmDBSchemaData>(schema[i]);
        statement += " '" + sd->get_name() + "' ";
        switch (sd->get_type()) {
            case 0:
                statement += "INT";
                break;
            case 1:
                statement += "REAL";
                break;
            case 2:
                statement += "TEXT";
                break;
            case 3:
                statement += "";
                break;
            default:
                UtilityFunctions::printerr("Wrong schema type: " + String::num_int64(sd->get_type()));
        }

        if (i != schema.size() - 1) {
            statement += ", ";
        }
    }

    statement += ")";

    UtilityFunctions::print_verbose("Create table statement: " + statement);

    execute(statement);

    UtilityFunctions::print_verbose("create_table " + table_name + " -- done");
}

} // namespace godot
