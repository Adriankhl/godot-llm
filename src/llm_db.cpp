#include "llm_db.hpp"
#include "gdembedding.hpp"
#include "sqlite3.h"
#include "sqlite-vec.h"
#include <cstring>
#include <gdextension_interface.h>
#include <godot_cpp/classes/global_constants.hpp>
#include <godot_cpp/classes/json.hpp>
#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/classes/worker_thread_pool.hpp>
#include <godot_cpp/core/binder_common.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/memory.hpp>
#include <godot_cpp/variant/callable_method_pointer.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/typed_array.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/variant.hpp>
#include <queue>

namespace godot {

void LlmDBMetaData::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_data_name"), &LlmDBMetaData::get_data_name);
    ClassDB::bind_method(D_METHOD("set_data_name", "p_data_name"), &LlmDBMetaData::set_data_name);
    ClassDB::add_property("LlmDBMetaData", PropertyInfo(Variant::STRING, "data_name", PROPERTY_HINT_NONE), "set_data_name", "get_data_name");

    ClassDB::bind_method(D_METHOD("get_data_type"), &LlmDBMetaData::get_data_type);
    ClassDB::bind_method(D_METHOD("set_data_type", "p_data_type"), &LlmDBMetaData::set_data_type);
    ClassDB::add_property("LlmDBMetaData", PropertyInfo(Variant::INT, "data_type", PROPERTY_HINT_ENUM, "INTEGER, REAL, TEXT, BLOB"), "set_data_type", "get_data_type");

    ClassDB::bind_static_method("LlmDBMetaData", D_METHOD("create_int", "data_name"), &LlmDBMetaData::create_int);
    ClassDB::bind_static_method("LlmDBMetaData", D_METHOD("create_real", "data_name"), &LlmDBMetaData::create_real);
    ClassDB::bind_static_method("LlmDBMetaData", D_METHOD("create_text", "data_name"), &LlmDBMetaData::create_text);
    ClassDB::bind_static_method("LlmDBMetaData", D_METHOD("create_blob", "data_name"), &LlmDBMetaData::create_blob);
}

LlmDBMetaData::LlmDBMetaData() : data_name {"default_name"},
    data_type {0}
{ }

LlmDBMetaData::~LlmDBMetaData() {}

LlmDBMetaData* LlmDBMetaData::create_int(String data_name) {
    LlmDBMetaData* data = memnew(LlmDBMetaData());
    data->set_data_name(data_name);
    data->set_data_type(0);
    return data;
}

LlmDBMetaData* LlmDBMetaData::create_real(String data_name) {
    LlmDBMetaData* data = memnew(LlmDBMetaData());
    data->set_data_name(data_name);
    data->set_data_type(1);
    return data;
}

LlmDBMetaData* LlmDBMetaData::create_text(String data_name) {
    LlmDBMetaData* data = memnew(LlmDBMetaData());
    data->set_data_name(data_name);
    data->set_data_type(2);
    return data;
}

LlmDBMetaData* LlmDBMetaData::create_blob(String data_name) {
    LlmDBMetaData* data = memnew(LlmDBMetaData());
    data->set_data_name(data_name);
    data->set_data_type(2);
    return data;
}

String LlmDBMetaData::get_data_name() const {
    return data_name;
};

void LlmDBMetaData::set_data_name(const String p_data_name) {
    data_name = p_data_name;
};

int LlmDBMetaData::get_data_type() const {
    return data_type;
}

void LlmDBMetaData::set_data_type(const int p_data_type) {
    data_type = p_data_type;
}

