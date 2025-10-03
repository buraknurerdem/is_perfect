// is_perfect_experiment.cpp
// Author: Burak Nur Erdem
// This experiment program needs the igraph library
/*
// Compile Command:
clang++ -std=c++20 -O3 -o experiment/is_perfect_experiment experiment/is_perfect_experiment.cpp \
    src/is_perfect_algo.cpp src/utils.cpp -Iinclude -I/usr/local/include/igraph -L/usr/local/lib \
    -L/opt/homebrew/lib -ligraph -larpack -llapack -lblas -lm -lpthread
*/

#include "is_perfect_algo.h"
#include "utils.h"

#include <fstream>
#include <igraph.h>
#include <iostream>

int main(int argc, char *argv[])
{
    std::string path = argv[1];

    bool run_igraph = (std::string(argv[3]) == "1");

    auto adj = read_graph_adj_matrix_from_file(path);

    long long runtime_igraph;
    igraph_bool_t is_perfect_igraph;
    if (run_igraph)
    {
        // Create igraph graph
        igraph_vector_int_t edges;
        igraph_vector_int_init(&edges, 0);

        for (size_t i = 0; i < adj.size(); i++)
        {
            for (size_t j = i + 1; j < adj[i].size(); j++)
            {
                if (adj[i][j])
                {
                    igraph_vector_int_push_back(&edges, i);
                    igraph_vector_int_push_back(&edges, j);
                }
            }
        }
        igraph_t g;
        igraph_create(&g, &edges, adj.size(), IGRAPH_UNDIRECTED);

        // igraph is_perfect
        auto time_start = std::chrono::high_resolution_clock::now();
        igraph_is_perfect(&g, &is_perfect_igraph);
        auto time_end = std::chrono::high_resolution_clock::now();

        igraph_vector_int_destroy(&edges);
        igraph_destroy(&g);

        runtime_igraph = std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start).count();
    }

    // Our is_perfect
    auto time_start_ours = std::chrono::high_resolution_clock::now();
    bool is_perfect_ours = is_perfect(adj);
    auto time_end_ours = std::chrono::high_resolution_clock::now();

    auto runtime_ours =
        std::chrono::duration_cast<std::chrono::milliseconds>(time_end_ours - time_start_ours).count();

    // Summary Output to experiment csv
    std::ofstream experiment_csv_file;
    experiment_csv_file.open(argv[2], std::ios::app);
    if (!experiment_csv_file)
    {
        std::cerr << "Error opening experiment_csv_file." << std::endl;
        return 1;
    }

    Graph_File_Info graph_file_info_obj;
    parse_graph_detail(path, graph_file_info_obj);

    std::string summary;
    // Write column names only if the file is empty
    if (experiment_csv_file.tellp() == 0)
    {
        // csv columns
        if (run_igraph)
        {
            experiment_csv_file << "graph_path,type,order,density,id,runtime_igraph,is_perfect_igraph"
                                   ",runtime_ours,is_perfect_ours";
        }
        else
        {
            experiment_csv_file << "graph_path,type,order,density,id,runtime_ours,is_perfect_ours";
        }
        experiment_csv_file << std::endl;
    }

    summary += path + ",";
    summary += graph_file_info_obj.type + ",";
    summary += graph_file_info_obj.order + ",";
    summary += graph_file_info_obj.density + ",";
    summary += graph_file_info_obj.id + ",";
    if (run_igraph)
    {
        summary += std::to_string(runtime_igraph) + ",";
        summary += std::to_string(is_perfect_igraph) + ",";
    }
    summary += std::to_string(runtime_ours) + ",";
    summary += std::to_string(is_perfect_ours);

    experiment_csv_file << summary << std::endl;

    experiment_csv_file.close();

    return 0;
}