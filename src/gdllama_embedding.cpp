#include "gdllama_embedding.h"
#include "llama_embedding_runner.h"
namespace godot {

void GDLlamaEmbedding::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_model_path"), &GDLlamaEmbedding::get_model_path);
	ClassDB::bind_method(D_METHOD("set_model_path", "p_model_path"), &GDLlamaEmbedding::set_model_path);
    ClassDB::add_property("GDLlamaEmbedding", PropertyInfo(Variant::STRING, "model_path", PROPERTY_HINT_FILE), "set_model_path", "get_model_path");

   	ClassDB::bind_method(D_METHOD("get_n_batch"), &GDLlamaEmbedding::get_n_batch);
	ClassDB::bind_method(D_METHOD("set_n_batch", "p_n_batch"), &GDLlamaEmbedding::set_n_batch);
    ClassDB::add_property("GDLlamaEmbedding", PropertyInfo(Variant::INT, "n_batch", PROPERTY_HINT_NONE), "set_n_batch", "get_n_batch");
}

GDLlamaEmbedding::GDLlamaEmbedding() : params {gpt_params()},
    llama_embedding_runner {new LlamaEmbeddingRunner()} 
{ }

GDLlamaEmbedding::~GDLlamaEmbedding() {}

String GDLlamaEmbedding::get_model_path() const {
    return String(params.model.c_str());
}

void GDLlamaEmbedding::set_model_path(const String p_model_path) {
    params.model = std::string(p_model_path.trim_prefix(String("res://")).utf8().get_data());
}

int32_t GDLlamaEmbedding::get_n_batch() const {
    return params.n_batch;
}

void GDLlamaEmbedding::set_n_batch(const int32_t p_n_batch) {
    params.n_batch = p_n_batch;
}

} //namespace godot