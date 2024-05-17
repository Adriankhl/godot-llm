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
    ClassDB::bind_method(D_METHOD("create_llm_tables"), &LlmDB::create_llm_tables);
    ClassDB::bind_method(D_METHOD("execute", "statement"), &LlmDB::execute);
    ClassDB::bind_method(D_METHOD("is_table_exist", "p_table_name"), &LlmDB::is_table_exist);
    ClassDB::bind_method(D_METHOD("is_table_valid", "p_table_name"), &LlmDB::is_table_valid);
}

LlmDB::LlmDB() : db_dir {"."},
    db_file {"llm.db"},
    table_name {"llm_table"},
    n_embd {384},
    schema {TypedArray<LlmDBSchemaData> (LlmDBSchemaData::create_text("id"))}
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

void LlmDB::set_schema(TypedArray<LlmDBSchemaData> p_schema) {
    bool is_id_valid = true;
    int col_to_remove = -1;


    if (p_schema.size() != 0) {
        // Remove any id column that is not the first row
        for (int i = 1; i < p_schema.size(); i++) {
            LlmDBSchemaData* sd = Object::cast_to<LlmDBSchemaData>(p_schema[i]);
            if (sd->get_name() == "id") {
                UtilityFunctions::printerr("Column " + String::num_int64(i) + " error: Id column must be the first column (0)");
                col_to_remove = i;
            }
        }
        if (col_to_remove != -1) {
            UtilityFunctions::printerr("Removing column " + String::num(col_to_remove));
            p_schema.remove_at(col_to_remove);
        }

        LlmDBSchemaData* sd0 = Object::cast_to<LlmDBSchemaData>(p_schema[0]);
        if (sd0->get_name() == "id" && sd0->get_type() != 2) {
            UtilityFunctions::printerr("Id column should be TEXT type, removing");
            p_schema.remove_at(0);
        }

        // Get again since it might get removed
        sd0 = Object::cast_to<LlmDBSchemaData>(p_schema[0]);
        if (sd0->get_name() != "id") {
            UtilityFunctions::printerr("First column is not id");
            is_id_valid = false;
        }
    }

    if (p_schema.size() == 0 || !is_id_valid) {
        UtilityFunctions::printerr("Creating id as the first column");
        p_schema.push_front(LlmDBSchemaData::create_text("id"));
    }
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
    rc = sqlite3_exec(db, statement.utf8().get_data(), callback, params, &errmsg);
    if (rc != SQLITE_OK) {
        UtilityFunctions::printerr("LlmDB execute fail: " + String(errmsg));
    }
}

void LlmDB::execute(String statement) {
    execute_internal(statement, print_all_callback, nullptr);
}

String LlmDB::type_int_to_string(int schema_data_type) {
    switch (schema_data_type) {
        case LlmDBSchemaDataType::INTEGER:
            return  "INT";
        case LlmDBSchemaDataType::REAL:
            return  "REAL";
        case LlmDBSchemaDataType::TEXT:
            return "TEXT";
        case LlmDBSchemaDataType::BLOB:
            return "";
        default:
            UtilityFunctions::printerr("Wrong schema type: " + String::num_int64(schema_data_type));
            return "";
    }
}

void LlmDB::create_llm_tables() {
    UtilityFunctions::print_verbose("create_llm_tables: " + table_name);
    String statement = "CREATE TABLE IF NOT EXISTS " + table_name + " (";
    for (int i = 0; i < schema.size(); i++) {
        LlmDBSchemaData* sd = Object::cast_to<LlmDBSchemaData>(schema[i]);
        statement += " '" + sd->get_name() + "' ";
        statement += type_int_to_string(sd->get_type());
        statement += ", ";
    }
    statement += "llm_text TEXT, ";

    statement += String("embedding") + " float[" + String::num_int64(n_embd)+ "]";

    statement += ")";

    UtilityFunctions::print_verbose("Create table statement: " + statement);

    execute(statement);

    UtilityFunctions::print_verbose("create_table " + table_name + " -- done");

    // Also create a table to store meta data
    String meta_table_name = table_name + "_meta";

    String statement_meta = "CREATE TABLE IF NOT EXISTS " + meta_table_name + " (";
    for (int i = 0; i < schema.size(); i++) {
        LlmDBSchemaData* sd = Object::cast_to<LlmDBSchemaData>(schema[i]);
        statement_meta += " '" + sd->get_name() + "' ";
        statement_meta += type_int_to_string(sd->get_type());
        
        if (i != schema.size() - 1) {
            statement_meta += ", ";
        }
    }
    statement_meta += ")";

    UtilityFunctions::print_verbose("Create meta table statement: " + statement_meta);

    execute(statement_meta);

    UtilityFunctions::print_verbose("create_table " + table_name + " -- done");

}

bool LlmDB::is_table_exist(String p_table_name) {
    UtilityFunctions::print_verbose("is_table_exist");

    sqlite3_stmt *stmt;

    String statement = "SELECT count(name) FROM sqlite_master WHERE type='table' AND name='" + p_table_name + "';";
    UtilityFunctions::print_verbose("statement: " + statement);

    int rc = sqlite3_prepare_v2(db, statement.utf8().get_data(), -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        UtilityFunctions::printerr("Error: " + String::utf8(sqlite3_errmsg(db)));
        return false;
    }

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW) {
        UtilityFunctions::printerr("Error: not a row");
        return false;
    }

    int num = sqlite3_column_int64(stmt, 0);

    sqlite3_finalize(stmt);

    UtilityFunctions::print_verbose("is_table_exist -- done");

    if (num == 0) {
        return false;
    }

    return true;
}


bool LlmDB::is_table_valid(String p_table_name) {
    UtilityFunctions::print_verbose("is_table_valid");

    sqlite3_stmt *stmt;

    String statement = "PRAGMA table_info(" + p_table_name + ");";
    UtilityFunctions::print_verbose("statement: " + statement);

    int rc = sqlite3_prepare_v2(db, statement.utf8().get_data(), -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        UtilityFunctions::printerr("Error: " + String::utf8(sqlite3_errmsg(db)));
        return false;
    }

    for (int i = 0; i < schema.size(); i++) {
        rc = sqlite3_step(stmt);
        if (rc != SQLITE_ROW) {
            UtilityFunctions::printerr("Error: not a row");
            return false;
        }
        String name = String::utf8((char *) sqlite3_column_text(stmt, 1));
        String type = String::utf8((char *) sqlite3_column_text(stmt, 2));

        LlmDBSchemaData* sd = Object::cast_to<LlmDBSchemaData>(schema[i]);

        if (name != sd->get_name()) {
            UtilityFunctions::printerr("Column name wrong, table : " + name + ", schema: " + sd->get_name());
            return false;
        }

        if (type != type_int_to_string(sd->get_type())) {
            UtilityFunctions::printerr("Column type wrong, table : " + type + ", schema: " + type_int_to_string(sd->get_type()));
            return false;

        }
    }

    sqlite3_finalize(stmt);

    UtilityFunctions::print_verbose("is_table_valid -- done");

    return true;
}

} // namespace godot
