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


class LlmDBSchemaData : public Resource {
    GDCLASS(LlmDBSchemaData, Resource)

    private:
        String name;
        int type; //0: integer, 1: real, 2: text, 3: blob

    protected:
	    static void _bind_methods();

    public:
        LlmDBSchemaData();
        ~LlmDBSchemaData();
        static LlmDBSchemaData* create_int(String name);
        static LlmDBSchemaData* create_real(String name);
        static LlmDBSchemaData* create_text(String name);
        static LlmDBSchemaData* create_blob(String name);
        String get_name() const;
        void set_name(const String p_name);
        int get_type() const;
        void set_type(const int p_type);
};

class LlmDB : public GDEmbedding {
    GDCLASS(LlmDB, GDEmbedding)
    private:
        sqlite3* db;
        TypedArray<LlmDBSchemaData> schema;

    protected:
	    static void _bind_methods();

    public:
        LlmDB();
        ~LlmDB();
        TypedArray<LlmDBSchemaData> get_schema() const;
        void set_schema(const TypedArray<LlmDBSchemaData> p_schema);
};

} // namespace godot

#endif