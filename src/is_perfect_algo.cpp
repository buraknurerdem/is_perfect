// is_perfect_algo.cpp
// Author: Burak Nur Erdem

#include "is_perfect_algo.h"
#include "utils.h"

void odd_hole_recursive(
    const std::vector<std::vector<bool>> &graph_adj_mat,
    const std::vector<std::vector<int>> &graph_adj_list,
    std::vector<int> &path_vector,
    std::unordered_map<std::vector<bool>, std::vector<int>> &odd_holes,
    bool is_anti_hole_search,
    int termination_batch_size
)
{

    if (termination_batch_size != 0 && (odd_holes.size() >= termination_batch_size))
    {
        return;
    }

    int last_added_v = path_vector.back();
    int path_length = path_vector.size();

    int wanted_min_cycle_length;
    // If searcing for odd anti-holes, wanted_min_cycle_length should be 7, to avoid double counting
    // C_5's
    if (is_anti_hole_search)
    {
        wanted_min_cycle_length = 7;
    }
    // If searching for odd holes wanted_min_cycle_length should be 5
    else
    {
        wanted_min_cycle_length = 5;
    }

    // for neighbors of last vertex
    for (auto i : graph_adj_list[last_added_v])
    {

        // Smaller indices of the start vertex is not considered. So an odd-hole is only identified
        // when path start with its smallest indexed vertex smallest index vertex
        if (i < path_vector[0])
            continue;

        // It skips the vertex if it the one added before last_added_v.
        if (path_length > 1 && path_vector[path_length - 2] == i)
            continue;

        // i should be skipped if it is an internal vertex in the path. there is no explicit check.
        // the chord check also manages to skip i if it is already an internal vertex

        // chord check
        bool chord_exist = false;
        for (int j = 1; (path_length > 2) && (j < (path_length - 1)); ++j)
        {
            // if chord exist, no odd hole, no need to continue the path with this vertex.
            if (graph_adj_mat[i][path_vector[j]])
            {
                chord_exist = true;
                break;
            }
        }

        // if there is no chord, then check for cycle (hole), and if cycle exists, check if it is
        // an oddhole. If it is an even-hole, pass vertex i.
        if ((!chord_exist) && (path_length > 1) && (graph_adj_mat[path_vector[0]][i]))
        {

            int cycle_length = path_length + 1;

            if (cycle_length % 2 == 1 && cycle_length >= wanted_min_cycle_length)
            { // if true, then odd hole is found

                // add found odd hole to the data structure.
                auto odd_hole = path_vector;
                odd_hole.push_back(i);

                auto cycle_vert_subset = cycle_vector_to_bool_vec(odd_hole, graph_adj_mat.size());
                odd_holes.try_emplace(cycle_vert_subset, std::move(odd_hole));
            }
        }

        // if no chord, no odd-hole, then add i to the path and continue recursion.
        else if (!chord_exist)
        {
            path_vector.push_back(i);
            odd_hole_recursive(
                graph_adj_mat, graph_adj_list, path_vector, odd_holes, is_anti_hole_search,
                termination_batch_size
            );
        }
    }

    path_vector.pop_back();
    return;
}


std::unordered_map<std::vector<bool>, std::vector<int>> find_odd_holes(
    const std::vector<std::vector<bool>> &graph_adj_mat, bool is_anti_hole_search, int termination_batch_size
)
{

    auto graph_adj_list = get_adj_list_from_adj_matrix(graph_adj_mat);
    std::unordered_map<std::vector<bool>, std::vector<int>> odd_holes;

    for (int i = 0; i < graph_adj_mat.size(); ++i)
    {
        std::vector<int> path_vector = {i};
        path_vector.reserve(graph_adj_mat.size());
        odd_hole_recursive(
            graph_adj_mat, graph_adj_list, path_vector, odd_holes, is_anti_hole_search, termination_batch_size
        );
    }

    return odd_holes;
}

bool is_perfect(const std::vector<std::vector<bool>> &graph_adj_mat)
{

    auto odd_holes = find_odd_holes(graph_adj_mat, false, 1);
    if (odd_holes.size() > 0)
    {
        return false;
    }

    auto complement_graph = get_complement_of_graph(graph_adj_mat);
    auto odd_anti_holes = find_odd_holes(complement_graph, true, 1);
    if (odd_anti_holes.size() > 0)
    {
        return false;
    }
    return true;
}

bool is_perfect_log_holes(const std::vector<std::vector<bool>> &graph_adj_mat, std::string &log)
{
    auto odd_holes = find_odd_holes(graph_adj_mat, false, 0);
    auto complement_graph = get_complement_of_graph(graph_adj_mat);
    auto odd_antiholes = find_odd_holes(complement_graph, true, 0);

    log_odd_holes(odd_holes, log, false);
    log_odd_holes(odd_antiholes, log, true);
    if ((odd_antiholes.size() > 0) || (odd_holes.size() > 0))
    {
        return false;
    }
    return true;
}