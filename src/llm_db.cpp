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
#include <godot_cpp/variant/variant.hpp>


namespace godot {

void LlmDBSchemaData::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_data_name"), &LlmDBSchemaData::get_data_name);
    ClassDB::bind_method(D_METHOD("set_data_name", "p_data_name"), &LlmDBSchemaData::set_data_name);
    ClassDB::add_property("LlmDBSchemaData", PropertyInfo(Variant::STRING, "data_name", PROPERTY_HINT_NONE), "set_data_name", "get_data_name");

    ClassDB::bind_method(D_METHOD("get_data_type"), &LlmDBSchemaData::get_data_type);
    ClassDB::bind_method(D_METHOD("set_data_type", "p_data_type"), &LlmDBSchemaData::set_data_type);
    ClassDB::add_property("LlmDBSchemaData", PropertyInfo(Variant::INT, "data_type", PROPERTY_HINT_ENUM, "INTEGER, REAL, TEXT, BLOB"), "set_data_type", "get_data_type");

    ClassDB::bind_static_method("LlmDBSchemaData", D_METHOD("create_int", "data_name"), &LlmDBSchemaData::create_int);
    ClassDB::bind_static_method("LlmDBSchemaData", D_METHOD("create_real", "data_name"), &LlmDBSchemaData::create_real);
    ClassDB::bind_static_method("LlmDBSchemaData", D_METHOD("create_text", "data_name"), &LlmDBSchemaData::create_text);
    ClassDB::bind_static_method("LlmDBSchemaData", D_METHOD("create_blob", "data_name"), &LlmDBSchemaData::create_blob);
}

LlmDBSchemaData::LlmDBSchemaData() : data_name {"default_name"},
    data_type {0}
{}

LlmDBSchemaData::~LlmDBSchemaData() {}

LlmDBSchemaData* LlmDBSchemaData::create_int(String data_name) {
    LlmDBSchemaData* data = memnew(LlmDBSchemaData());
    data->set_data_name(data_name);
    data->set_data_type(0);
    return data;
}

LlmDBSchemaData* LlmDBSchemaData::create_real(String data_name) {
    LlmDBSchemaData* data = memnew(LlmDBSchemaData());
    data->set_data_name(data_name);
    data->set_data_type(1);
    return data;
}

LlmDBSchemaData* LlmDBSchemaData::create_text(String data_name) {
    LlmDBSchemaData* data = memnew(LlmDBSchemaData());
    data->set_data_name(data_name);
    data->set_data_type(2);
    return data;
}

LlmDBSchemaData* LlmDBSchemaData::create_blob(String data_name) {
    LlmDBSchemaData* data = memnew(LlmDBSchemaData());
    data->set_data_name(data_name);
    data->set_data_type(2);
    return data;
}

String LlmDBSchemaData::get_data_name() const {
    return data_name;
};

void LlmDBSchemaData::set_data_name(const String p_data_name) {
    data_name = p_data_name;
};

int LlmDBSchemaData::get_data_type() const {
    return data_type;
}

void LlmDBSchemaData::set_data_type(const int p_data_type) {
    data_type = p_data_type;
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

    ClassDB::bind_method(D_METHOD("get_embedding_size"), &LlmDB::get_embedding_size);
    ClassDB::bind_method(D_METHOD("set_embedding_size", "p_embedding_size"), &LlmDB::set_embedding_size);
    ClassDB::add_property("LlmDB", PropertyInfo(Variant::INT, "embedding_size", PROPERTY_HINT_GLOBAL_FILE), "set_embedding_size", "get_embedding_size");

    ClassDB::bind_method(D_METHOD("get_separators"), &LlmDB::get_separators);
    ClassDB::bind_method(D_METHOD("set_separators", "p_separators"), &LlmDB::set_separators);
    ClassDB::add_property("LlmDB", PropertyInfo(Variant::ARRAY, "separators", PROPERTY_HINT_ARRAY_TYPE, "String"), "set_separators", "get_separators");


    ClassDB::bind_method(D_METHOD("calibrate_embedding_size"), &LlmDB::calibrate_embedding_size);
    ClassDB::bind_method(D_METHOD("open_db"), &LlmDB::open_db);
    ClassDB::bind_method(D_METHOD("close_db"), &LlmDB::close_db);
    ClassDB::bind_method(D_METHOD("execute", "statement"), &LlmDB::execute);
    ClassDB::bind_method(D_METHOD("create_llm_tables"), &LlmDB::create_llm_tables);
    ClassDB::bind_method(D_METHOD("drop_table", "p_table_name"), &LlmDB::drop_table);
    ClassDB::bind_method(D_METHOD("has_table", "p_table_name"), &LlmDB::has_table);
    ClassDB::bind_method(D_METHOD("is_table_valid", "p_table_name"), &LlmDB::is_table_valid);
    ClassDB::bind_method(D_METHOD("insert_meta", "meta_dict"), &LlmDB::insert_meta);
    ClassDB::bind_method(D_METHOD("has_id", "id", "p_table_name"), &LlmDB::has_id);
}

