#include "gdllava.h"
#include "conversion.h"
#include "llava_runner.h"

namespace godot {

void GDLlava::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_model_path"), &GDLlava::get_model_path);
	ClassDB::bind_method(D_METHOD("set_model_path", "p_model_path"), &GDLlava::set_model_path);
    ClassDB::add_property("GDLlava", PropertyInfo(Variant::STRING, "model_path", PROPERTY_HINT_FILE), "set_model_path", "get_model_path");

    ClassDB::bind_method(D_METHOD("get_n_ctx"), &GDLlava::get_n_ctx);
    ClassDB::bind_method(D_METHOD("set_n_ctx", "p_n_ctx"), &GDLlava::set_n_ctx);
    ClassDB::add_property("GDLlava", PropertyInfo(Variant::INT, "context_size", PROPERTY_HINT_NONE), "set_n_ctx", "get_n_ctx");

	ClassDB::bind_method(D_METHOD("get_n_predict"), &GDLlava::get_n_predict);
	ClassDB::bind_method(D_METHOD("set_n_predict", "p_n_predict"), &GDLlava::set_n_predict);
    ClassDB::add_property("GDLlava", PropertyInfo(Variant::INT, "n_predict", PROPERTY_HINT_NONE), "set_n_predict", "get_n_predict");

    ClassDB::bind_method(D_METHOD("get_n_batch"), &GDLlava::get_n_batch);
    ClassDB::bind_method(D_METHOD("set_n_batch", "p_n_batch"), &GDLlava::set_n_batch);
    ClassDB::add_property("GDLlava", PropertyInfo(Variant::INT, "n_batch", PROPERTY_HINT_NONE), "set_n_batch", "get_n_batch");
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

bool GDLlava::is_running() {
    return !generate_text_mutex->try_lock() || generate_text_thread->is_alive();
}

} //namespace godot