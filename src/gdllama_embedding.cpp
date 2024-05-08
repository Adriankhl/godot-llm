#include "gdllama_embedding.h"
#include "llama_embedding_runner.h"
#include "log.h"
#include <godot_cpp/variant/packed_float32_array.hpp>
#include <godot_cpp/variant/string.hpp>
#include <vector>
namespace godot {

void GDLlamaEmbedding::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_model_path"), &GDLlamaEmbedding::get_model_path);
	ClassDB::bind_method(D_METHOD("set_model_path", "p_model_path"), &GDLlamaEmbedding::set_model_path);
    ClassDB::add_property("GDLlamaEmbedding", PropertyInfo(Variant::STRING, "model_path", PROPERTY_HINT_FILE), "set_model_path", "get_model_path");

   	ClassDB::bind_method(D_METHOD("get_n_batch"), &GDLlamaEmbedding::get_n_batch);
	ClassDB::bind_method(D_METHOD("set_n_batch", "p_n_batch"), &GDLlamaEmbedding::set_n_batch);
    ClassDB::add_property("GDLlamaEmbedding", PropertyInfo(Variant::INT, "n_batch", PROPERTY_HINT_NONE), "set_n_batch", "get_n_batch");

    ClassDB::bind_method(D_METHOD("compute_embedding", "prompt"), &GDLlamaEmbedding::compute_embedding);
    ClassDB::bind_method(D_METHOD("run_compute_embedding", "prompt"), &GDLlamaEmbedding::run_compute_embedding);
    ClassDB::bind_method(D_METHOD("is_running"), &GDLlamaEmbedding::is_running);

    ADD_SIGNAL(MethodInfo("compute_finished", PropertyInfo(Variant::PACKED_FLOAT32_ARRAY, "embedding")));
}

// A dummy function for instantiating the state of generate_text_thread
void GDLlamaEmbedding::dummy() {}

GDLlamaEmbedding::GDLlamaEmbedding() : params {gpt_params()},
    llama_embedding_runner {new LlamaEmbeddingRunner()} 
{
    log_set_target(stdout);
    LOG("Instantiate GDLlamaEmbedding mutex\n");

    func_mutex.instantiate();
    compute_embedding_mutex.instantiate();

    LOG("Instantiate GDLlamaEmbedding thread\n");
    compute_embedding_thread.instantiate();
    compute_embedding_thread->start(callable_mp_static(&GDLlamaEmbedding::dummy));
    compute_embedding_thread->wait_to_finish();

    LOG("Instantiate GDLlamaEmbedding thread -- done\n");
}

GDLlamaEmbedding::~GDLlamaEmbedding() {
    LOG("GDLlamaEmbedding destructor\n");

    //is_started instead of is_alive to properly clean up all threads
    if (compute_embedding_thread->is_started()) {
        LOG("GDLlamaEmbedding destructor waiting thread to finish\n");
        compute_embedding_thread->wait_to_finish();
    }
    LOG("GDLlamaEmbedding destructor -- done\n");
}

void GDLlamaEmbedding::_exit_tree() {
    LOG("GDLlamaEmbedding exit tree\n");

    func_mutex->lock();

    LOG("func_mutex locked\n");

    //is_started instead of is_alive to properly clean up all threads
    if (compute_embedding_thread->is_started()) {
        LOG("Waiting thread to finish\n");
        compute_embedding_thread->wait_to_finish();
    }
}

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

bool GDLlamaEmbedding::is_running() {
    return !compute_embedding_mutex->try_lock() || compute_embedding_thread->is_alive();
}

PackedFloat32Array GDLlamaEmbedding::compute_embedding_internal(String prompt) {
    LOG("compute_embedding_internal\n");
    llama_embedding_runner.reset(new LlamaEmbeddingRunner());

    std::vector<float> vec = llama_embedding_runner->compute_embedding(
        prompt.utf8().get_data(),
        params,
        [this](std::vector<float> vec) {
            PackedFloat32Array array = float32_vec_to_array(vec);
            call_deferred("emit_signal", "compute_finished", array);
        }
    );

    PackedFloat32Array array = float32_vec_to_array(vec);

    return array;
}

PackedFloat32Array GDLlamaEmbedding::compute_embedding(String prompt) {
    LOG("compute_embedding\n");
    func_mutex->lock();

    if (compute_embedding_mutex->try_lock()) {
        LOG("GDLlamaEmbedding is busy\n");
    }

    compute_embedding_mutex->lock();
    LOG("compute_embedding_mutex locked\n");

    func_mutex->unlock();

    PackedFloat32Array pfa = compute_embedding_internal(prompt);

    compute_embedding_mutex->unlock();

    return pfa;
}

Error GDLlamaEmbedding::run_compute_embedding(String prompt) {
    LOG("run_compute_embedding\n");
    func_mutex->lock();
    
    std::cout << "Locking" << std::endl;
    
    if (compute_embedding_mutex->try_lock()) {
        LOG("GDLlamaEmbedding is busy\n");
    }

    compute_embedding_mutex->lock();
    LOG("compute_embedding_mutex locked\n");

    func_mutex->unlock();

    //is_started instead of is_alive to properly clean up all threads
    if (compute_embedding_thread->is_started()) {
        compute_embedding_thread->wait_to_finish();
    }

    compute_embedding_thread.instantiate();
    LOG("compute_embedding_thread instantiated\n");

    Callable c = callable_mp(this, &GDLlamaEmbedding::compute_embedding_internal);
    Error error = compute_embedding_thread->start(c.bind(prompt));

    return error;
}

} //namespace godot