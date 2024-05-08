#ifndef GDLLAMA_EMBEDDING_H
#define GDLLAMA_EMBEDDING_H

#include "common.h"
#include "llama_embedding_runner.h"
#include <godot_cpp/classes/node.hpp>


namespace godot {

class GDLlamaEmbedding : public Node {
    GDCLASS(GDLlamaEmbedding, Node)

    private:
        gpt_params params;
        std::unique_ptr<LlamaEmbeddingRunner> llama_embedding_runner;

    protected:
	    static void _bind_methods();
    
    public:
        GDLlamaEmbedding();
        ~GDLlamaEmbedding();
        String get_model_path() const;
        void set_model_path(const String p_model_path);
        int32_t get_n_batch() const;
        void set_n_batch(const int32_t p_n_batch);
};

} //namespace godot

#endif //GDLLAMA_EMBEDDING_H