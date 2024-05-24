#include "conversion.hpp"
#include "gdembedding.hpp"
#include "embedding_runner.hpp"
#include <godot_cpp/variant/callable_method_pointer.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <vector>


namespace godot {

void GDEmbedding::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_model_path"), &GDEmbedding::get_model_path);
	ClassDB::bind_method(D_METHOD("set_model_path", "p_model_path"), &GDEmbedding::set_model_path);
    ClassDB::add_property("GDEmbedding", PropertyInfo(Variant::STRING, "model_path", PROPERTY_HINT_FILE), "set_model_path", "get_model_path");

    ClassDB::bind_method(D_METHOD("get_n_threads"), &GDEmbedding::get_n_threads);
    ClassDB::bind_method(D_METHOD("set_n_threads", "p_n_threads"), &GDEmbedding::set_n_threads);
    ClassDB::add_property("GDEmbedding", PropertyInfo(Variant::INT, "n_threads", PROPERTY_HINT_NONE), "set_n_threads", "get_n_threads");

	ClassDB::bind_method(D_METHOD("get_n_gpu_layer"), &GDEmbedding::get_n_gpu_layer);
	ClassDB::bind_method(D_METHOD("set_n_gpu_layer", "p_n_gpu_layer"), &GDEmbedding::set_n_gpu_layer);
    ClassDB::add_property("GDEmbedding", PropertyInfo(Variant::INT, "n_gpu_layer", PROPERTY_HINT_NONE), "set_n_gpu_layer", "get_n_gpu_layer");

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

GDEmbedding::GDEmbedding() : params {gpt_params()},
    embedding_runner {new EmbeddingRunner()},
    glog {[](std::string s) {godot::UtilityFunctions::print(s.c_str());}},
    glog_verbose {[](std::string s) {godot::UtilityFunctions::print_verbose(s.c_str());}}
{
    glog_verbose("Instantiate GDEmbedding mutex");

    func_mutex.instantiate();
    compute_embedding_mutex.instantiate();

    glog_verbose("Instantiate GDEmbedding thread");
    compute_embedding_thread.instantiate();
    auto f = (void(*)())[](){};
    compute_embedding_thread->start(create_custom_callable_static_function_pointer(f));
    compute_embedding_thread->wait_to_finish();

    glog_verbose("Instantiate GDEmbedding thread -- done");
}

GDEmbedding::~GDEmbedding() {
    glog_verbose("GDEmbedding destructor");

    func_mutex->try_lock();

    while (!compute_embedding_mutex->try_lock()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    //is_started instead of is_alive to properly clean up all threads
    if (compute_embedding_thread->is_started()) {
        glog_verbose("GDEmbedding destructor waiting thread to finish");
        compute_embedding_thread->wait_to_finish();
    }
    glog_verbose("GDEmbedding destructor -- done");
}

void GDEmbedding::_exit_tree() {
    glog_verbose("GDEmbedding exit tree");

    func_mutex->lock();

    glog_verbose("func_mutex locked");

    while (!compute_embedding_mutex->try_lock()) {
        glog_verbose("Waiting for lock");
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    //is_started instead of is_alive to properly clean up all threads
    if (compute_embedding_thread->is_started()) {
        glog_verbose("Waiting thread to finish");
        compute_embedding_thread->wait_to_finish();
    }

    glog_verbose("GDEmbedding exit tree -- done");
}

String GDEmbedding::get_model_path() const {
    return string_std_to_gd(params.model);
}

void GDEmbedding::set_model_path(const String p_model_path) {
    params.model = string_gd_to_std(p_model_path.trim_prefix(String("res://")));
}

int32_t GDEmbedding::get_n_threads() const {
    return params.n_threads;
}

void GDEmbedding::set_n_threads(const int32_t p_n_threads) {
    params.n_threads = p_n_threads;
}

int32_t GDEmbedding::get_n_gpu_layer() const {
    return params.n_gpu_layers;
}

void GDEmbedding::set_n_gpu_layer(const int32_t p_n_gpu_layers) {
    params.n_gpu_layers = p_n_gpu_layers;
}


int32_t GDEmbedding::get_n_batch() const {
    return params.n_batch;
}

void GDEmbedding::set_n_batch(const int32_t p_n_batch) {
    params.n_batch = p_n_batch;
}

bool GDEmbedding::is_running() {
    return compute_embedding_thread->is_alive();
}

PackedFloat32Array GDEmbedding::compute_embedding_internal(String prompt) {
    glog_verbose("compute_embedding_internal");
    embedding_runner.reset(new EmbeddingRunner());

    std::vector<float> vec = embedding_runner->compute_embedding(
        string_gd_to_std(prompt),
        params,
        [this](std::vector<float> vec) {
            PackedFloat32Array array = float32_vec_to_array(vec);
            call_deferred("emit_signal", "compute_embedding_finished", array);
        }
    );

    PackedFloat32Array array = float32_vec_to_array(vec);

    compute_embedding_mutex->unlock();
    glog_verbose("compute_embedding_mutex unlocked");

    glog_verbose("compute_embedding_internal -- done");

    return array;
}

PackedFloat32Array GDEmbedding::compute_embedding(String prompt) {
    glog_verbose("compute_embedding");
    func_mutex->lock();

    if (!compute_embedding_mutex->try_lock()) {
        glog("GDEmbedding is busy");
        compute_embedding_mutex->lock();
    }

    glog_verbose("compute_embedding_mutex locked");

    func_mutex->unlock();

    PackedFloat32Array pfa = compute_embedding_internal(prompt);

    glog_verbose("compute_embedding -- done");

    return pfa;
}

Error GDEmbedding::run_compute_embedding(String prompt) {
    glog_verbose("run_compute_embedding");
    func_mutex->lock();
    
    if (!compute_embedding_mutex->try_lock()) {
        glog("GDEmbedding is busy");
        compute_embedding_mutex->lock();
    }
    glog_verbose("compute_embedding_mutex locked");

    func_mutex->unlock();

    //is_started instead of is_alive to properly clean up all threads
    if (compute_embedding_thread->is_started()) {
        compute_embedding_thread->wait_to_finish();
    }

    compute_embedding_thread.instantiate();
    glog_verbose("compute_embedding_thread instantiated");

    Callable c = callable_mp(this, &GDEmbedding::compute_embedding_internal);
    Error error = compute_embedding_thread->start(c.bind(prompt));

    glog_verbose("run_compute_embedding -- done");

    return error;
}

float GDEmbedding::similarity_cos_array(PackedFloat32Array array1, PackedFloat32Array array2){
    if (array1.size() != array2.size() || array1.size() == 0) {
        glog_verbose("Error: embedding sizes don't match");
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
    glog_verbose("similarity_cos_string_internal");
    embedding_runner.reset(new EmbeddingRunner());

    std::vector<float> vec1 = embedding_runner->compute_embedding(
        string_gd_to_std(s1),
        params,
        [](auto a){}
    );

    std::vector<float> vec2 = embedding_runner->compute_embedding(
        string_gd_to_std(s2),
        params,
        [](auto a){}
    );

    float similarity = embedding_runner->similarity_cos(vec1, vec2);

    call_deferred("emit_signal", "similarity_cos_string_finished", similarity);

    compute_embedding_mutex->unlock();
    glog_verbose("compute_embedding_mutex unlocked");

    glog_verbose("similarity_cos_string_internal -- done");

    return similarity;
}

float GDEmbedding::similarity_cos_string(String s1, String s2) {
    glog_verbose("similarity_cos_string");
    func_mutex->lock();

    if (!compute_embedding_mutex->try_lock()) {
        glog("GDEmbedding is busy");
        compute_embedding_mutex->lock();
    }

    glog_verbose("compute_embedding_mutex locked");

    func_mutex->unlock();

    float similarity = similarity_cos_string(s1, s2);

    glog_verbose("similarity_cos_string -- done");

    return similarity;
}


Error GDEmbedding::run_similarity_cos_string(String s1, String s2) {
    glog_verbose("run_similarity_cos_string");
    func_mutex->lock();

    if (!compute_embedding_mutex->try_lock()) {
        glog("GDEmbedding is busy");
        compute_embedding_mutex->lock();
    }

    glog_verbose("compute_embedding_mutex locked");

    func_mutex->unlock();

    //is_started instead of is_alive to properly clean up all threads
    if (compute_embedding_thread->is_started()) {
        compute_embedding_thread->wait_to_finish();
    }

    compute_embedding_thread.instantiate();
    glog_verbose("compute_embedding_thread instantiated");

    Callable c = callable_mp(this, &GDEmbedding::similarity_cos_string_internal);
    Error error = compute_embedding_thread->start(c.bind(s1, s2));

    glog_verbose("run_similarity_cos_string -- done");

    return error;
}

int GDEmbedding::get_n_embd() {
    return embedding_runner->get_n_embd(params);
}

} //namespace godot
