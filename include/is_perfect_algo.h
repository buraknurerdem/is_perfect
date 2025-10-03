// is_perfect_algo.h
// Author: Burak Nur Erdem

#pragma once

#include <string>
#include <vector>

bool is_perfect(const std::vector<std::vector<bool>> &graph_adj_mat);

bool is_perfect_log_holes(const std::vector<std::vector<bool>> &graph_adj_mat, std::string &log);