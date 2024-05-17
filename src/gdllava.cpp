#include "gdllava.hpp"
#include "conversion.hpp"
#include "llava_runner.hpp"
#include <godot_cpp/classes/marshalls.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

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

    ClassDB::bind_method(D_METHOD("get_temperature"), &GDLlava::get_temperature);
    ClassDB::bind_method(D_METHOD("set_temperature", "p_temperature"), &GDLlava::set_temperature);
    ClassDB::add_property("GDLlava", PropertyInfo(Variant::FLOAT, "temperature", PROPERTY_HINT_NONE), "set_temperature", "get_temperature");

    ClassDB::bind_method(D_METHOD("get_n_threads"), &GDLlava::get_n_threads);
    ClassDB::bind_method(D_METHOD("set_n_threads", "p_n_threads"), &GDLlava::set_n_threads);
    ClassDB::add_property("GDLlava", PropertyInfo(Variant::INT, "n_threads", PROPERTY_HINT_NONE), "set_n_threads", "get_n_threads");

	ClassDB::bind_method(D_METHOD("get_escape"), &GDLlava::get_escape);
	ClassDB::bind_method(D_METHOD("set_escape", "p_escape"), &GDLlava::set_escape);
    ClassDB::add_property("GDLlava", PropertyInfo(Variant::BOOL, "escape", PROPERTY_HINT_NONE), "set_escape", "get_escape");

    ClassDB::bind_method(D_METHOD("get_n_batch"), &GDLlava::get_n_batch);
    ClassDB::bind_method(D_METHOD("set_n_batch", "p_n_batch"), &GDLlava::set_n_batch);
    ClassDB::add_property("GDLlava", PropertyInfo(Variant::INT, "n_batch", PROPERTY_HINT_NONE), "set_n_batch", "get_n_batch");

    ClassDB::bind_method(D_METHOD("generate_text_base64", "prompt", "image_base64"), &GDLlava::generate_text_base64);
    ClassDB::bind_method(D_METHOD("generate_text_image", "prompt", "image"), &GDLlava::generate_text_image);
    ClassDB::bind_method(D_METHOD("run_generate_text_base64", "prompt", "image_base64"), &GDLlava::run_generate_text_base64);
    ClassDB::bind_method(D_METHOD("run_generate_text_image", "prompt", "image"), &GDLlava::run_generate_text_image);
    ClassDB::bind_method(D_METHOD("is_running"), &GDLlava::is_running);
    ClassDB::bind_method(D_METHOD("stop_generate_text"), &GDLlava::stop_generate_text);

    ADD_SIGNAL(MethodInfo("generate_text_updated", PropertyInfo(Variant::STRING, "new_text")));
    ADD_SIGNAL(MethodInfo("generate_text_finished", PropertyInfo(Variant::STRING, "text")));
}

// A dummy function for instantiating the state of generate_text_thread
void GDLlava::dummy() {}

GDLlava::GDLlava() : params {gpt_params()},
    llava_runner {new LlavaRunner()},
    glog {[](std::string s) {godot::UtilityFunctions::print(s.c_str());}},
    glog_verbose {[](std::string s) {godot::UtilityFunctions::print_verbose(s.c_str());}},
    generate_text_buffer {""}
{
    glog_verbose("Instantiate GDLlava mutex");
    func_mutex.instantiate();
    generate_text_mutex.instantiate();

    glog_verbose("Instantiate GDLlava thread");
    generate_text_thread.instantiate();
    generate_text_thread->start(callable_mp_static(&GDLlava::dummy));
    generate_text_thread->wait_to_finish();

    glog_verbose("Instantiate GDLlava thread -- done");
}

