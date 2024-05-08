#ifndef CONVERSION_H
#define CONVERSION_H

#include <vector>
#include <string>
#include <godot_cpp/variant/packed_float32_array.hpp>


namespace godot {

std::vector<float> float32_array_to_vec(PackedFloat32Array array);
PackedFloat32Array float32_vec_to_array(std::vector<float> vec);

std::string string_gd_to_std(String s);
String string_std_to_gd(std::string s);

} //namespace godot

#endif