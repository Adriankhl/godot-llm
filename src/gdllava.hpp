#ifndef GDLLAVA_HPP
#define GDLLAVA_HPP

#include "common.h"
#include "llava_runner.hpp"
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/mutex.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/thread.hpp>


namespace godot {

class GDLlava : public Node {
    GDCLASS(GDLlava, Node)

    private:
        gpt_params params;
        std::unique_ptr<LlavaRunner> llava_runner;
        Ref<Mutex> generate_text_mutex;
        Ref<Mutex> func_mutex;
        Ref<Thread> generate_text_thread;
        String generate_text_common(String prompt, String image_base64);
        String generate_text_base64_internal(String prompt, String image_base64);
        String generate_text_image_internal(String prompt, Image* image);
        std::function<void(std::string)> glog;
        std::function<void(std::string)> glog_verbose;
        std::string generate_text_buffer;

    protected:
	    static void _bind_methods();
    
    public:
        GDLlava();
        ~GDLlava();
        void _ready() override;
        void _exit_tree() override;
        String get_model_path() const;
        void set_model_path(const String p_model_path);
        String get_mmproj_path() const;
        void set_mmproj_path(const String p_mmproj_path);
        int32_t get_n_ctx() const;
        void set_n_ctx(const int32_t p_n_ctx);
        int32_t get_n_predict() const;
        void set_n_predict(const int32_t p_n_predict);
        float get_temperature() const;
        void set_temperature(const float p_temperature);
        int32_t get_n_threads() const;
        void set_n_threads(const int32_t n_threads);
        int32_t get_n_gpu_layer() const;
        void set_n_gpu_layer(const int32_t p_n_gpu_layers);
        int32_t get_main_gpu() const;
        void set_main_gpu(const int32_t p_main_gpu);
        int32_t get_split_mode();
        void set_split_mode(const int32_t p_split_mode);
        bool get_escape() const;
        void set_escape(const bool p_escape);
        int32_t get_n_batch() const;
        void set_n_batch(const int32_t p_n_batch);
        bool is_running();
        String generate_text_base64(String prompt, String image_base64);
        Error run_generate_text_base64(String prompt, String image_base64);
        String generate_text_image(String prompt, Image* image);
        Error run_generate_text_image(String prompt, Image* image);
        void stop_generate_text();
};

} //namespace godot

#endif //GDLLAVA_H