void LlmDB::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_meta"), &LlmDB::get_meta);
    ClassDB::bind_method(D_METHOD("set_meta", "p_meta"), &LlmDB::set_meta);
    ClassDB::add_property("LlmDB", PropertyInfo(Variant::ARRAY, "meta", PROPERTY_HINT_ARRAY_TYPE, vformat("%s/%s:%s", Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE, "LlmDBMetaData")), "set_meta", "get_meta");

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

    ClassDB::bind_method(D_METHOD("get_absolute_separators"), &LlmDB::get_absolute_separators);
    ClassDB::bind_method(D_METHOD("set_absolute_separators", "p_absolute_separators"), &LlmDB::set_absolute_separators);
    ClassDB::add_property("LlmDB", PropertyInfo(Variant::ARRAY, "absolute_separators", PROPERTY_HINT_ARRAY_TYPE, "String"), "set_absolute_separators", "get_absolute_separators");

    ClassDB::bind_method(D_METHOD("get_chunk_separators"), &LlmDB::get_chunk_separators);
    ClassDB::bind_method(D_METHOD("set_chunk_separators", "p_chunk_separators"), &LlmDB::set_chunk_separators);
    ClassDB::add_property("LlmDB", PropertyInfo(Variant::ARRAY, "chunk_separators", PROPERTY_HINT_ARRAY_TYPE, "String"), "set_chunk_separators", "get_chunk_separators");

    ClassDB::bind_method(D_METHOD("get_chunk_size"), &LlmDB::get_chunk_size);
    ClassDB::bind_method(D_METHOD("set_chunk_size", "p_chunk_size"), &LlmDB::set_chunk_size);
    ClassDB::add_property("LlmDB", PropertyInfo(Variant::INT, "chunk_size", PROPERTY_HINT_GLOBAL_FILE), "set_chunk_size", "get_chunk_size");

    ClassDB::bind_method(D_METHOD("get_chunk_overlap"), &LlmDB::get_chunk_overlap);
    ClassDB::bind_method(D_METHOD("set_chunk_overlap", "p_chunk_overlap"), &LlmDB::set_chunk_overlap);
    ClassDB::add_property("LlmDB", PropertyInfo(Variant::INT, "chunk_overlap", PROPERTY_HINT_GLOBAL_FILE), "set_chunk_overlap", "get_chunk_overlap");

    ClassDB::bind_method(D_METHOD("calibrate_embedding_size"), &LlmDB::calibrate_embedding_size);
    ClassDB::bind_method(D_METHOD("open_db"), &LlmDB::open_db);
    ClassDB::bind_method(D_METHOD("close_db"), &LlmDB::close_db);
    ClassDB::bind_method(D_METHOD("execute", "statement"), &LlmDB::execute);
    ClassDB::bind_method(D_METHOD("create_llm_tables"), &LlmDB::create_llm_tables);
    ClassDB::bind_method(D_METHOD("drop_table", "p_table_name"), &LlmDB::drop_table);
    ClassDB::bind_method(D_METHOD("drop_llm_tables", "p_table_name"), &LlmDB::drop_llm_tables);
    ClassDB::bind_method(D_METHOD("has_table", "p_table_name"), &LlmDB::has_table);
    ClassDB::bind_method(D_METHOD("is_table_valid", "p_table_name"), &LlmDB::is_table_valid);
    ClassDB::bind_method(D_METHOD("store_meta", "meta_dict"), &LlmDB::store_meta);
    ClassDB::bind_method(D_METHOD("has_id", "id", "p_table_name"), &LlmDB::has_id);
    ClassDB::bind_method(D_METHOD("split_text", "text"), &LlmDB::split_text);
    ClassDB::bind_method(D_METHOD("store_text_by_id", "id", "text"), &LlmDB::store_text_by_id);
    ClassDB::bind_method(D_METHOD("run_store_text_by_id", "id", "text"), &LlmDB::run_store_text_by_id);
    ClassDB::bind_method(D_METHOD("store_text_by_meta", "meta_dict", "text"), &LlmDB::store_text_by_meta);
    ClassDB::bind_method(D_METHOD("run_store_text_by_meta", "meta_dict", "text"), &LlmDB::run_store_text_by_meta);
    ClassDB::bind_method(D_METHOD("retrieve_similar_texts", "text", "where", "n_results"), &LlmDB::retrieve_similar_texts);
    ClassDB::bind_method(D_METHOD("run_retrieve_similar_texts", "text", "where", "n_results"), &LlmDB::run_retrieve_similar_texts);

    ADD_SIGNAL(MethodInfo("retrieve_similar_text_finished", PropertyInfo(Variant::PACKED_STRING_ARRAY, "array")));
}

// A dummy function for instantiating the state of generate_text_thread
void LlmDB::dummy() {}

