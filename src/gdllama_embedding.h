#ifndef GDLLAMA_EMBEDDING_H
#define GDLLAMA_EMBEDDING_H

#include "common.h"
#include "llama_embedding_runner.h"
#include <godot_cpp/classes/mutex.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/thread.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>
#include <vector>


namespace godot {

class GDLlamaEmbedding : public Node {
    GDCLASS(GDLlamaEmbedding, Node)

    private:
        static void dummy();
        gpt_params params;
        std::unique_ptr<LlamaEmbeddingRunner> llama_embedding_runner;
        Ref<Mutex> compute_embedding_mutex;
        Ref<Mutex> func_mutex;
        Ref<Thread> compute_embedding_thread;
        PackedFloat32Array compute_embedding_internal(String prompt);
        float similarity_cos_string_internal(String s1, String s2);

    protected:
	    static void _bind_methods();
    
    public:
        GDLlamaEmbedding();
        ~GDLlamaEmbedding();
        static std::vector<float> float32_array_to_vec(PackedFloat32Array array);
        static PackedFloat32Array float32_vec_to_array(std::vector<float> vec);
        void _exit_tree() override;
        String get_model_path() const;
        void set_model_path(const String p_model_path);
        int32_t get_n_batch() const;
        void set_n_batch(const int32_t p_n_batch);
        bool is_running();
        PackedFloat32Array compute_embedding(String prompt);
        Error run_compute_embedding(String prompt);
        float similarity_cos_array(PackedFloat32Array array1, PackedFloat32Array array2);
        float similarity_cos_string(String s1, String s2);
        Error run_similarity_cos_string(String s1, String s2);
};

} //namespace godot

#endif //GDLLAMA_EMBEDDING_H