#ifndef GDEMBEDDING_HPP
#define GDEMBEDDING_HPP

#include "common.h"
#include "embedding_runner.hpp"
#include <godot_cpp/classes/mutex.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/thread.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>


namespace godot {

class GDEmbedding : public Node {
    GDCLASS(GDEmbedding, Node)

    private:
        gpt_params params;
        std::unique_ptr<EmbeddingRunner> embedding_runner;
        Ref<Mutex> compute_embedding_mutex;
        Ref<Mutex> func_mutex;
        Ref<Thread> compute_embedding_thread;
        PackedFloat32Array compute_embedding_internal(String prompt);
        float similarity_cos_string_internal(String s1, String s2);
        std::function<void(std::string)> glog;
        std::function<void(std::string)> glog_verbose;

    protected:
	    static void _bind_methods();
    
    public:
        GDEmbedding();
        ~GDEmbedding();

        void _ready() override;
        void _exit_tree() override;
        String get_model_path() const;
        void set_model_path(const String p_model_path);
        int32_t get_n_threads() const;
        void set_n_threads(const int32_t n_threads);
        int32_t get_n_gpu_layer() const;
        void set_n_gpu_layer(const int32_t p_n_gpu_layers);
        int32_t get_main_gpu() const;
        void set_main_gpu(const int32_t p_main_gpu);
        int32_t get_split_mode();
        void set_split_mode(const int32_t p_split_mode);
        int32_t get_n_batch() const;
        void set_n_batch(const int32_t p_n_batch);
        bool is_running();
        PackedFloat32Array compute_embedding(String prompt);
        Error run_compute_embedding(String prompt);
        float similarity_cos_array(PackedFloat32Array array1, PackedFloat32Array array2);
        float similarity_cos_string(String s1, String s2);
        Error run_similarity_cos_string(String s1, String s2);
        int get_n_embd();
};

} //namespace godot

#endif //GDEMBEDDING_H
