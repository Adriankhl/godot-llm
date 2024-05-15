#ifndef CONVERSION_HPP
#define CONVERSION_HPP

#include <vector>
#include <string>
#include <godot_cpp/variant/packed_float32_array.hpp>
#include <godot_cpp/variant/string.hpp>


namespace godot {

std::vector<float> float32_array_to_vec(PackedFloat32Array array);
PackedFloat32Array float32_vec_to_array(std::vector<float> vec);

std::string string_gd_to_std(String s);
String string_std_to_gd(std::string s);

bool is_utf8(const char * string);

} //namespace godot

#endif