LlmDB::LlmDB() : db_dir {"."},
    db {nullptr},
    db_file {"llm.db"},
    table_name {"llm_table"},
    embedding_size {384}
{
    schema.append(LlmDBSchemaData::create_text("id"));

    separators.append("\n\n");
    separators.append("\n");
    separators.append(" ");
    separators.append(".");
    separators.append(",");
    separators.append(String::utf8("\u200b"));
    separators.append(String::utf8("\uff0c"));
    separators.append(String::utf8("\u3001"));
    separators.append(String::utf8("\uff0e"));
    separators.append(String::utf8("\u3002"));
    separators.append(String::utf8(""));

    int rc = SQLITE_OK;
    rc = sqlite3_auto_extension((void (*)())sqlite3_vec_init);
    if (rc != SQLITE_OK) {
        UtilityFunctions::printerr("Unable to load sqlite3_vec extension");
    }
}

LlmDB::~LlmDB() {
    if (db != nullptr) {
        close_db();
    }
}

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
            if (sd->get_data_name() == "id") {
                UtilityFunctions::printerr("Column " + String::num_int64(i) + " error: Id column must be the first column (0)");
                col_to_remove = i;
            }
        }
        if (col_to_remove != -1) {
            UtilityFunctions::printerr("Removing column " + String::num(col_to_remove));
            p_schema.remove_at(col_to_remove);
        }

        LlmDBSchemaData* sd0 = Object::cast_to<LlmDBSchemaData>(p_schema[0]);
        if (sd0->get_data_name() == "id" && sd0->get_data_type() != 2) {
            UtilityFunctions::printerr("Id column should be TEXT type, removing");
            p_schema.remove_at(0);
        }

        // Get again since it might get removed
        sd0 = Object::cast_to<LlmDBSchemaData>(p_schema[0]);
        if (sd0->get_data_name() != "id") {
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

int LlmDB::get_embedding_size() const {
    return embedding_size;
}

void LlmDB::set_embedding_size(const int p_embedding_size) {
    embedding_size = p_embedding_size;
}

void LlmDB::calibrate_embedding_size() {
    embedding_size = get_n_embd();
}

TypedArray<String> LlmDB::get_separators() const {
    return separators;
};

void LlmDB::set_separators(const TypedArray<String> p_separators) {
    separators = p_separators;
};


void LlmDB::open_db() {
    UtilityFunctions::print_verbose("open_db");
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

    UtilityFunctions::print_verbose("open_db -- done");
}

void LlmDB::close_db() {
    UtilityFunctions::print_verbose("close_db");
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
    UtilityFunctions::print_verbose("close_db -- done");
}

int print_all_callback(void *unused, int count, char **data, char **columns) {

    UtilityFunctions::print_verbose("print_all_callback called");

    UtilityFunctions::print_verbose("There are " + String::num_int64(count) + " column(s)");

    for (int idx = 0; idx < count; idx++) {
        UtilityFunctions::print_verbose("The data in column " + String::utf8(columns[idx]) + " is: " + String::utf8(data[idx]));
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
        default: {
            UtilityFunctions::printerr("Wrong schema type: " + String::num_int64(schema_data_type));
            return "";
        }
    }
}

Variant::Type LlmDB::type_int_to_variant(int schema_data_type) {
    switch (schema_data_type) {
        case LlmDBSchemaDataType::INTEGER:
            return  Variant::INT;
        case LlmDBSchemaDataType::REAL:
            return  Variant::FLOAT;
        case LlmDBSchemaDataType::TEXT:
            return Variant::STRING;
        case LlmDBSchemaDataType::BLOB:
            return Variant::VARIANT_MAX;
        default: {
            UtilityFunctions::printerr("Wrong schema type for variant: " + String::num_int64(schema_data_type));
            return Variant::NIL;
        }
    }
}

void LlmDB::create_llm_tables() {
    UtilityFunctions::print_verbose("create_llm_tables: " + table_name);
    String statement = "CREATE TABLE IF NOT EXISTS " + table_name + " (";
    for (int i = 0; i < schema.size(); i++) {
        LlmDBSchemaData* sd = Object::cast_to<LlmDBSchemaData>(schema[i]);
        statement += " '" + sd->get_data_name() + "' ";
        statement += type_int_to_string(sd->get_data_type());
        statement += ", ";
    }
    statement += "llm_text TEXT, ";

    statement += String("embedding") + " float[" + String::num_int64(embedding_size)+ "]";

    statement += ");";

    UtilityFunctions::print_verbose("Create table statement: " + statement);

    execute(statement);

    UtilityFunctions::print_verbose("create_table " + table_name + " -- done");

    // Also create a table to store meta data
    String meta_table_name = table_name + "_meta";

    String statement_meta = "CREATE TABLE IF NOT EXISTS " + meta_table_name + " (";
    for (int i = 0; i < schema.size(); i++) {
        LlmDBSchemaData* sd = Object::cast_to<LlmDBSchemaData>(schema[i]);
        statement_meta += " '" + sd->get_data_name() + "' ";
        statement_meta += type_int_to_string(sd->get_data_type());
        if (i == 0) {
            statement_meta += " PRIMARY KEY";
        }
        
        if (i != schema.size() - 1) {
            statement_meta += ", ";
        }
    }
    statement_meta += ") WITHOUT ROWID;";

    UtilityFunctions::print_verbose("Create meta table statement: " + statement_meta);

    execute(statement_meta);

    UtilityFunctions::print_verbose("create_table " + table_name + " -- done");

}

void LlmDB::drop_table(String p_table_name) {
    String statement = "DROP TABLE  " + p_table_name + ";";
    UtilityFunctions::print_verbose("Drop table statement: " + statement);
    execute(statement);
}

bool LlmDB::has_table(String p_table_name) {
    UtilityFunctions::print_verbose("has_table");

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

    UtilityFunctions::print_verbose("has_table -- done");

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

        if (name != sd->get_data_name()) {
            UtilityFunctions::printerr("Column name wrong, table : " + name + ", schema: " + sd->get_data_name());
            return false;
        }

        if (type != type_int_to_string(sd->get_data_type())) {
            UtilityFunctions::printerr("Column type wrong, table : " + type + ", schema: " + type_int_to_string(sd->get_data_type()));
            return false;

        }
    }

    sqlite3_finalize(stmt);

    UtilityFunctions::print_verbose("is_table_valid -- done");

    return true;
}

