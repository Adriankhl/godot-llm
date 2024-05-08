#include "gdllama.h"
#include "llama_runner.h"
#include "log.h"
#include <godot_cpp/classes/global_constants.hpp>
#include <godot_cpp/classes/mutex.hpp>
#include <godot_cpp/classes/thread.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/callable.hpp>
#include <godot_cpp/variant/callable_method_pointer.hpp>
#include <godot_cpp/variant/quaternion.hpp>
#include <godot_cpp/variant/string.hpp>
#include <common/common.h>
#include <common/json.hpp>
#include <common/json-schema-to-grammar.h>
#include <cstdint>
#include <memory>
#include <string>

namespace godot {

void GDLlama::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_model_path"), &GDLlama::get_model_path);
	ClassDB::bind_method(D_METHOD("set_model_path", "p_model_path"), &GDLlama::set_model_path);
    ClassDB::add_property("GDLlama", PropertyInfo(Variant::STRING, "model_path", PROPERTY_HINT_FILE), "set_model_path", "get_model_path");

	ClassDB::bind_method(D_METHOD("get_instruct"), &GDLlama::get_instruct);
	ClassDB::bind_method(D_METHOD("set_instruct", "p_instruct"), &GDLlama::set_instruct);
    ClassDB::add_property("GDLlama", PropertyInfo(Variant::BOOL, "instruct", PROPERTY_HINT_NONE), "set_instruct", "get_instruct");

	ClassDB::bind_method(D_METHOD("get_interactive"), &GDLlama::get_interactive);
	ClassDB::bind_method(D_METHOD("set_interactive", "p_interactive"), &GDLlama::set_interactive);
    ClassDB::add_property("GDLlama", PropertyInfo(Variant::BOOL, "interactive", PROPERTY_HINT_NONE), "set_interactive", "get_interactive");

	ClassDB::bind_method(D_METHOD("get_reverse_prompt"), &GDLlama::get_reverse_prompt);
	ClassDB::bind_method(D_METHOD("set_reverse_prompt", "p_reverse_prompt"), &GDLlama::set_reverse_prompt);
    ClassDB::add_property("GDLlama", PropertyInfo(Variant::STRING, "reverse_prompt", PROPERTY_HINT_NONE), "set_reverse_prompt", "get_reverse_prompt");

    ClassDB::bind_method(D_METHOD("get_should_output_prompt"), &GDLlama::get_should_output_prompt);
	ClassDB::bind_method(D_METHOD("set_should_output_prompt", "p_should_output_prompt"), &GDLlama::set_should_output_prompt);
    ClassDB::add_property("GDLlama", PropertyInfo(Variant::BOOL, "should_output_prompt", PROPERTY_HINT_NONE), "set_should_output_prompt", "get_should_output_prompt");

    ClassDB::bind_method(D_METHOD("get_should_output_bos"), &GDLlama::get_should_output_bos);
	ClassDB::bind_method(D_METHOD("set_should_output_bos", "p_should_output_bos"), &GDLlama::set_should_output_bos);
    ClassDB::add_property("GDLlama", PropertyInfo(Variant::BOOL, "should_output_bos", PROPERTY_HINT_NONE), "set_should_output_bos", "get_should_output_bos");

    ClassDB::bind_method(D_METHOD("get_should_output_eos"), &GDLlama::get_should_output_eos);
	ClassDB::bind_method(D_METHOD("set_should_output_eos", "p_should_output_eos"), &GDLlama::set_should_output_eos);
    ClassDB::add_property("GDLlama", PropertyInfo(Variant::BOOL, "should_output_eos", PROPERTY_HINT_NONE), "set_should_output_eos", "get_should_output_eos");

	ClassDB::bind_method(D_METHOD("get_input_prefix"), &GDLlama::get_input_prefix);
	ClassDB::bind_method(D_METHOD("set_input_prefix", "p_input_prefix"), &GDLlama::set_input_prefix);
    ClassDB::add_property("GDLlama", PropertyInfo(Variant::STRING, "input_prefix", PROPERTY_HINT_NONE), "set_input_prefix", "get_input_prefix");

