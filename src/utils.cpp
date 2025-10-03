// utils.cpp
// Author: Burak Nur Erdem

#include "utils.h"

#include <fstream>
#include <iostream>

std::vector<std::vector<bool>> read_graph_adj_matrix_from_file(std::string graph_file_name)
{
    std::ifstream graph_file(graph_file_name);
    std::vector<std::vector<bool>> graph;

    if (!graph_file)
    {
        std::cerr << "Unable to open graph file!" << std::endl;
        return graph;
    }

    std::string line;
    while (std::getline(graph_file, line))
    {
        std::vector<bool> row;
        for (const char &c : line)
        {
            if (c == '0' || c == '1')
            {
                row.push_back(c == '1');
            }
        }
        if (!row.empty())
        {
            graph.push_back(row);
        }
    }

    graph_file.close();

    return graph;
}

bool is_matrix_symmetric(const std::vector<std::vector<bool>> &mat)
{
    int matrix_len = mat.size();
    for (const auto &row : mat)
    {
        int row_len = row.size();
        if (row_len != matrix_len)
        {
            return false;
        }
    }

    for (int i = 0; i < matrix_len; i++)
    {
        for (int j = 0; j < matrix_len; j++)
        {
            if (mat[i][j] != mat[j][i])
            {
                return false;
            }
        }
    }

    return true;
}

std::string cycle_vector_to_string(std::vector<int> vec)
{

    std::sort(vec.begin(), vec.end());

    std::string cycle_string;

    for (const auto &v : vec)
    {
        cycle_string.append(std::to_string(v) + "_");
    }

    return cycle_string;
}

std::vector<bool> cycle_vector_to_bool_vec(const std::vector<int> &vec, size_t vec_size)
{
    std::vector<bool> bool_vec(vec_size, false);

    for (int i : vec)
    {
        bool_vec[i] = true;
    }

    return bool_vec;
}

std::vector<std::vector<int>> get_adj_list_from_adj_matrix(const std::vector<std::vector<bool>> &graph)
{
    int n = graph.size();
    std::vector<std::vector<int>> graph_adj_list(n);

    for (int i = 0; i < n; ++i)
    {
        for (int j = i + 1; j < n; ++j)
        {
            if (graph[i][j])
            {
                graph_adj_list[i].push_back(j);
                graph_adj_list[j].push_back(i);
            }
        }
    }
    return graph_adj_list;
}

std::vector<std::vector<bool>> get_complement_of_graph(const std::vector<std::vector<bool>> &graph)
{

    int n = graph.size();
    std::vector<std::vector<bool>> complement_graph(n, std::vector<bool>(n, false));

    for (int i = 0; i < n - 1; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (!graph[i][j] && i != j)
            {
                complement_graph[i][j] = 1;
                complement_graph[j][i] = 1;
            }
        }
    }

    return complement_graph;
}

bool path_has_valid_graph_extension(std::string path)
{
    std::string extension = path.substr(path.find_last_of('.') + 1);
    if (extension == "txt" || extension == "csv")
    {
        return true;
    }
    return false;
};

void log_odd_holes(
    const std::unordered_map<std::vector<bool>, std::vector<int>> &odd_holes, std::string &log, bool is_anti
)
{
    if (!is_anti)
        log += "Number of odd holes: " + std::to_string(odd_holes.size()) + "\n";
    else
        log += "Number of odd antiholes: " + std::to_string(odd_holes.size()) + "\n";

    if (!odd_holes.empty())
    {
        if (is_anti)
            log += "Printing odd holes:\n";
        else
            log += "Printing odd antiholes:\n";

        for (const auto &[key, cycle] : odd_holes)
        {
            for (const auto &v : cycle)
            {
                log += std::to_string(v) + ", ";
            }
            log += "\n";
        }
    }
    return;
}

bool parse_graph_detail(const std::string &path, Graph_File_Info &detail)
{
    size_t last_slash = path.find_last_of("/\\");
    std::string filename = (last_slash == std::string::npos) ? path : path.substr(last_slash + 1);

    if (filename.size() < 4 || filename.substr(filename.size() - 4) != ".txt")
        return false;
    std::string base = filename.substr(0, filename.size() - 4);

    size_t last_underscore = base.rfind('_');
    if (last_underscore == std::string::npos)
        return false;
    detail.id = base.substr(last_underscore + 1);

    base = base.substr(0, last_underscore);
    last_underscore = base.rfind('_');
    if (last_underscore == std::string::npos)
        return false;
    detail.density = base.substr(last_underscore + 1);

    base = base.substr(0, last_underscore);
    last_underscore = base.rfind('_');
    if (last_underscore == std::string::npos)
        return false;
    detail.order = base.substr(last_underscore + 1);

    base = base.substr(0, last_underscore);
    last_underscore = base.rfind('_');
    if (last_underscore == std::string::npos)
        return false;
    detail.type = base.substr(last_underscore + 1);

    return true;
}