// utils.h
// Author: Burak Nur Erdem

#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <cstddef>
#include <functional>

struct Graph_File_Info
{
    std::string type, order, density, id;
};

std::vector<std::vector<bool>> read_graph_adj_matrix_from_file(std::string graph_file_name);

bool is_matrix_symmetric(const std::vector<std::vector<bool>> &mat);

std::string cycle_vector_to_string(std::vector<int> vec);

std::vector<bool> cycle_vector_to_bool_vec(const std::vector<int> &vec, size_t vec_size);

std::vector<std::vector<int>> get_adj_list_from_adj_matrix(const std::vector<std::vector<bool>> &graph);

std::vector<std::vector<bool>> get_complement_of_graph(const std::vector<std::vector<bool>> &graph);

bool path_has_valid_graph_extension(std::string path);

void log_odd_holes(
    const std::unordered_map<std::vector<bool>, std::vector<int>> &odd_holes, std::string &log, bool is_anti
);

bool parse_graph_detail(const std::string &path, Graph_File_Info &detail);