	ClassDB::bind_method(D_METHOD("get_input_suffix"), &GDLlama::get_input_suffix);
	ClassDB::bind_method(D_METHOD("set_input_suffix", "p_input_suffix"), &GDLlama::set_input_suffix);
    ClassDB::add_property("GDLlama", PropertyInfo(Variant::STRING, "input_suffix", PROPERTY_HINT_NONE), "set_input_suffix", "get_input_suffix");

   	ClassDB::bind_method(D_METHOD("get_n_ctx"), &GDLlama::get_n_ctx);
	ClassDB::bind_method(D_METHOD("set_n_ctx", "p_n_ctx"), &GDLlama::set_n_ctx);
    ClassDB::add_property("GDLlama", PropertyInfo(Variant::INT, "context_size", PROPERTY_HINT_NONE), "set_n_ctx", "get_n_ctx");

	ClassDB::bind_method(D_METHOD("get_n_predict"), &GDLlama::get_n_predict);
	ClassDB::bind_method(D_METHOD("set_n_predict", "p_n_predict"), &GDLlama::set_n_predict);
    ClassDB::add_property("GDLlama", PropertyInfo(Variant::INT, "n_predict", PROPERTY_HINT_NONE), "set_n_predict", "get_n_predict");

	ClassDB::bind_method(D_METHOD("get_n_keep"), &GDLlama::get_n_keep);
	ClassDB::bind_method(D_METHOD("set_n_keep", "p_n_keep"), &GDLlama::set_n_keep);
    ClassDB::add_property("GDLlama", PropertyInfo(Variant::INT, "n_keep", PROPERTY_HINT_NONE), "set_n_keep", "get_n_keep");

    ClassDB::bind_method(D_METHOD("get_temperature"), &GDLlama::get_temperature);
    ClassDB::bind_method(D_METHOD("set_temperature", "p_temperature"), &GDLlama::set_temperature);
    ClassDB::add_property("GDLlama", PropertyInfo(Variant::FLOAT, "temperature", PROPERTY_HINT_NONE), "set_temperature", "get_temperature");

    ClassDB::bind_method(D_METHOD("get_penalty_repeat"), &GDLlama::get_penalty_repeat);
    ClassDB::bind_method(D_METHOD("set_penalty_repeat", "p_penalty_repeat"), &GDLlama::set_penalty_repeat);
    ClassDB::add_property("GDLlama", PropertyInfo(Variant::FLOAT, "penalty_repeat", PROPERTY_HINT_NONE), "set_penalty_repeat", "get_penalty_repeat");

    ClassDB::bind_method(D_METHOD("get_penalty_last_n"), &GDLlama::get_penalty_last_n);
    ClassDB::bind_method(D_METHOD("set_penalty_last_n", "p_penalty_last_n"), &GDLlama::set_penalty_last_n);
    ClassDB::add_property("GDLlama", PropertyInfo(Variant::INT, "penalty_last_n", PROPERTY_HINT_NONE), "set_penalty_last_n", "get_penalty_last_n");

    ClassDB::bind_method(D_METHOD("get_penalize_nl"), &GDLlama::get_penalize_nl);
    ClassDB::bind_method(D_METHOD("set_penalize_nl", "p_penalize_nl"), &GDLlama::set_penalize_nl);
    ClassDB::add_property("GDLlama", PropertyInfo(Variant::BOOL, "penalize_nl", PROPERTY_HINT_NONE), "set_penalize_nl", "get_penalize_nl");

    ClassDB::bind_method(D_METHOD("get_top_k"), &GDLlama::get_top_k);
    ClassDB::bind_method(D_METHOD("set_top_k", "p_top_k"), &GDLlama::set_top_k);
    ClassDB::add_property("GDLlama", PropertyInfo(Variant::INT, "top_k", PROPERTY_HINT_NONE), "set_top_k", "get_top_k");

    ClassDB::bind_method(D_METHOD("get_top_p"), &GDLlama::get_top_p);
    ClassDB::bind_method(D_METHOD("set_top_p", "p_top_p"), &GDLlama::set_top_p);
    ClassDB::add_property("GDLlama", PropertyInfo(Variant::FLOAT, "top_p", PROPERTY_HINT_NONE), "set_top_p", "get_top_p");

