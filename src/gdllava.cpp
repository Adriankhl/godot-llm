#include "gdllava.h"
#include "conversion.h"
#include "llava_runner.h"
#include "log.h"
#include <godot_cpp/classes/marshalls.hpp>

namespace godot {

void GDLlava::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_model_path"), &GDLlava::get_model_path);
	ClassDB::bind_method(D_METHOD("set_model_path", "p_model_path"), &GDLlava::set_model_path);
    ClassDB::add_property("GDLlava", PropertyInfo(Variant::STRING, "model_path", PROPERTY_HINT_FILE), "set_model_path", "get_model_path");

	ClassDB::bind_method(D_METHOD("get_mmproj_path"), &GDLlava::get_mmproj_path);
	ClassDB::bind_method(D_METHOD("set_mmproj_path", "p_mmproj_path"), &GDLlava::set_mmproj_path);
    ClassDB::add_property("GDLlava", PropertyInfo(Variant::STRING, "mmproj_path", PROPERTY_HINT_FILE), "set_mmproj_path", "get_mmproj_path");

    ClassDB::bind_method(D_METHOD("get_n_ctx"), &GDLlava::get_n_ctx);
    ClassDB::bind_method(D_METHOD("set_n_ctx", "p_n_ctx"), &GDLlava::set_n_ctx);
    ClassDB::add_property("GDLlava", PropertyInfo(Variant::INT, "context_size", PROPERTY_HINT_NONE), "set_n_ctx", "get_n_ctx");

	ClassDB::bind_method(D_METHOD("get_n_predict"), &GDLlava::get_n_predict);
	ClassDB::bind_method(D_METHOD("set_n_predict", "p_n_predict"), &GDLlava::set_n_predict);
    ClassDB::add_property("GDLlava", PropertyInfo(Variant::INT, "n_predict", PROPERTY_HINT_NONE), "set_n_predict", "get_n_predict");

    ClassDB::bind_method(D_METHOD("get_n_batch"), &GDLlava::get_n_batch);
    ClassDB::bind_method(D_METHOD("set_n_batch", "p_n_batch"), &GDLlava::set_n_batch);
    ClassDB::add_property("GDLlava", PropertyInfo(Variant::INT, "n_batch", PROPERTY_HINT_NONE), "set_n_batch", "get_n_batch");

    ClassDB::bind_method(D_METHOD("generate_text_base64", "prompt", "image_base64"), &GDLlava::generate_text_base64);
    ClassDB::bind_method(D_METHOD("generate_text_image", "prompt", "image"), &GDLlava::generate_text_image);

    ADD_SIGNAL(MethodInfo("generate_text_updated", PropertyInfo(Variant::STRING, "new_text")));
    ADD_SIGNAL(MethodInfo("generate_text_finished", PropertyInfo(Variant::STRING, "text")));
}

// A dummy function for instantiating the state of generate_text_thread
void GDLlava::dummy() {}

GDLlava::GDLlava() : params {gpt_params()},
    llava_runner {new LlavaRunner()}
{
    log_set_target(stdout);
    LOG("Instantiate GDLlava mutex\n");
    func_mutex.instantiate();
    generate_text_mutex.instantiate();

    LOG("Instantiate GDLlava thread\n");
    generate_text_thread.instantiate();
    generate_text_thread->start(callable_mp_static(&GDLlava::dummy));
    generate_text_thread->wait_to_finish();

    LOG("Instantiate GDLlava thread -- done\n");
}

GDLlava::~GDLlava() {
    LOG("GDLlava destructor\n");

    //is_started instead of is_alive to properly clean up all threads
    if (generate_text_thread->is_started()) {
        LOG("GDLlava destructor waiting thread to finish\n");
        generate_text_thread->wait_to_finish();
    }
    LOG("GDLlava destructor -- done\n");
}