LlmDB::LlmDB() : db_dir {"."},
    db {nullptr},
    meta {TypedArray<LlmDBMetaData>()},
    db_file {"llm.db"},
    table_name {"llm_table"},
    embedding_size {384},
    chunk_size {100},
    chunk_overlap {20},
    absolute_separators {PackedStringArray()},
    chunk_separators {PackedStringArray()},
    store_text_queue {std::queue<std::function<void()>>()},
    retrieve_text_queue {std::queue<std::function<void()>>()}
{
    UtilityFunctions::print_verbose("Instantiating LlmDB store_text_mutex");
    store_text_mutex.instantiate();

    UtilityFunctions::print_verbose("Instantiate GDLlava mutex");
    func_mutex.instantiate();

    UtilityFunctions::print_verbose("Instantiating LlmDB store_text_thread");
    store_text_thread.instantiate();

    store_text_thread->start(callable_mp_static(&LlmDB::dummy));
    store_text_thread->wait_to_finish();

    UtilityFunctions::print_verbose("Instantiating LlmDB retrieve_text_thread");
    retrieve_text_thread.instantiate();

    retrieve_text_thread->start(callable_mp_static(&LlmDB::dummy));
    retrieve_text_thread->wait_to_finish();

    meta.append(LlmDBMetaData::create_text("id"));

    absolute_separators.append("\n\n");
    absolute_separators.append("\n");

    chunk_separators.append(".");
    chunk_separators.append(",");
    chunk_separators.append(String::utf8("\uff0c"));
    chunk_separators.append(String::utf8("\u3001"));
    chunk_separators.append(String::utf8("\uff0e"));
    chunk_separators.append(String::utf8("\u3002"));
    chunk_separators.append("\u200b");
    chunk_separators.append(" ");
    chunk_separators.append("");

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

    func_mutex->try_lock();

    if (store_text_thread->is_started()) {
        UtilityFunctions::print_verbose("Waiting thread to finish");
        store_text_thread->wait_to_finish();
    }
}

void LlmDB::_exit_tree() {
    UtilityFunctions::print_verbose("LlmDB exit tree");

    GDEmbedding::_exit_tree();

    func_mutex->lock();

    UtilityFunctions::print_verbose("func_mutex locked");

    //is_started instead of is_alive to properly clean up all threads
    if (store_text_thread->is_started()) {
        UtilityFunctions::print_verbose("Waiting thread to finish");
        store_text_thread->wait_to_finish();
    }

    UtilityFunctions::print_verbose("LlmDB exit tree -- done");
}

TypedArray<LlmDBMetaData> LlmDB::get_meta() const {
    return meta;
}

