// is_perfect.cpp
// Author: Burak Nur Erdem
// Compile Command:
// clang++ -std=c++20 -O3 -Iinclude -o is_perfect src/*.cpp

#include "is_perfect_algo.h"
#include "utils.h"

#include <chrono>
#include <filesystem>
#include <getopt.h>
#include <iomanip>
#include <iostream>


struct Program_Options
{
    std::string graph_file_path;
    std::string graph_folder_path;
    std::string experiment_csv_path;

    bool verbose_perfect = false;
    bool verbose_nonperfect = false;
    bool verbose_holes = false;

    bool is_run_for_folder = false;
    bool is_run_for_file = false;
};

struct Program_Log
{
    unsigned int n_perfect = 0;
    unsigned int n_nonperfect = 0;

    std::chrono::high_resolution_clock::time_point time_start;
    std::chrono::high_resolution_clock::time_point time_end;
};

void print_usage()
{
    std::cout << "Usage: is_perfect [OPTIONS]\n"
              << "Options:\n"
              << "\nProgram \"is_perfect\" is used to determine whether a graph is perfect "
                 "or not. It can also enumerate present odd holes and odd antiholes in the graph. "
                 "The input is either a single graph or a folder of graphs.\n";

    const int col_width = 30;
    std::cout << std::left << std::setw(col_width) << "  -i, --input FILE"
              << "Path to input graph file\n"
              << std::setw(col_width) << "  -f, --folder FOLDER"
              << "Path to folder which contains the graph files\n"
              << std::setw(col_width) << "  -p, --perfect"
              << "Log perfect graphs\n"
              << std::setw(col_width) << "  -n, --nonperfect"
              << "Log non-perfect graphs\n"
              << std::setw(col_width) << "  -o, --holes"
              << "Log odd holes and odd antiholes\n"
              << std::setw(col_width) << "  -h, --help"
              << "Show this help message\n";
}

void run_for_graph_file(std::string &path, Program_Options &options, Program_Log &program_log)
{
    // File Checks
    if (!std::filesystem::is_regular_file(path))
    {
        std::cout << "Not a regular file. Skipped: " << path << '\n';
        return;
    }
    if (!path_has_valid_graph_extension(path))
    {
        return;
    }

    // Read Graph
    const auto graph = read_graph_adj_matrix_from_file(path);
    if (!is_matrix_symmetric(graph))
    {
        std::cout << "Adjacency matrix is not symmetric: " << path << '\n';
    }

    // Extract info for experiment
    Graph_File_Info graph_file_info_obj;
    parse_graph_detail(path, graph_file_info_obj);

    // Solve
    std::string log_hole_str = "";
    bool result;
    auto time_start = std::chrono::high_resolution_clock::now();
    if (options.verbose_holes)
    {
        result = is_perfect_log_holes(graph, log_hole_str);
    }
    else
    {
        result = is_perfect(graph);
    }
    auto time_end = std::chrono::high_resolution_clock::now();

    // Prints
    if (options.verbose_perfect && result)
        std::cout << "Perfect: " << path << std::endl;
    if (options.verbose_nonperfect && !result)
        std::cout << "Non-perfect: " << path << std::endl;
    if (options.verbose_holes)
        std::cout << log_hole_str << std::endl;

    result ? program_log.n_perfect++ : program_log.n_nonperfect++;

    return;
}