    ClassDB::bind_method(D_METHOD("get_min_p"), &GDLlama::get_min_p);
    ClassDB::bind_method(D_METHOD("set_min_p", "p_min_p"), &GDLlama::set_min_p);
    ClassDB::add_property("GDLlama", PropertyInfo(Variant::FLOAT, "min_p", PROPERTY_HINT_NONE), "set_min_p", "get_min_p");

    ClassDB::bind_method(D_METHOD("get_n_threads"), &GDLlama::get_n_threads);
    ClassDB::bind_method(D_METHOD("set_n_threads", "p_n_threads"), &GDLlama::set_n_threads);
    ClassDB::add_property("GDLlama", PropertyInfo(Variant::INT, "n_threads", PROPERTY_HINT_NONE), "set_n_threads", "get_n_threads");

	ClassDB::bind_method(D_METHOD("get_n_gpu_layer"), &GDLlama::get_n_gpu_layer);
	ClassDB::bind_method(D_METHOD("set_n_gpu_layer", "p_n_gpu_layer"), &GDLlama::set_n_gpu_layer);
    ClassDB::add_property("GDLlama", PropertyInfo(Variant::INT, "n_gpu_layer", PROPERTY_HINT_NONE), "set_n_gpu_layer", "get_n_gpu_layer");

	ClassDB::bind_method(D_METHOD("get_escape"), &GDLlama::get_escape);
	ClassDB::bind_method(D_METHOD("set_escape", "p_escape"), &GDLlama::set_escape);
    ClassDB::add_property("GDLlama", PropertyInfo(Variant::BOOL, "escape", PROPERTY_HINT_NONE), "set_escape", "get_escape");

   	ClassDB::bind_method(D_METHOD("get_n_batch"), &GDLlama::get_n_batch);
	ClassDB::bind_method(D_METHOD("set_n_batch", "p_n_batch"), &GDLlama::set_n_batch);
    ClassDB::add_property("GDLlama", PropertyInfo(Variant::INT, "n_batch", PROPERTY_HINT_NONE), "set_n_batch", "get_n_batch");

   	ClassDB::bind_method(D_METHOD("get_n_ubatch"), &GDLlama::get_n_ubatch);
	ClassDB::bind_method(D_METHOD("set_n_ubatch", "p_n_ubatch"), &GDLlama::set_n_ubatch);
    ClassDB::add_property("GDLlama", PropertyInfo(Variant::INT, "n_ubatch", PROPERTY_HINT_NONE), "set_n_ubatch", "get_n_ubatch");

    ClassDB::bind_method(D_METHOD("generate_text_simple", "prompt"), &GDLlama::generate_text_simple);
    ClassDB::bind_method(D_METHOD("generate_text_grammar", "prompt", "grammar"), &GDLlama::generate_text_grammar);
    ClassDB::bind_method(D_METHOD("generate_text_json", "prompt", "json"), &GDLlama::generate_text_json);
    ClassDB::bind_method(D_METHOD("generate_text", "prompt", "grammar", "json"), &GDLlama::generate_text);
    ClassDB::bind_method(D_METHOD("run_generate_text", "prompt", "grammar", "json"), &GDLlama::run_generate_text);
    ClassDB::bind_method(D_METHOD("stop_generate_text"), &GDLlama::stop_generate_text);
    ClassDB::bind_method(D_METHOD("input_text", "input"), &GDLlama::input_text);
    ClassDB::bind_method(D_METHOD("is_running"), &GDLlama::is_running);
    ClassDB::bind_method(D_METHOD("is_waiting_input"), &GDLlama::is_waiting_input);

    ADD_SIGNAL(MethodInfo("generate_text_updated", PropertyInfo(Variant::STRING, "new_text")));
    ADD_SIGNAL(MethodInfo("input_wait_started"));
    ADD_SIGNAL(MethodInfo("generate_text_finished", PropertyInfo(Variant::STRING, "text")));

}

// A dummy function for instantiating the state of generate_text_thread
void GDLlama::dummy() {}

