#include "conversion.h"
#include <vector>
#include <godot_cpp/variant/packed_float32_array.hpp>
#include <godot_cpp/variant/string.hpp>

namespace godot {

std::vector<float> float32_array_to_vec(PackedFloat32Array array) {
    std::vector<float> vec {};
    for (float f : array) {
        vec.push_back(f);
    }
    return vec;
}

PackedFloat32Array float32_vec_to_array(std::vector<float> vec) {
    PackedFloat32Array array {};
    for (float f : vec) {
        array.push_back(f);
    }
    return array;
}

std::string string_gd_to_std(String s) {
    return s.utf8().get_data();
}

String string_std_to_gd(std::string s) {
    return String(s.c_str());
}

} //namespace godot