int main(int argc, char *argv[])
{
    // Initial new line
    std::cout << '\n';

    // if no argument is given, print help
    if (argc == 1)
    {
        print_usage();
        return 1;
    }

    Program_Options program_options;
    Program_Log program_log;
    program_log.time_start = std::chrono::high_resolution_clock::now();

    const struct option long_opts[] = {
        {"help", no_argument, nullptr, 'h'},
        {"input", required_argument, nullptr, 'i'},
        {"folder", required_argument, nullptr, 'f'},
        {"perfect", no_argument, nullptr, 'p'},
        {"nonperfect", no_argument, nullptr, 'n'},
        {"holes", no_argument, nullptr, 'o'},
        {nullptr, 0, nullptr, 0}
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "hi:f:pno", long_opts, nullptr)) != -1)
    {
        switch (opt)
        {
        case 'h':
            print_usage();
            return 0;
        case 'f':
            program_options.is_run_for_folder = true;
            program_options.graph_folder_path = optarg;
            break;
        case 'i':
            program_options.is_run_for_file = true;
            program_options.graph_file_path = optarg;
            program_options.verbose_perfect = true;
            program_options.verbose_nonperfect = true;
            break;
        case 'p':
            program_options.verbose_perfect = true;
            break;
        case 'n':
            program_options.verbose_nonperfect = true;
            break;
        case 'o':
            program_options.verbose_holes = true;
            break;
        default:
            print_usage();
            return 1;
        }
    }

    //  Initial Checks
    if (program_options.is_run_for_folder && program_options.is_run_for_file)
    {
        std::cerr << "Error: Either run for a file or a folder.\n"
                  << "       --input (-i) and --folder (-f) cannot be given simultaneously.\n";
        return 1;
    }
    else if (!program_options.is_run_for_folder && !program_options.is_run_for_file)
    {
        std::cerr << "Error: Requires either an input graph file or a folder that contains graphs.\n"
                  << "       Need one of --input (-i) and --folder (-f).\n";
        return 1;
    }

    // Main process, for a folder
    if (program_options.is_run_for_folder)
    {
        if (!std::filesystem::exists(program_options.graph_folder_path) ||
            !std::filesystem::is_directory(program_options.graph_folder_path))
        {
            std::cerr << "Folder does not exist: " << program_options.graph_folder_path << "\n";
            return 1;
        }

        std::cout << "Running is_perfect for folder: " << program_options.graph_folder_path << "\n\n";

        // Count graphs in the folder
        int n_graphs = 0;
        for (const auto &file : std::filesystem::directory_iterator(program_options.graph_folder_path))
        {
            std::string file_path_str = file.path().string();
            if (!path_has_valid_graph_extension(file_path_str))
            {
                std::cout << "File extension not \"txt\" or \"csv\". Skipped: " << file_path_str << "\n\n";
            }
            else
            {
                n_graphs++;
            }
        }

        // Number of characters in the progress bar
        int bar_width = 50;
        if (n_graphs < bar_width)
            bar_width = n_graphs;

        // Iterate through files, run the program
        int i = 0;
        for (const auto &file : std::filesystem::directory_iterator(program_options.graph_folder_path))
        {
            std::string file_path_str = file.path().string();

            // Skip file if it has not valid extension.
            if (!path_has_valid_graph_extension(file_path_str))
                continue;

            // Progress bar output. Displayed when there is no console output of the runs
            if (!program_options.verbose_perfect && !program_options.verbose_nonperfect &&
                !program_options.verbose_holes)
            {

                double progress_width = static_cast<double>(i) / n_graphs * bar_width;
                std::cout << "\rProgress: [";
                for (int j = 0; j < bar_width; j++)
                {
                    if (j < progress_width)
                        std::cout << "#";
                    else
                        std::cout << " ";
                }
                std::cout << "] " << i << " / " << n_graphs << std::flush;
                i++;
            }

            run_for_graph_file(file_path_str, program_options, program_log);
        }
        // Progress Bar finished
        if (!program_options.verbose_perfect && !program_options.verbose_nonperfect &&
            !program_options.verbose_holes)
        {
            std::cout << "\rProgress: [";
            for (int j = 0; j < bar_width; j++)
                std::cout << "#";
            std::cout << "] " << i << " / " << n_graphs << std::endl;
        }

        // Print the number of perfect and non-perfect graphs
        const int col_width = 30;
        std::cout << "\n";
        std::cout << std::left << std::setw(col_width) << "Number of perfect graphs";
        std::cout << std::right << std::setw(10) << program_log.n_perfect << "\n";
        std::cout << std::left << std::setw(col_width) << "Number of non-perfect graphs";
        std::cout << std::right << std::setw(10) << program_log.n_nonperfect << "\n";
    }
    // Main process, for a file
    else
    {
        if (!std::filesystem::exists(program_options.graph_file_path))
        {
            std::cerr << "File does not exist: " << program_options.graph_file_path << "\n";
            return 1;
        }

        std::cout << "Running is_perfect for file: " << program_options.graph_file_path << "\n\n";

        run_for_graph_file(program_options.graph_file_path, program_options, program_log);
    }

    program_log.time_end = std::chrono::high_resolution_clock::now();
    auto total_time =
        std::chrono::duration_cast<std::chrono::seconds>(program_log.time_end - program_log.time_start)
            .count();

    std::cout << "\nTotal Runtime: " << total_time << " seconds\n\n";

    return 0;
}