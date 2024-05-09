#ifndef GDLLAVA_H
#define GDLLAVA_H

#include "common.h"
#include "llava_runner.h"
#include <godot_cpp/classes/mutex.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/thread.hpp>


namespace godot {

class GDLlava : public Node {
    GDCLASS(GDLlava, Node)

    private:
        static void dummy();
        gpt_params params;
        std::unique_ptr<LlavaRunner> llava_runner;
        Ref<Mutex> generate_text_mutex;
        Ref<Mutex> func_mutex;
        Ref<Thread> generate_text_thread;
        String generate_text_common(String prompt, String image_base64);
        String generate_text_base64_internal(String prompt, String image_base64);

    protected:
	    static void _bind_methods();
    
    public:
        GDLlava();
        ~GDLlava();
        void _exit_tree() override;
        String get_model_path() const;
        void set_model_path(const String p_model_path);
        String get_mmproj_path() const;
        void set_mmproj_path(const String p_mmproj_path);
        int32_t get_n_ctx() const;
        void set_n_ctx(const int32_t p_n_ctx);
        int32_t get_n_predict() const;
        void set_n_predict(const int32_t p_n_predict);
        int32_t get_n_batch() const;
        void set_n_batch(const int32_t p_n_batch);
        bool is_running();
        String generate_text_base64(String prompt, String image_base64);
};

} //namespace godot

#endif //GDLLAVA_H