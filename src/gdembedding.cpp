#include "conversion.h"
#include "gdembedding.h"
#include "llama_embedding_runner.h"
#include "log.h"
#include <godot_cpp/variant/packed_float32_array.hpp>
#include <godot_cpp/variant/string.hpp>
#include <vector>


namespace godot {

void GDEmbedding::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_model_path"), &GDEmbedding::get_model_path);
	ClassDB::bind_method(D_METHOD("set_model_path", "p_model_path"), &GDEmbedding::set_model_path);
    ClassDB::add_property("GDEmbedding", PropertyInfo(Variant::STRING, "model_path", PROPERTY_HINT_FILE), "set_model_path", "get_model_path");

   	ClassDB::bind_method(D_METHOD("get_n_batch"), &GDEmbedding::get_n_batch);
	ClassDB::bind_method(D_METHOD("set_n_batch", "p_n_batch"), &GDEmbedding::set_n_batch);
    ClassDB::add_property("GDEmbedding", PropertyInfo(Variant::INT, "n_batch", PROPERTY_HINT_NONE), "set_n_batch", "get_n_batch");

    ClassDB::bind_method(D_METHOD("compute_embedding", "prompt"), &GDEmbedding::compute_embedding);
    ClassDB::bind_method(D_METHOD("run_compute_embedding", "prompt"), &GDEmbedding::run_compute_embedding);
    ClassDB::bind_method(D_METHOD("is_running"), &GDEmbedding::is_running);
    ClassDB::bind_method(D_METHOD("similarity_cos_array", "array1", "array2"), &GDEmbedding::similarity_cos_array);
    ClassDB::bind_method(D_METHOD("similarity_cos_string", "s1", "s2"), &GDEmbedding::similarity_cos_string);
    ClassDB::bind_method(D_METHOD("run_similarity_cos_string", "s1", "s2"), &GDEmbedding::run_similarity_cos_string);

    ADD_SIGNAL(MethodInfo("compute_embedding_finished", PropertyInfo(Variant::PACKED_FLOAT32_ARRAY, "embedding")));
    ADD_SIGNAL(MethodInfo("similarity_cos_string_finished", PropertyInfo(Variant::FLOAT, "similarity")));
}

// A dummy function for instantiating the state of generate_text_thread
void GDEmbedding::dummy() {}

GDEmbedding::GDEmbedding() : params {gpt_params()},
    llama_embedding_runner {new LlamaEmbeddingRunner()} 
{
    log_set_target(stdout);
    LOG("Instantiate GDEmbedding mutex\n");

    func_mutex.instantiate();
    compute_embedding_mutex.instantiate();

    LOG("Instantiate GDEmbedding thread\n");
    compute_embedding_thread.instantiate();
    compute_embedding_thread->start(callable_mp_static(&GDEmbedding::dummy));
    compute_embedding_thread->wait_to_finish();

    LOG("Instantiate GDEmbedding thread -- done\n");
}