void GDLlava::_exit_tree() {
    LOG("GDLlava exit tree\n");

    func_mutex->lock();

    LOG("func_mutex locked\n");

    //is_started instead of is_alive to properly clean up all threads
    if (generate_text_thread->is_started()) {
        LOG("Waiting thread to finish\n");
        generate_text_thread->wait_to_finish();
    }
}

String GDLlava::get_model_path() const {
    return string_std_to_gd(params.model);
}

void GDLlava::set_model_path(const String p_model_path) {
    params.model = string_gd_to_std(p_model_path.trim_prefix(String("res://")));
}

String GDLlava::get_mmproj_path() const {
    return string_std_to_gd(params.mmproj);
}

void GDLlava::set_mmproj_path(const String p_mmproj_path) {
    params.mmproj = string_gd_to_std(p_mmproj_path.trim_prefix(String("res://")));
}

int32_t GDLlava::get_n_ctx() const {
    return params.n_ctx;
}

void GDLlava::set_n_ctx(const int32_t p_n_ctx) {
    params.n_ctx = p_n_ctx;
}

int32_t GDLlava::get_n_predict() const {
    return params.n_predict;
}

void GDLlava::set_n_predict(const int32_t p_n_predict) {
    params.n_predict = p_n_predict;
}

int32_t GDLlava::get_n_batch() const {
    return params.n_batch;
}

void GDLlava::set_n_batch(const int32_t p_n_batch) {
    params.n_batch = p_n_batch;
}

String GDLlava::generate_text_common(String prompt, String image_base64) {
    LOG("generate_text_common");

    llava_runner.reset(new LlavaRunner());

    std::string generated_text = llava_runner->llava_generate_text_base64(
        string_gd_to_std(prompt),
        string_gd_to_std(image_base64),
        params,
        [this](std::string s) {
            String new_text = string_std_to_gd(s);
            call_deferred("emit_signal", "generate_text_updated", new_text);
        },
        [this](std::string s) {
            String text {string_std_to_gd(s)};
            call_deferred("emit_signal", "generate_text_finished", text);
        }
    );

    LOG("generate_text_common -- done");

    return string_std_to_gd(generated_text);
}

String GDLlava::generate_text_base64_internal(String prompt, String image_base64) {
    LOG("generate_text_base64_internal");

    String full_generated_text = generate_text_common(prompt, image_base64);

    generate_text_mutex->unlock();
    LOG("generate_text_mutex unlocked\n");

    LOG("generate_text_base64_internal -- done");

    return full_generated_text;
}

String GDLlava::generate_text_base64(String prompt, String image_base64) {
    LOG("generate_text_base64");

    func_mutex->lock();

    if (!generate_text_mutex->try_lock()) {
        LOG("GDLlama is busy\n");
    }

    generate_text_mutex->lock();
    LOG("generate_text_mutex locked\n");

    func_mutex->unlock();

    String full_generated_text = generate_text_base64_internal(prompt, image_base64);

    LOG("generate_text_base64 -- done");

    return full_generated_text;
}

String GDLlava::generate_text_image_internal(String prompt, Image* image) {
    LOG("generate_text_image_internal");

    String image_base64 = Marshalls::get_singleton()->raw_to_base64(image->save_jpg_to_buffer());

    String full_generated_text = generate_text_common(prompt, image_base64);

    generate_text_mutex->unlock();
    LOG("generate_text_mutex unlocked\n");

    LOG("generate_text_image_internal -- done");

    return full_generated_text;
}

String GDLlava::generate_text_image(String prompt, Image* image) {
    LOG("generate_text_image");

    func_mutex->lock();

    if (!generate_text_mutex->try_lock()) {
        LOG("GDLlama is busy\n");
    }

    generate_text_mutex->lock();
    LOG("generate_text_mutex locked\n");

    func_mutex->unlock();

    String full_generated_text = generate_text_image_internal(prompt, image);

    LOG("generate_text_image -- done");

    return full_generated_text;
}

bool GDLlava::is_running() {
    return !generate_text_mutex->try_lock() || generate_text_thread->is_alive();
}

} //namespace godot