void LlmDB::set_meta(TypedArray<LlmDBMetaData> p_meta) {
    bool is_id_valid = true;
    int col_to_remove = -1;


    if (p_meta.size() != 0) {
        // Remove any id column that is not the first row
        for (int i = 1; i < p_meta.size(); i++) {
            UtilityFunctions::print_verbose("Checking meta data " + String::num_int64(i));
            if (p_meta[i].get_type() != Variant::NIL) {
                UtilityFunctions::print_verbose("Correct resource type");
                LlmDBMetaData* sd = Object::cast_to<LlmDBMetaData>(p_meta[i]);
                if (sd->get_data_name() == "id") {
                    UtilityFunctions::printerr("Column " + String::num_int64(i) + " error: Id column must be the first column (0)");
                    col_to_remove = i;
                }
            }
        }
        if (col_to_remove != -1) {
            UtilityFunctions::printerr("Removing column " + String::num(col_to_remove));
            p_meta.remove_at(col_to_remove);
        }

        LlmDBMetaData* sd0 = Object::cast_to<LlmDBMetaData>(p_meta[0]);
        if (sd0->get_data_name() == "id" && sd0->get_data_type() != LlmDBMetaDataType::TEXT) {
            UtilityFunctions::printerr("Id column should be TEXT type, removing");
            p_meta.remove_at(0);
        }

        // Get again since it might get removed
        sd0 = Object::cast_to<LlmDBMetaData>(p_meta[0]);
        if (sd0->get_data_name() != "id") {
            UtilityFunctions::printerr("First column is not id");
            is_id_valid = false;
        }
    }

    if (p_meta.size() == 0 || !is_id_valid) {
        UtilityFunctions::printerr("Creating id as the first column");
        p_meta.push_front(LlmDBMetaData::create_text("id"));
    }
    meta = p_meta;
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

PackedStringArray LlmDB::get_absolute_separators() const {
    return absolute_separators;
};

void LlmDB::set_absolute_separators(const PackedStringArray p_absolute_separators) {
    absolute_separators = p_absolute_separators;
};

PackedStringArray LlmDB::get_chunk_separators() const {
    return chunk_separators;
};

void LlmDB::set_chunk_separators(const PackedStringArray p_chunk_separators) {
    chunk_separators = p_chunk_separators;
};

int LlmDB::get_chunk_size() const {
    return chunk_size;
}

void LlmDB::set_chunk_size(const int p_chunk_size) {
    chunk_size = p_chunk_size;
}

int LlmDB::get_chunk_overlap() const {
    return chunk_overlap;
}

void LlmDB::set_chunk_overlap(const int p_chunk_overlap) {
    chunk_overlap = p_chunk_overlap;
}

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

String LlmDB::type_int_to_string(int meta_data_type) {
    switch (meta_data_type) {
        case LlmDBMetaDataType::INTEGER:
            return  "INT";
        case LlmDBMetaDataType::REAL:
            return  "REAL";
        case LlmDBMetaDataType::TEXT:
            return "TEXT";
        case LlmDBMetaDataType::BLOB:
            return "";
        default: {
            UtilityFunctions::printerr("Wrong meta type: " + String::num_int64(meta_data_type));
            return "";
        }
    }
}

Variant::Type LlmDB::type_int_to_variant(int meta_data_type) {
    switch (meta_data_type) {
        case LlmDBMetaDataType::INTEGER:
            return  Variant::INT;
        case LlmDBMetaDataType::REAL:
            return  Variant::FLOAT;
        case LlmDBMetaDataType::TEXT:
            return Variant::STRING;
        case LlmDBMetaDataType::BLOB:
            return Variant::VARIANT_MAX;
        default: {
            UtilityFunctions::printerr("Wrong meta type for variant: " + String::num_int64(meta_data_type));
            return Variant::NIL;
        }
    }
}

void LlmDB::create_llm_tables() {
    UtilityFunctions::print_verbose("create_llm_tables: " + table_name);
    String statement = "CREATE TABLE IF NOT EXISTS " + table_name + " (";
    for (int i = 0; i < meta.size(); i++) {
        LlmDBMetaData* sd = Object::cast_to<LlmDBMetaData>(meta[i]);
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
    for (int i = 0; i < meta.size(); i++) {
        LlmDBMetaData* sd = Object::cast_to<LlmDBMetaData>(meta[i]);
        statement_meta += " '" + sd->get_data_name() + "' ";
        statement_meta += type_int_to_string(sd->get_data_type());
        if (i == 0) {
            statement_meta += " PRIMARY KEY";
        }

        statement_meta += ", ";
    }

    statement_meta = statement_meta.trim_suffix(", ");
    statement_meta += ") WITHOUT ROWID;";

    UtilityFunctions::print_verbose("Create meta table statement: " + statement_meta);

    execute(statement_meta);

    String statement_virtual = "CREATE VIRTUAL TABLE IF NOT EXISTS " + table_name + "_virtual USING vec0(embedding float[" + String::num_int64(embedding_size) + "]);";

    UtilityFunctions::print_verbose("Create virtual table statement: " + statement_virtual);

    execute(statement_virtual);

    UtilityFunctions::print_verbose("create_table " + table_name + " -- done");

}

void LlmDB::drop_table(String p_table_name) {
    String statement = "DROP TABLE  " + p_table_name + ";";
    UtilityFunctions::print_verbose("Drop table statement: " + statement);
    execute(statement);
}

void LlmDB::drop_llm_tables(String p_table_name) {
    drop_table(p_table_name);
    drop_table(p_table_name + "_virtual");
    drop_table(p_table_name + "_meta");
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
        UtilityFunctions::printerr("Error: " + String::utf8(sqlite3_errmsg(db)));
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

    for (int i = 0; i < meta.size(); i++) {
        rc = sqlite3_step(stmt);
        if (rc != SQLITE_ROW) {
            UtilityFunctions::printerr("Error: " + String::utf8(sqlite3_errmsg(db)));
            return false;
        }
        String name = String::utf8((char *) sqlite3_column_text(stmt, 1));
        String type = String::utf8((char *) sqlite3_column_text(stmt, 2));

        LlmDBMetaData* sd = Object::cast_to<LlmDBMetaData>(meta[i]);

        if (name != sd->get_data_name()) {
            UtilityFunctions::printerr("Column name wrong, table : " + name + ", meta: " + sd->get_data_name());
            return false;
        }

        if (type != type_int_to_string(sd->get_data_type())) {
            UtilityFunctions::printerr("Column type wrong, table : " + type + ", meta: " + type_int_to_string(sd->get_data_type()));
            return false;

        }
    }

    sqlite3_finalize(stmt);

    UtilityFunctions::print_verbose("is_table_valid -- done");

    return true;
}

void LlmDB::store_meta(Dictionary meta_dict) {
    UtilityFunctions::print_verbose("insert_meta");

    //Check id
    if (!meta_dict.has("id") || meta_dict.get("id", nullptr).get_type() != Variant::STRING) {
        UtilityFunctions::printerr("insert_meta error: no id or id is not a string");
        return;
    }

    String statement_1 = "INSERT OR REPLACE INTO " + table_name + "_meta";
    String statement_2 = "(";
    String statement_3 = "(";
    Dictionary p_meta_dict = meta_dict.duplicate(false);
    PackedStringArray array_bind {PackedStringArray()};
    for (int i = 0; i < meta.size(); i++) {
        LlmDBMetaData* sd = Object::cast_to<LlmDBMetaData>(meta[i]);
        if(p_meta_dict.has(sd->get_data_name())) {
            Variant v = p_meta_dict.get(sd->get_data_name(), nullptr);
            if (v.get_type() != type_int_to_variant(sd->get_data_type())) {
                UtilityFunctions::printerr("Wrong data type for key " + sd->get_data_name() + " : " + v.get_type_name(v.get_type()) + " instead of " + sd->get_data_type());
            }

            p_meta_dict.erase(sd->get_data_name());

            switch (sd->get_data_type()) {
                case LlmDBMetaDataType::INTEGER: {
                    statement_2 += sd->get_data_name() + ", ";
                    int k = v;
                    statement_3 += String::num_int64(k) + ", ";
                    break;
                }
                case LlmDBMetaDataType::REAL: {
                    statement_2 += sd->get_data_name() + ", ";
                    float f = v;
                    statement_3 += String::num_real(f) + ", ";
                    break;
                }
                case LlmDBMetaDataType::TEXT: {
                    statement_2 += sd->get_data_name() + ", ";
                    String s = v;
                    statement_3 += "?, ";
                    array_bind.append(s);
                    break;
                }
                case LlmDBMetaDataType::BLOB: {
                    statement_2 += sd->get_data_name() + ", ";
                    String s = v.stringify();
                    statement_3 += "?, ";
                    array_bind.append(s);
                    break;
                }
            }
        }
    }

    if (!p_meta_dict.is_empty()) {
        UtilityFunctions::printerr("Some meta data has incorrect key or incorrect format, key: " + JSON::stringify(p_meta_dict.keys()));
    }

    statement_2 = statement_2.trim_suffix(", ") + ")";
    statement_3 = statement_3.trim_suffix(", ") + ")";
    String statement = statement_1 + " " + statement_2 + " VALUES " + statement_3 + ";";
    UtilityFunctions::print_verbose("store_meta statement: " + statement);

    int rc = SQLITE_OK;
    sqlite3_stmt *stmt;
    rc = sqlite3_prepare_v2(db, statement.utf8().get_data(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        UtilityFunctions::printerr("Failed to perpare statement");
    }
    for (int i = 0; i < array_bind.size(); i++) {
        sqlite3_bind_text(stmt, i+1, array_bind[i].utf8().get_data(), -1, SQLITE_TRANSIENT);
    }
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        UtilityFunctions::printerr("Error: " + String::utf8(sqlite3_errmsg(db)));
    }
    sqlite3_finalize(stmt);

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
        UtilityFunctions::printerr("Error: " + String::utf8(sqlite3_errmsg(db)));
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

PackedStringArray LlmDB::absolute_split_text(String text, int index) {
    if ((index >= absolute_separators.size()) || (text.length() <= chunk_size)) {
        PackedStringArray array {PackedStringArray()};
        array.append(text);
        return array;
    } else {
        PackedStringArray array {PackedStringArray()};
        PackedStringArray subarray = text.split(absolute_separators[index], false);
        for (String s : subarray) {
            array.append_array(absolute_split_text(s, index + 1));
        }

        return array;
    }
}

PackedStringArray LlmDB::chunk_split_text(String text, int index) {
    if (index >= chunk_separators.size()) {
        UtilityFunctions::printerr("Failed to split text in chunk");
        PackedStringArray array {PackedStringArray()};
        array.append(text);
        return array;
    } else {
        PackedStringArray array = text.split(chunk_separators[index], false);
        bool is_chunk_fit = true;
        for (String s : array) {
            if (s.length() > chunk_size) {
                is_chunk_fit = false;
            }
        }

        if (!is_chunk_fit) {
            return chunk_split_text(text, index + 1);
        } else {
            PackedStringArray chunk_array {PackedStringArray()};

            String separator = chunk_separators[index];
            int separator_size = separator.length();

            UtilityFunctions::print_verbose("Using separator '" + separator + "'");
            UtilityFunctions::print_verbose("Separator size: " + String::num_int64(separator_size));
            UtilityFunctions::print_verbose("array size: " + String::num_int64(array.size()));

            int end_index = 0;
            String s = array[end_index];

            while (end_index < array.size()) {
                end_index += 1;
                while ((end_index < array.size()) && (s.length() + separator_size + array[end_index].length() < chunk_size)) {
                    s += separator + array[end_index];
                    end_index += 1;
                }

                UtilityFunctions::print_verbose("Chunk: " + s);
                UtilityFunctions::print_verbose("Chunk length: " + String::num_int64(s.length()));
                chunk_array.append(s);

                // Add overlap
                if (end_index < array.size()) {
                    s = array[end_index];
                    int start_index = end_index - 1;
                    while ((start_index >= 0) && (array[start_index].length() + separator_size + s.length() < chunk_overlap)) {
                        s = array[start_index] + separator + s;
                        start_index -= 1;
                    }
                    UtilityFunctions::print_verbose("Overlap: " + s);
                    UtilityFunctions::print_verbose("Overlap length: " + String::num_int64(s.length()));
                }
            }

            return chunk_array;
        }
    }
}


PackedStringArray LlmDB::split_text(String text) {
    if (text.is_empty()) {
        return PackedStringArray();
    }

    if (absolute_separators.is_empty()) {
        UtilityFunctions::printerr("Empty absolute_separators");
    }

    PackedStringArray separated_array_a = absolute_split_text(text, 0);

    if (chunk_separators.is_empty()) {
        UtilityFunctions::printerr("Empty chunk_separators");
    }

    PackedStringArray array {PackedStringArray()};

    for (String s : separated_array_a) {
        array.append_array(chunk_split_text(s, 0));
    }

    return array;
};

void LlmDB::insert_text_by_id(String id, String text) {
    UtilityFunctions::print_verbose("insert_text_by_id");
    PackedFloat32Array embedding = compute_embedding(text);

    String statement = "INSERT INTO " + table_name + " (";

    for (int i = 0; i < meta.size(); i++) {
        LlmDBMetaData* sd = Object::cast_to<LlmDBMetaData>(meta[i]);
        statement += sd->get_data_name() + ", ";
    }

    statement += "llm_text, embedding) VALUES (?, ";
    
    for (int i = 1; i < meta.size(); i++) {
        LlmDBMetaData* sd = Object::cast_to<LlmDBMetaData>(meta[i]);
        statement += "(SELECT " + sd->get_data_name() + " FROM " + table_name + "_meta" + " WHERE id=?), ";
    }
    
    statement += "?, '[";

    for (float f : embedding) {
        statement += String::num_real(f) + ", ";
    }

    statement = statement.trim_suffix(", ");
    statement += "]')";

    UtilityFunctions::print_verbose("insert_text statement: " + statement);
    int rc = SQLITE_OK;
    sqlite3_stmt *stmt;
    rc = sqlite3_prepare_v2(db, statement.utf8().get_data(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        UtilityFunctions::printerr("Failed to perpare statement");
    }

    for (int i = 0; i < meta.size(); i++) {
        sqlite3_bind_text(stmt, i + 1, id.utf8().get_data(), -1, SQLITE_TRANSIENT);
    }
    sqlite3_bind_text(stmt, meta.size() + 1, text.utf8().get_data(), -1, SQLITE_TRANSIENT);
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        UtilityFunctions::printerr("Error: " + String::utf8(sqlite3_errmsg(db)));
    }
    sqlite3_finalize(stmt);

    String statement_virtual = "INSERT INTO " + table_name + "_virtual (rowid, embedding)" + " SELECT rowid, embedding FROM " + table_name + " WHERE rowid=last_insert_rowid()";
    UtilityFunctions::print_verbose("insert_text virtual statement: " + statement_virtual);
    execute(statement_virtual);

    UtilityFunctions::print_verbose("insert_text_by_id -- done");
}

void LlmDB::store_text_by_id(String id, String text) {
    UtilityFunctions::print_verbose("store_text_by_id");

    store_text_mutex->lock();
    UtilityFunctions::print_verbose("store_text_mutex locked");

    PackedStringArray text_array = split_text(text);

    for (String s : text_array) {
        insert_text_by_id(id, s);
    }

    store_text_mutex->unlock();
    UtilityFunctions::print_verbose("store_text_mutex unlocked");

    UtilityFunctions::print_verbose("store_text_by_id -- done");
};

void LlmDB::store_text_process() {
    UtilityFunctions::print_verbose("store_text_process");
    while (!store_text_queue.empty()) {
        std::function<void()> f = store_text_queue.front();

        f();

        store_text_queue.pop();
    }
    UtilityFunctions::print_verbose("store_text_process -- done");
}

void LlmDB::run_store_text_by_id(String id, String text) {
    func_mutex->lock();

    store_text_queue.push([this, text, id](){ store_text_by_id(id, text); });

    if (store_text_thread->is_started()) {
        if (!store_text_thread->is_alive()) {
            store_text_thread->wait_to_finish();
            store_text_thread.instantiate();
            store_text_thread->start(callable_mp(this, &LlmDB::store_text_process));
        }
    } else {
        store_text_thread.instantiate();
        store_text_thread->start(callable_mp(this, &LlmDB::store_text_process));
    }

    func_mutex->unlock();
};

void LlmDB::insert_text_by_meta(Dictionary meta_dict, String text) {
    UtilityFunctions::print_verbose("insert_text_by_meta");

    PackedFloat32Array embedding = compute_embedding(text);

    String statement_1 = "INSERT INTO " + table_name;
    String statement_2 = "(";
    String statement_3 = "(";
    Dictionary p_meta_dict = meta_dict.duplicate(false);
    PackedStringArray array_bind {PackedStringArray()};
    for (int i = 0; i < meta.size(); i++) {
        LlmDBMetaData* sd = Object::cast_to<LlmDBMetaData>(meta[i]);
        if(p_meta_dict.has(sd->get_data_name())) {
            Variant v = p_meta_dict.get(sd->get_data_name(), nullptr);
            if (v.get_type() != type_int_to_variant(sd->get_data_type())) {
                UtilityFunctions::printerr("Wrong data type for key " + sd->get_data_name() + " : " + v.get_type_name(v.get_type()) + " instead of " + sd->get_data_type());
            }

            p_meta_dict.erase(sd->get_data_name());

            switch (sd->get_data_type()) {
                case LlmDBMetaDataType::INTEGER: {
                    statement_2 += sd->get_data_name() + ", ";
                    int k = v;
                    statement_3 += String::num_int64(k) + ", ";
                    break;
                }
                case LlmDBMetaDataType::REAL: {
                    statement_2 += sd->get_data_name() + ", ";
                    float f = v;
                    statement_3 += String::num_real(f) + ", ";
                    break;
                }
                case LlmDBMetaDataType::TEXT: {
                    statement_2 += sd->get_data_name() + ", ";
                    String s = v;
                    statement_3 += "?, ";
                    array_bind.append(s);
                    break;
                }
                case LlmDBMetaDataType::BLOB: {
                    statement_2 += sd->get_data_name() + ", ";
                    String s = v.stringify();
                    statement_3 += "?, ";
                    array_bind.append(s);
                    break;
                }
            }
        }
    }

    if (!p_meta_dict.is_empty()) {
        UtilityFunctions::printerr("Some meta data has incorrect key or incorrect format, key: " + JSON::stringify(p_meta_dict.keys()));
    }

    statement_2 = statement_2 + "llm_text, embedding)";
    statement_3 = statement_3 + "?, '[";
    array_bind.append(text);

    for (float f : embedding) {
        statement_3 += String::num_real(f) + ", ";
    }

    statement_3 = statement_3.trim_suffix(", ");
    statement_3 += "]')";

    String statement = statement_1 + " " + statement_2 + " VALUES " + statement_3 + ";";
    UtilityFunctions::print_verbose("insert_text_by_meta statement: " + statement);

    int rc = SQLITE_OK;
    sqlite3_stmt *stmt;
    rc = sqlite3_prepare_v2(db, statement.utf8().get_data(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        UtilityFunctions::printerr("Failed to perpare statement");
    }
    for (int i = 0; i < array_bind.size(); i++) {
        sqlite3_bind_text(stmt, i+1, array_bind[i].utf8().get_data(), -1, SQLITE_TRANSIENT);
    }
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        UtilityFunctions::printerr("Error: " + String::utf8(sqlite3_errmsg(db)));
    }
    sqlite3_finalize(stmt);

    String statement_virtual = "INSERT INTO " + table_name + "_virtual (rowid, embedding)" + " SELECT rowid, embedding FROM " + table_name + " WHERE rowid=last_insert_rowid()";
    UtilityFunctions::print_verbose("insert_text virtual statement: " + statement_virtual);
    execute(statement_virtual);

    UtilityFunctions::print_verbose("insert_text_by_meta -- done");
}

void LlmDB::store_text_by_meta(Dictionary meta_dict, String text) {
    UtilityFunctions::print_verbose("store_text_by_meta");

    store_text_mutex->lock();
    UtilityFunctions::print_verbose("store_text_mutex locked");

    PackedStringArray text_array = split_text(text);

    for (String s : text_array) {
        insert_text_by_meta(meta_dict, s);
    }

    store_text_mutex->unlock();
    UtilityFunctions::print_verbose("store_text_mutex unlocked");

    UtilityFunctions::print_verbose("store_text_by_meta -- done");
};

Error LlmDB::run_store_text_by_meta(Dictionary meta_dict, String text) {
    func_mutex->lock();

    store_text_queue.push([this, text, meta_dict](){ store_text_by_meta(meta_dict, text); });

    Error error = OK;

    if (store_text_thread->is_started()) {
        if (!store_text_thread->is_alive()) {
            store_text_thread->wait_to_finish();
            store_text_thread.instantiate();
            error = store_text_thread->start(callable_mp(this, &LlmDB::store_text_process));
        }
    } else {
        store_text_thread.instantiate();
        error = store_text_thread->start(callable_mp(this, &LlmDB::store_text_process));
    }

    func_mutex->unlock();

    return error;
};

PackedStringArray LlmDB::retrieve_similar_texts(String text, String where, int n_results) {
    UtilityFunctions::print_verbose("retrieve_similar_texts");
    PackedFloat32Array embedding = compute_embedding(text);

    String statement_filter = "SELECT rowid FROM " + table_name;
    if (!where.is_empty()) {
        statement_filter += " WHERE " + where;
    }


    String statement_virtual = "SELECT rowid FROM " + table_name + "_virtual WHERE embedding MATCH '[";

    for (float f : embedding) {
        statement_virtual += String::num_real(f) + ", ";
    }

    statement_virtual = statement_virtual.trim_suffix(", ");
    statement_virtual += "]' ";

    statement_virtual += "AND rowid in (" + statement_filter + ") ";

    statement_virtual += "ORDER BY distance LIMIT " + String::num_int64(n_results);

    String statement_final = "SELECT llm_text FROM " + table_name + " WHERE rowid IN (" + statement_virtual + ");";

    UtilityFunctions::print_verbose("retrieve_text_array statement: " + statement_final);

    PackedStringArray array {PackedStringArray()};

    int rc = SQLITE_OK;
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, statement_final.utf8().get_data(), -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        UtilityFunctions::printerr("Failed to prepare statement");
        return array;
    }

    while (true) {
        rc = sqlite3_step(stmt);
        if(rc == SQLITE_DONE) break;
        if (rc != SQLITE_ROW) {
            UtilityFunctions::printerr("Error: " + String::utf8(sqlite3_errmsg(db)));
            return array;
        }
        const char* c = (char*) sqlite3_column_text(stmt, 0);

        String retrieved_text = String::utf8(c);
        UtilityFunctions::print_verbose("retrieved text: " + retrieved_text);
        array.append(retrieved_text);
    }

    sqlite3_finalize(stmt);

    UtilityFunctions::print_verbose("retrieve_similar_texts -- done");

    return array;
}

void LlmDB::retrieve_text_process() {
    UtilityFunctions::print_verbose("retrieve_text_process");
    while (!retrieve_text_queue.empty()) {
        std::function<void()> f = retrieve_text_queue.front();

        f();

        retrieve_text_queue.pop();
    }
    UtilityFunctions::print_verbose("retrieve_text_process -- done");
}

Error LlmDB::run_retrieve_similar_texts(String text, String where, int n_results) {
    UtilityFunctions::print_verbose("run_retrieve_similar_texts");

    func_mutex->lock();

    retrieve_text_queue.push(
        [this, text, where, n_results](){ 
            PackedStringArray array = retrieve_similar_texts(text, where, n_results); 
            call_deferred("emit_signal", "retrieve_similar_text_finished", array);
        }
    );

    if (retrieve_text_thread->is_started()) {
        retrieve_text_thread->wait_to_finish();
    }

    retrieve_text_thread.instantiate();
    Error error = retrieve_text_thread->start(callable_mp(this, &LlmDB::retrieve_text_process));

    func_mutex->unlock();

    UtilityFunctions::print_verbose("run_retrieve_similar_texts -- done");

    return error;
};

} // namespace godot
