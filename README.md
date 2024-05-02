# Godot LLM
Isn't it cool to utilize large language model (LLM) to generate contents for your game? LLM has great potential in NPC models, game mechanics and design assisting. Thanks for technology like [llama.cpp](https://github.com/ggerganov/llama.cpp), "small" LLM, such as [llama-3-8B](https://huggingface.co/meta-llama/Meta-Llama-3-8B), run reasonably well locally on lower-end machine without a good GPU.
I want to experiment LLM in Godot but I couldn't find any good library, so I decided to create one here.

# How to use
1. Download the zip file from the [release page](https://github.com/Adriankhl/godot-llm/releases), and unzip it to place it in the `addons` folder in your godot project
2. Download an LLM model in GGUF format (recommendation: [Meta-Llama-3-8B-Instruct-Q5_K_M.gguf](https://huggingface.co/lmstudio-community/Meta-Llama-3-8B-Instruct-GGUF/tree/main)), move the file to somewhere in your godot project
3. Now you should be able to add the `GdLlama` node, set the variables (you must point the `Model Path` to the GGUF file) and call function `generate_text(prompt: String) -> String` to generate some texts

# Demo
See [godot-llm-template](https://github.com/Adriankhl/godot-llm-template)

# Features
* `GdLlama` node
  - Function:
    * `generate_text(prompt: String) -> String`: generate text from prommpt, note that this function takes a long time to run so it is best to use this with `Thread`
    * `generate_text_json(prompt: String, json: String) -> String`: generate text in a format enforced by a [json schema](https://json-schema.org/), see [the following section](#text-generation-with-json-schema)
    * `generate_text_grammar(prompt: String, grammar: String) -> String`: generate text in a format enforced by [GBNF grammar](https://github.com/ggerganov/llama.cpp/blob/master/grammars/README.md)
    * `input_text(input: String)`: input text to interactively generate text (with either `Instruct` or `Interactive` enabled) with the model, only works if the model is waiting for intput, inputing an empty string means the model should continue to generate what it has been generating
    * `stop_generate_text()`: stop text generation from the above function
  - Signals
    * `generate_text_updated(new_text: String)`: instead of waiting the full generated text, this signal is emited whenever a new token (part of the text sequence) is generated, which forms a stream of strings
    * `input_wait_started()`: the model is now starting to wait for user input
  - Inspector variables
    * `Model Path`: location of your gguf model
    * `Instruct`: question and answer interactive mode
    * `Interactive`: custom interactive mode, you should set your `reverse_prompt`, `input_prefix`, and `input_suffix` to set up a smooth interaction
    * `Reverse Prompt`: AI stops to wait for user input after seeing this prompt being generated, a good example is "User:"
    * `Input Prefix`: append before every user input
    * `Input Suffix`: append after every user input
    * `Context Size`: number of tokens the model can process at a time
    * `N Predict`: number of new tokens to generate
    * `N Keep`: when the model run out of `context size`, it starts to forget about earlier context, set this variable to force the model to keep a number of the earliest tokens to keep the conversation relevant
    * `Temperature`: the higher the temperature, the more random the generated text
    * `N Thread`: number of cpu threads to use
    * `N GPU Layer`: number of layer offloaded to GPU
    * `N Batch`: maximum number of tokens per iteration during continuous batching
    * `N Ubatch`: maximum batch size for computation
    * `Escape`: process escape character in input prompt

# Text generation with Json schema

Suppose you want to generate a character with:
  * `name`: a string from 3 character to 20 character
  * `birthday`: a string with a specific date format
  * `weapon`: either "sword", "bow", or "wand
  * `description`: a text with minimum 10 character

You can first construct the following `_person_schema` dictionary in GDScript:
```
var _person_schema = {
	"type": "object",
	"properties": {
		"name": {
			"type": "string",
			"minLength": 3,
			"maxLength": 20,
		},
		"birthday": {
			"type": "string",
			"format": "date"
		},
		"weapon": {
			 "enum": ["sword", "bow", "wand"],
		},
		"description": {
			"type": "string",
			"minLength": 10,
		},
	},
	"required": ["name", "birthday", "weapon", "description"]
}
```

Then convert it to a json string
```
var person_schema: String = JSON.stringify(_person_schema)
```

Supposed you are interested in a "Main character in a magic world", you can generate the character using the `generate_text_json(prompt, json_scheme)` of the `GDLlama` node:
```
var generated_text: String = generate_text_json(prompt, json_scheme)
```

Note that text generation is slow, you typically want to do the computation using a new `Thread` to avoid blocking the main thread, then either keep checking if the computing thread is runing (`is_alive()`) or use a signal to notify the main thread to indicate that the computation is finished.


Now, the generated text may look like:
```
<|begin_of_text|>Main character in a magic world: {"birthday":"1997-07-22","description":"The main character is a young and ambitious magician who has been training for years to master the art of magic. They are skilled in both combat and spellcasting, and are known for their bravery and quick thinking in the heat of battle.","name":"Eira","weapon":"wand"} <|eot_id|>
```

You need a bit of further processing to get the properly formatted `json_string` back:
```
var json_string = generated_text
json_string = json_string.right(-json_string.find("{"))
json_string = json_string.left(json_string.rfind("}") + 1)
```

`json_string` should look like this:
```
{"birthday": "2000-05-12", "description": "A young wizard with a pure heart and a mischievous grin. He has a wild imagination and a love for adventure. He is always up for a challenge and is not afraid to take risks.", "name": "Eryndor Thorne", "weapon": "wand"}
```

Now, the generated data is ready, you can parse back to a dictionary or other object to use the data.
```
var dict: Dictionary = {}
var json = JSON.new()
var error = json.parse(json_string)
if (error == OK):
		dict = json.data

print(dict["name"]) ##Eryndor Thorne
```

# Already working
* Windows, Linux, Android (experimental)
* [llama.cpp](https://github.com/ggerganov/llama.cpp) text generation

# TODO
* Mac
* Automatically generate schema from data classes in GDSCript
* More [llama.cpp](https://github.com/ggerganov/llama.cpp) features
* [mlc-llm](https://github.com/mlc-ai/mlc-llm) integration

# Compile from source
Install build tools and Vulkan SDK for your operating system, then clone this repository
```
git clone https://github.com/Adriankhl/godot-llm.git
cd godot-llm
git submodule update --init --recursive
mkdir build
cd build
```

Run `cmake`.

On Windows:
```
cmake .. -GNinja -DCMAKE_C_COMPILER=clang-cl -DCMAKE_CXX_COMPILER=clang-cl -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DLLAMA_VULKAN=1 -DCMAKE_BUILD_TYPE=Release
```

On Linux:
```
cmake .. -GNinja -DLLAMA_VULKAN=1 -DCMAKE_BUILD_TYPE=Release
```

For Android, set `$NDK_PATH` to your android ndk directory, then:
```
cmake .. -GNinja -DCMAKE_TOOLCHAIN_FILE=$NDK_PATH\cmake\android.toolchain.cmake -DANDROID_ABI=arm64-v8a -DANDROID_PLATFORM=android-23 -DCMAKE_C_FLAGS="-mcpu=generic" -DCMAKE_CXX_FLAGS="-mcpu=generic" -DCMAKE_BUILD_TYPE=Release
```

You may want to adjust the compile flags for Android to suit different types of CPU.

Then compile and install by `ninja`:
```
ninja -j4
ninja install
```

The folder `../install/addons/godot_llm` can be copy directly to the `addons` folder of your godot project.
