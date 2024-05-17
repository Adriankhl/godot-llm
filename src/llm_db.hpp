#ifndef LLM_DB_HPP
#define LLM_DB_HPP

#include "gdembedding.hpp"
#include "sqlite3.h"
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/wrapped.hpp>
#include <godot_cpp/core/binder_common.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/typed_array.hpp>

namespace godot {

enum LlmDBSchemaDataType {
    INTEGER = 0,
    REAL = 1,
    TEXT = 2,
    BLOB = 3
};

class LlmDBSchemaData : public Resource {
    GDCLASS(LlmDBSchemaData, Resource)

    private:
        String data_name;
        int data_type; //0: integer, 1: real, 2: text, 3: blob

    protected:
	    static void _bind_methods();

    public:
        LlmDBSchemaData();
        ~LlmDBSchemaData();
        static LlmDBSchemaData* create_int(String data_name);
        static LlmDBSchemaData* create_real(String data_name);
        static LlmDBSchemaData* create_text(String data_name);
        static LlmDBSchemaData* create_blob(String data_name);
        String get_data_name() const;
        void set_data_name(const String p_data_name);
        int get_data_type() const;
        void set_data_type(const int p_data_type);
};

class LlmDB : public GDEmbedding {
    GDCLASS(LlmDB, GDEmbedding)
    private:
        sqlite3* db;
        TypedArray<LlmDBSchemaData> schema;
        String db_dir;
        String db_file;
        String table_name;
        int embedding_size;
        TypedArray<String> separators;
        int chunk_size;
        int chunk_overlap;

        void execute_internal(String statement, int (*callback)(void*,int,char**,char**), void* params);
        String type_int_to_string(int schema_data_type);
        Variant::Type type_int_to_variant(int schema_data_type);

    protected:
	    static void _bind_methods();

    public:
        LlmDB();
        ~LlmDB();
        TypedArray<LlmDBSchemaData> get_schema() const;
        void set_schema(TypedArray<LlmDBSchemaData> p_schema);
        String get_db_dir() const;
        void set_db_dir(const String p_db_dir);
        String get_db_file() const;
        void set_db_file(const String p_db_file);
        String get_table_name() const;
        void set_table_name(const String p_table_name);
        int get_embedding_size() const;
        void set_embedding_size(const int p_embedding_size);
        TypedArray<String> get_separators() const;
        void set_separators(const TypedArray<String> p_separators);
        int get_chunk_size() const;
        void set_chunk_size(const int p_chunk_size);
        int get_chunk_overlap() const;
        void set_chunk_overlap(const int p_chunk_overlap);

        void calibrate_embedding_size();
        void open_db();
        void close_db();
        void execute(String statement);
        void create_llm_tables();
        void drop_table(String p_table_name);
        bool has_table(String p_table_name);
        bool is_table_valid(String p_table_name);
        void insert_meta(Dictionary meta_dict);
        bool has_id(String id, String p_table_name);
};

} // namespace godot

#endif