GDLlava::~GDLlava() {
    glog_verbose("GDLlava destructor");

    func_mutex->try_lock();

    while (!generate_text_mutex->try_lock()) {
        stop_generate_text();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    //is_started instead of is_alive to properly clean up all threads
    if (generate_text_thread->is_started()) {
        glog_verbose("GDLlava destructor waiting thread to finish");
        generate_text_thread->wait_to_finish();
    }
    glog_verbose("GDLlava destructor -- done");
}

void GDLlava::_exit_tree() {
    glog_verbose("GDLlava exit tree");

    func_mutex->lock();

    glog_verbose("func_mutex locked");

    while (!generate_text_mutex->try_lock()) {
        stop_generate_text();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    //is_started instead of is_alive to properly clean up all threads
    if (generate_text_thread->is_started()) {
        glog_verbose("Waiting thread to finish");
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

float GDLlava::get_temperature() const {
    return params.sparams.temp;
}

void GDLlava::set_temperature(const float p_temperature) {
    params.sparams.temp = p_temperature;
}

int32_t GDLlava::get_n_threads() const {
    return params.n_threads;
}

void GDLlava::set_n_threads(const int32_t p_n_threads) {
    params.n_threads = p_n_threads;
}

bool GDLlava::get_escape() const {
    return params.escape;
}

void GDLlava::set_escape(const bool p_escape) {
    params.escape = p_escape;
}

int32_t GDLlava::get_n_batch() const {
    return params.n_batch;
}

void GDLlava::set_n_batch(const int32_t p_n_batch) {
    params.n_batch = p_n_batch;
}

String GDLlava::generate_text_common(String prompt, String image_base64) {
    glog_verbose("generate_text_common");

    llava_runner.reset(new LlavaRunner());

    std::string generated_text = llava_runner->llava_generate_text_base64(
        string_gd_to_std(prompt),
        string_gd_to_std(image_base64),
        params,
        [this](std::string s) {
            if (generate_text_buffer.empty() && is_utf8(s.data())){
                String new_text = string_std_to_gd(s);
                call_deferred("emit_signal", "generate_text_updated", new_text);
            } else {
                generate_text_buffer.append(s);
                if (is_utf8(generate_text_buffer.data())) {
                    String new_text = string_std_to_gd(generate_text_buffer);
                    generate_text_buffer.clear();
                    call_deferred("emit_signal", "generate_text_updated", new_text);
                }
            }
        },
        [this](std::string s) {
            String text {string_std_to_gd(s)};
            call_deferred("emit_signal", "generate_text_finished", text);
        }
    );

    glog_verbose("generate_text_common -- done");

    return string_std_to_gd(generated_text);
}

String GDLlava::generate_text_base64_internal(String prompt, String image_base64) {
    glog_verbose("generate_text_base64_internal");

    String full_generated_text = generate_text_common(prompt, image_base64);

    generate_text_mutex->unlock();
    glog_verbose("generate_text_mutex unlocked");

    glog_verbose("generate_text_base64_internal -- done");

    return full_generated_text;
}

String GDLlava::generate_text_base64(String prompt, String image_base64) {
    glog_verbose("generate_text_base64");

    func_mutex->lock();

    if (!generate_text_mutex->try_lock()) {
        glog("GDLlava is busy");

        generate_text_mutex->lock();
    }

    glog_verbose("generate_text_mutex locked");

    func_mutex->unlock();

    String full_generated_text = generate_text_base64_internal(prompt, image_base64);

    glog_verbose("generate_text_base64 -- done");

    return full_generated_text;
}

Error GDLlava::run_generate_text_base64(String prompt, String image_base64) {
    glog_verbose("run_generate_text_base64");
    func_mutex->lock();

    if (!generate_text_mutex->try_lock()) {
        glog("GDLlava is busy");

        generate_text_mutex->lock();
    }

    glog_verbose("generate_text_mutex locked");

    func_mutex->unlock();

    //is_started instead of is_alive to properly clean up all threads
    if (generate_text_thread->is_started()) {
        generate_text_thread->wait_to_finish();
    }

    generate_text_thread.instantiate();
    glog_verbose("generate_text_thread instantiated");

    Callable c = callable_mp(this, &GDLlava::generate_text_base64_internal);
    Error error = generate_text_thread->start(c.bind(prompt, image_base64));

    glog_verbose("run_generate_text_base64 -- done");
    return error;
}


String GDLlava::generate_text_image_internal(String prompt, Image* image) {
    glog_verbose("generate_text_image_internal");

    String image_base64 = Marshalls::get_singleton()->raw_to_base64(image->save_jpg_to_buffer());

    String full_generated_text = generate_text_common(prompt, image_base64);

    generate_text_mutex->unlock();
    glog_verbose("generate_text_mutex unlocked");

    glog_verbose("generate_text_image_internal -- done");

    return full_generated_text;
}

String GDLlava::generate_text_image(String prompt, Image* image) {
    glog_verbose("generate_text_image");

    func_mutex->lock();

    if (!generate_text_mutex->try_lock()) {
        glog("GDLlava is busy");

        generate_text_mutex->lock();
    }

    glog_verbose("generate_text_mutex locked");

    func_mutex->unlock();

    String full_generated_text = generate_text_image_internal(prompt, image);

    glog_verbose("generate_text_image -- done");

    return full_generated_text;
}

Error GDLlava::run_generate_text_image(String prompt, Image* image) {
    glog_verbose("run_generate_text_image");
    func_mutex->lock();

    if (!generate_text_mutex->try_lock()) {
        glog("GDLlava is busy");

        generate_text_mutex->lock();
    }

    glog_verbose("generate_text_mutex locked");

    func_mutex->unlock();

    //is_started instead of is_alive to properly clean up all threads
    if (generate_text_thread->is_started()) {
        generate_text_thread->wait_to_finish();
    }

    generate_text_thread.instantiate();
    glog_verbose("generate_text_thread instantiated");

    Callable c = callable_mp(this, &GDLlava::generate_text_image_internal);
    Error error = generate_text_thread->start(c.bind(prompt, image));

    glog_verbose("run_generate_text_image -- done");
    return error;
}

bool GDLlava::is_running() {
    return generate_text_thread->is_alive();
}

void GDLlava::stop_generate_text() {
    glog_verbose("Stopping llava_runner");
    llava_runner->llava_stop_generate_text();
}



} //namespace godot
