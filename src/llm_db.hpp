#ifndef LLM_DB_HPP
#define LLM_DB_HPP

#include "gdembedding.hpp"
#include "sqlite3.h"
#include <functional>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/wrapped.hpp>
#include <godot_cpp/core/binder_common.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/typed_array.hpp>
#include <queue>

namespace godot {

enum LlmDBMetaDataType {
    INTEGER = 0,
    REAL = 1,
    TEXT = 2,
    BLOB = 3
};

class LlmDBMetaData : public Resource {
    GDCLASS(LlmDBMetaData, Resource)

    private:
        String data_name;
        int data_type; //0: integer, 1: real, 2: text, 3: blob

    protected:
	    static void _bind_methods();

    public:
        LlmDBMetaData();
        ~LlmDBMetaData();
        static LlmDBMetaData* create_int(String data_name);
        static LlmDBMetaData* create_real(String data_name);
        static LlmDBMetaData* create_text(String data_name);
        static LlmDBMetaData* create_blob(String data_name);
        String get_data_name() const;
        void set_data_name(const String p_data_name);
        int get_data_type() const;
        void set_data_type(const int p_data_type);
};

class LlmDB : public GDEmbedding {
    GDCLASS(LlmDB, GDEmbedding)
    private:
        sqlite3* db;
        TypedArray<LlmDBMetaData> meta;
        String db_dir;
        String db_file;
        String table_name;
        int embedding_size;
        PackedStringArray absolute_separators;
        PackedStringArray chunk_separators;
        int chunk_size;
        int chunk_overlap;
        Ref<Mutex> store_text_mutex;
        Ref<Thread> store_text_thread;
        std::queue<std::function<void()>> store_text_queue;
        Ref<Thread> retrieve_text_thread;
        std::queue<std::function<void()>> retrieve_text_queue;
        Ref<Mutex> func_mutex;

        void execute_internal(String statement, int (*callback)(void*,int,char**,char**), void* params);
        String type_int_to_string(int meta_data_type);
        Variant::Type type_int_to_variant(int meta_data_type);
        PackedStringArray absolute_split_text(String text, int index);
        PackedStringArray chunk_split_text(String text, int index);
        void insert_text_by_id(String id, String text);
        void insert_text_by_meta(Dictionary meta_dict, String text);
        void store_text_process();
        void retrieve_text_process();

    protected:
	    static void _bind_methods();

    public:
        LlmDB();
        ~LlmDB();
        void _exit_tree() override;
        TypedArray<LlmDBMetaData> get_meta() const;
        void set_meta(TypedArray<LlmDBMetaData> p_meta);
        String get_db_dir() const;
        void set_db_dir(const String p_db_dir);
        String get_db_file() const;
        void set_db_file(const String p_db_file);
        String get_table_name() const;
        void set_table_name(const String p_table_name);
        int get_embedding_size() const;
        void set_embedding_size(const int p_embedding_size);
        PackedStringArray get_absolute_separators() const;
        void set_absolute_separators(const PackedStringArray p_absolute_separators);
        PackedStringArray get_chunk_separators() const;
        void set_chunk_separators(const PackedStringArray p_chunk_separators);
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
        void drop_llm_tables(String p_table_name);
        bool has_table(String p_table_name);
        bool is_table_valid(String p_table_name);
        void store_meta(Dictionary meta_dict);
        bool has_id(String id, String p_table_name);
        PackedStringArray split_text(String text);
        void store_text_by_id(String id, String text);
        Error run_store_text_by_id(String id, String text);
        void store_text_by_meta(Dictionary meta_dict, String text);
        Error run_store_text_by_meta(Dictionary meta_dict, String text);
        PackedStringArray retrieve_similar_texts(String text, String where, int n_results);
        Error run_retrieve_similar_texts(String text, String where, int n_results);
};

} // namespace godot

#endif