GDLlama::GDLlama() : params {gpt_params()},
    reverse_prompt {""},
    llama_runner {new LlamaRunner(should_output_prompt)},
    should_output_prompt {true},
    should_output_bos {true},
    should_output_eos {true}
{
    log_set_target(stdout);
    LOG("Instantiate GDLlama mutex\n");
    func_mutex.instantiate();
    generate_text_mutex.instantiate();

    LOG("Instantiate GDLlama thread\n");
    generate_text_thread.instantiate();
    generate_text_thread->start(callable_mp_static(&GDLlama::dummy));
    generate_text_thread->wait_to_finish();

    LOG("Instantiate GDLlama thread -- done\n");
}

GDLlama::~GDLlama() {
    LOG("GDLlama destructor\n");

    while (!generate_text_mutex->try_lock()) {
        stop_generate_text();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    //is_started instead of is_alive to properly clean up all threads
    if (generate_text_thread->is_started()) {
        LOG("GDLlama destructor waiting thread to finish\n");
        generate_text_thread->wait_to_finish();
    }
    LOG("GDLlama destructor -- done\n");
}

void GDLlama::_exit_tree() {
    LOG("GDLlama exit tree\n");

    func_mutex->lock();

    LOG("func_mutex locked\n");

    while (!generate_text_mutex->try_lock()) {
        stop_generate_text();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    //is_started instead of is_alive to properly clean up all threads
    if (generate_text_thread->is_started()) {
        LOG("Waiting thread to finish\n");
        generate_text_thread->wait_to_finish();
    }
}

String GDLlama::get_model_path() const {
    return String(params.model.c_str());
}

void GDLlama::set_model_path(const String p_model_path) {
    params.model = std::string(p_model_path.trim_prefix(String("res://")).utf8().get_data());
}

bool GDLlama::get_instruct() const {
    return params.instruct;
}

void GDLlama::set_instruct(const bool p_instruct) {
    params.instruct = p_instruct;
}

bool GDLlama::get_interactive() const {
    return params.interactive;
}

void GDLlama::set_interactive(const bool p_interactive) {
    params.interactive = p_interactive;
}

String GDLlama::get_reverse_prompt() const {
    return String(reverse_prompt.c_str());
};

void GDLlama::set_reverse_prompt(const String p_reverse_prompt) {
    reverse_prompt = std::string(p_reverse_prompt.utf8().get_data());
};

bool GDLlama::get_should_output_prompt() const {
    return should_output_prompt;
};

void GDLlama::set_should_output_prompt(const bool p_should_output_prompt) {
    should_output_prompt = p_should_output_prompt;
};

bool GDLlama::get_should_output_bos() const {
    return should_output_bos;
};

void GDLlama::set_should_output_bos(const bool p_should_output_bos) {
    should_output_bos = p_should_output_bos;
};

bool GDLlama::get_should_output_eos() const {
    return should_output_eos;
};

void GDLlama::set_should_output_eos(const bool p_should_output_eos) {
    should_output_eos = p_should_output_eos;
};

String GDLlama::get_input_prefix() const {
    return String(params.input_prefix.c_str());
};

void GDLlama::set_input_prefix(const String p_input_prefix) {
    params.input_prefix = std::string(p_input_prefix.utf8().get_data());
};

String GDLlama::get_input_suffix() const {
    return String(params.input_suffix.c_str());
};

void GDLlama::set_input_suffix(const String p_input_suffix) {
    params.input_suffix = std::string(p_input_suffix.utf8().get_data());
};

int32_t GDLlama::get_n_ctx() const {
    return params.n_ctx;
}

void GDLlama::set_n_ctx(const int32_t p_n_ctx) {
    params.n_ctx = p_n_ctx;
}

int32_t GDLlama::get_n_predict() const {
    return params.n_predict;
}

void GDLlama::set_n_predict(const int32_t p_n_predict) {
    params.n_predict = p_n_predict;
}

int32_t GDLlama::get_n_keep() const {
    return params.n_keep;
}

void GDLlama::set_n_keep(const int32_t p_n_keep) {
    params.n_keep = p_n_keep;
}

float GDLlama::get_temperature() const {
    return params.sparams.temp;
}

void GDLlama::set_temperature(const float p_temperature) {
    params.sparams.temp = p_temperature;
}

float GDLlama::get_penalty_repeat() const {
    return params.sparams.penalty_repeat;
}

void GDLlama::set_penalty_repeat(const float p_penalty_repeat) {
    params.sparams.penalty_repeat = p_penalty_repeat;
}

int32_t GDLlama::get_penalty_last_n() const {
    return params.sparams.penalty_last_n;
}

void GDLlama::set_penalty_last_n(const int32_t p_penalty_last_n) {
    params.sparams.penalty_last_n = p_penalty_last_n;
}

bool GDLlama::get_penalize_nl() const {
    return params.sparams.penalize_nl;
}

void GDLlama::set_penalize_nl(const bool p_penalize_nl) {
    params.sparams.penalize_nl = p_penalize_nl;
}

int32_t GDLlama::get_top_k() const {
    return params.sparams.top_k;
}

void GDLlama::set_top_k(const int32_t p_top_k) {
    params.sparams.top_k = p_top_k;
}

float GDLlama::get_top_p() const {
    return params.sparams.top_p;
}

void GDLlama::set_top_p(const float p_top_p) {
    params.sparams.top_p = p_top_p;
}

float GDLlama::get_min_p() const {
    return params.sparams.min_p;
}

void GDLlama::set_min_p(const float p_min_p) {
    params.sparams.min_p = p_min_p;
}

int32_t GDLlama::get_n_threads() const {
    return params.n_threads;
}

void GDLlama::set_n_threads(const int32_t p_n_threads) {
    params.n_threads = p_n_threads;
}

int32_t GDLlama::get_n_gpu_layer() const {
    return params.n_gpu_layers;
}

void GDLlama::set_n_gpu_layer(const int32_t p_n_gpu_layers) {
    params.n_gpu_layers = p_n_gpu_layers;
}

bool GDLlama::get_escape() const {
    return params.escape;
}

void GDLlama::set_escape(const bool p_escape) {
    params.escape = p_escape;
}

int32_t GDLlama::get_n_batch() const {
    return params.n_batch;
}

void GDLlama::set_n_batch(const int32_t p_n_batch) {
    params.n_batch = p_n_batch;
}

int32_t GDLlama::get_n_ubatch() const {
    return params.n_ubatch;
}

void GDLlama::set_n_ubatch(const int32_t p_n_ubatch) {
    params.n_ubatch = p_n_ubatch;
}

String GDLlama::generate_text_common(String prompt) {
    LOG("generate_text_common start\n");

    llama_runner.reset(
        new LlamaRunner(should_output_prompt, should_output_bos, should_output_eos)
    );

    // Remove modified antiprompt from the previouss generate_text call (e.g., instruct mode)
    params.antiprompt.clear();

    if (reverse_prompt != "") {
        LOG("Adding reverse prompt: %s\n", reverse_prompt.c_str());
        params.antiprompt.emplace_back(reverse_prompt);
    }

    std::string text = llama_runner->llama_generate_text(
        std::string(prompt.utf8().get_data()),
            params,
            [this](std::string s) {
                String new_text {s.c_str()};
                call_deferred("emit_signal", "generate_text_updated", new_text);
            },
            [this]() {
                call_deferred("emit_signal", "input_wait_started");
            },
            [this](std::string s) {
                String text {s.c_str()};
                call_deferred("emit_signal", "generate_text_finished", text);
            }
    );

    return String(text.c_str());
}

String GDLlama::generate_text_simple_internal(String prompt) {
    LOG("generate_text_simple_internal\n");

    String full_generated_text = generate_text_common(prompt);

    generate_text_mutex->unlock();
    LOG("generate_text_mutex unlocked\n");

    LOG("generate_text_simple_internal -- done\n");

    return full_generated_text;
}


String GDLlama::generate_text_simple(String prompt) {
    LOG("generate_text_simple\n");
    func_mutex->lock();

    if (!generate_text_mutex->try_lock()) {
        LOG("GDLlama is busy\n");
    }

    generate_text_mutex->lock();
    LOG("generate_text_mutex locked\n");

    func_mutex->unlock();

    String full_generated_text = generate_text_simple_internal(prompt);

    LOG("generate_text_simple -- done\n");

    return full_generated_text;
}

String GDLlama::generate_text_grammar_internal(String prompt, String grammar) {
    LOG("generate_text_grammar_internal\n");
    params.sparams.grammar = std::string(grammar.utf8().get_data());

    String full_generated_text = generate_text_common(prompt);

    params.sparams.grammar = std::string();

    generate_text_mutex->unlock();
    LOG("generate_text_mutex unlocked\n");

    LOG("generate_text_grammar_internal -- done\n");

    return full_generated_text;
}

String GDLlama::generate_text_grammar(String prompt, String grammar) {
    LOG("generate_text_grammar\n");
    func_mutex->lock();

    if (!generate_text_mutex->try_lock()) {
        LOG("GDLlama is busy\n");
    }

    generate_text_mutex->lock();
    LOG("generate_text_mutex locked\n");

    func_mutex->unlock();

    String full_generated_text = generate_text_grammar_internal(prompt, grammar);

    LOG("generate_text_grammar -- done\n");

    return full_generated_text;
}

String GDLlama::generate_text_json_internal(String prompt, String json) {
    LOG("generate_text_json_internal\n");
    std::string grammar = json_schema_to_grammar(nlohmann::ordered_json::parse(json.utf8().get_data()));
    params.sparams.grammar = grammar;

    String full_generated_text = generate_text_common(prompt);

    params.sparams.grammar = std::string();

    generate_text_mutex->unlock();
    LOG("generate_text_mutex unlocked\n");

    LOG("generate_text_json_internal -- done\n");
    return full_generated_text;
}


String GDLlama::generate_text_json(String prompt, String json) {
    LOG("generate_text_json\n");
    func_mutex->lock();

    if (!generate_text_mutex->try_lock()) {
        LOG("GDLlama is busy\n");
    }

    generate_text_mutex->lock();
    LOG("generate_text_mutex locked\n");

    func_mutex->unlock();

    String full_generated_text = generate_text_json_internal(prompt, json);

    LOG("generate_text_json -- done\n");
    return full_generated_text;
}

String GDLlama::generate_text(String prompt, String grammar, String json) {
    LOG("generate_text\n");
    func_mutex->lock();

    if (!generate_text_mutex->try_lock()) {
        LOG("GDLlama is busy\n");
    }

    generate_text_mutex->lock();
    LOG("generate_text_mutex locked\n");

    func_mutex->unlock();

    String full_generated_text;
    if (!grammar.is_empty()) {
        full_generated_text = generate_text_grammar_internal(prompt, grammar);
    } else if (!json.is_empty()) {
        full_generated_text = generate_text_json_internal(prompt, json);
    } else {
        full_generated_text = generate_text_simple_internal(prompt);
    }

    LOG("generate_text_json -- done\n");
    return full_generated_text;
}

Error GDLlama::run_generate_text(String prompt, String grammar, String json) {
    LOG("run_generate_text\n");
    func_mutex->lock();

    if (!generate_text_mutex->try_lock()) {
        LOG("GDLlama is busy\n");
    }

    generate_text_mutex->lock();
    LOG("generate_text_mutex locked\n");

    func_mutex->unlock();

    //is_started instead of is_alive to properly clean up all threads
    if (generate_text_thread->is_started()) {
        generate_text_thread->wait_to_finish();
    }

    generate_text_thread.instantiate();
    LOG("generate_text_thread instantiated\n");

    Error error;

    if (!grammar.is_empty()) {
        Callable c = callable_mp(this, &GDLlama::generate_text_grammar_internal);
        error = generate_text_thread->start(c.bind(prompt, grammar));
    } else if (!json.is_empty()) {
        Callable c = callable_mp(this, &GDLlama::generate_text_json_internal);
        error = generate_text_thread->start(c.bind(prompt, json));
    } else {
        Callable c = callable_mp(this, &GDLlama::generate_text_simple_internal);
        error = generate_text_thread->start(c.bind(prompt));
    }

    LOG("run_generate_text -- done\n");
    return error;
}

void GDLlama::stop_generate_text() {
    LOG("Stopping llama_runner\n");
    llama_runner->llama_stop_generate_text();
}

void GDLlama::input_text(String input) {
    LOG("input_text: %s\n", input.utf8().get_data());
    llama_runner->set_input(std::string(input.utf8().get_data()));
}

bool GDLlama::is_running() {
    return !generate_text_mutex->try_lock() || generate_text_thread->is_alive();
}

bool GDLlama::is_waiting_input() {
    return llama_runner->get_is_waiting_input();
}

} //namespace godot