GDEmbedding::~GDEmbedding() {
    LOG("GDEmbedding destructor\n");

    if (!func_mutex->try_lock()) {
        func_mutex->lock();
    }

    while (!compute_embedding_mutex->try_lock()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    //is_started instead of is_alive to properly clean up all threads
    if (compute_embedding_thread->is_started()) {
        LOG("GDEmbedding destructor waiting thread to finish\n");
        compute_embedding_thread->wait_to_finish();
    }
    LOG("GDEmbedding destructor -- done\n");
}

void GDEmbedding::_exit_tree() {
    LOG("GDEmbedding exit tree\n");

    func_mutex->lock();

    LOG("func_mutex locked\n");

    while (!compute_embedding_mutex->try_lock()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    //is_started instead of is_alive to properly clean up all threads
    if (compute_embedding_thread->is_started()) {
        LOG("Waiting thread to finish\n");
        compute_embedding_thread->wait_to_finish();
    }
}

String GDEmbedding::get_model_path() const {
    return string_std_to_gd(params.model);
}

void GDEmbedding::set_model_path(const String p_model_path) {
    params.model = string_gd_to_std(p_model_path.trim_prefix(String("res://")));
}

int32_t GDEmbedding::get_n_batch() const {
    return params.n_batch;
}

void GDEmbedding::set_n_batch(const int32_t p_n_batch) {
    params.n_batch = p_n_batch;
}

bool GDEmbedding::is_running() {
    return !compute_embedding_mutex->try_lock() || compute_embedding_thread->is_alive();
}

PackedFloat32Array GDEmbedding::compute_embedding_internal(String prompt) {
    LOG("compute_embedding_internal\n");
    llama_embedding_runner.reset(new LlamaEmbeddingRunner());

    std::vector<float> vec = llama_embedding_runner->compute_embedding(
        string_gd_to_std(prompt),
        params,
        [this](std::vector<float> vec) {
            PackedFloat32Array array = float32_vec_to_array(vec);
            call_deferred("emit_signal", "compute_embedding_finished", array);
        }
    );

    PackedFloat32Array array = float32_vec_to_array(vec);

    LOG("compute_embedding_internal -- done\n");

    return array;
}

PackedFloat32Array GDEmbedding::compute_embedding(String prompt) {
    LOG("compute_embedding\n");
    func_mutex->lock();

    if (compute_embedding_mutex->try_lock()) {
        LOG("GDEmbedding is busy\n");
    }

    compute_embedding_mutex->lock();
    LOG("compute_embedding_mutex locked\n");

    func_mutex->unlock();

    PackedFloat32Array pfa = compute_embedding_internal(prompt);

    compute_embedding_mutex->unlock();

    LOG("compute_embedding -- done\n");

    return pfa;
}

Error GDEmbedding::run_compute_embedding(String prompt) {
    LOG("run_compute_embedding\n");
    func_mutex->lock();
    
    if (compute_embedding_mutex->try_lock()) {
        LOG("GDEmbedding is busy\n");
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

    Callable c = callable_mp(this, &GDEmbedding::compute_embedding_internal);
    Error error = compute_embedding_thread->start(c.bind(prompt));

    LOG("run_compute_embedding -- done\n");

    return error;
}

float GDEmbedding::similarity_cos_array(PackedFloat32Array array1, PackedFloat32Array array2){
    if (array1.size() != array2.size() || array1.size() == 0) {
        LOG("Error: embedding sizes don't match");
        return 0.0;
    }

    double sum  = 0.0;
    double sum1 = 0.0;
    double sum2 = 0.0;

    for (int i = 0; i < array1.size(); i++) {
        sum  += array1[i] * array2[i];
        sum1 += array1[i] * array1[i];
        sum2 += array2[i] * array2[i];
    }

    return sum / (sqrt(sum1) * sqrt(sum2));
};

float GDEmbedding::similarity_cos_string_internal(String s1, String s2) {
    LOG("similarity_cos_string_internal\n");
    llama_embedding_runner.reset(new LlamaEmbeddingRunner());

    std::vector<float> vec1 = llama_embedding_runner->compute_embedding(
        string_gd_to_std(s1),
        params,
        [](auto a){}
    );

    std::vector<float> vec2 = llama_embedding_runner->compute_embedding(
        string_gd_to_std(s2),
        params,
        [](auto a){}
    );

    float similarity = LlamaEmbeddingRunner::similarity_cos(vec1, vec2);

    call_deferred("emit_signal", "similarity_cos_string_finished", similarity);

    compute_embedding_mutex->unlock();

    LOG("similarity_cos_string_internal -- done\n");

    return similarity;
}

float GDEmbedding::similarity_cos_string(String s1, String s2) {
    LOG("similarity_cos_string\n");
    func_mutex->lock();

    if (compute_embedding_mutex->try_lock()) {
        LOG("GDEmbedding is busy\n");
    }

    compute_embedding_mutex->lock();
    LOG("compute_embedding_mutex locked\n");

    func_mutex->unlock();

    float similarity = similarity_cos_string(s1, s2);

    LOG("similarity_cos_string -- done\n");

    return similarity;
}


Error GDEmbedding::run_similarity_cos_string(String s1, String s2) {
    LOG("run_similarity_cos_string\n");
    func_mutex->lock();

    if (compute_embedding_mutex->try_lock()) {
        LOG("GDEmbedding is busy\n");
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

    Callable c = callable_mp(this, &GDEmbedding::similarity_cos_string_internal);
    Error error = compute_embedding_thread->start(c.bind(s1, s2));

    LOG("run_similarity_cos_string -- done\n");

    return error;
}

} //namespace godot