void LlmDB::insert_meta(Dictionary meta_dict) {
    UtilityFunctions::print_verbose("insert_meta");

    String statement_1 = "INSERT OR REPLACE INTO " + table_name + "_meta";
    String statement_2 = "(";
    String statement_3 = "(";
    Dictionary p_meta_dict = meta_dict.duplicate(false);
    for (int i = 0; i < schema.size(); i++) {
        LlmDBSchemaData* sd = Object::cast_to<LlmDBSchemaData>(schema[i]);
        if(p_meta_dict.has(sd->get_data_name())) {
            Variant v = p_meta_dict.get(sd->get_data_name(), nullptr);
            if (v.get_type() != type_int_to_variant(sd->get_data_type())) {
                UtilityFunctions::printerr("Wrong data type for key " + sd->get_data_name() + " : " + v.get_type_name(v.get_type()) + " instead of " + sd->get_data_type());
            }

            switch (sd->get_data_type()) {
                case LlmDBSchemaDataType::INTEGER: {
                    statement_2 += sd->get_data_name() + ", ";
                    int k = v;
                    statement_3 += String::num_int64(k) + ", ";
                    break;
                }
                case LlmDBSchemaDataType::REAL: {
                    statement_2 += sd->get_data_name() + ", ";
                    float f = v;
                    statement_3 += String::num_real(f) + ", ";
                    break;
                }
                case LlmDBSchemaDataType::TEXT: {
                    statement_2 += sd->get_data_name() + ", ";
                    String s = v;
                    statement_3 += "'" + s + "', ";
                    break;
                }
                case LlmDBSchemaDataType::BLOB: {
                    statement_2 += sd->get_data_name() + ", ";
                    String s = v.stringify();
                    statement_3 += "'" + s + "', ";
                    break;
                }
            }
        }
    }
    statement_2 = statement_2.trim_suffix(", ") + ")";
    statement_3 = statement_3.trim_suffix(", ") + ")";
    String statement = statement_1 + " " + statement_2 + " VALUES " + statement_3 + ";";
    UtilityFunctions::print_verbose("insert_meta statement: " + statement);
    execute(statement);

    UtilityFunctions::print_verbose("insert_meta -- done");
}

bool LlmDB::has_id(String id, String p_table_name) {
    UtilityFunctions::print_verbose("has_id");

    sqlite3_stmt *stmt;

    String statement = "SELECT COUNT(1) FROM " + p_table_name + " WHERE " + p_table_name + ".id" " = '" + id + "';";
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

    UtilityFunctions::print_verbose("has_id -- done");

    if (num == 0) {
        return false;
    }

    return true;
}

} // namespace godot
