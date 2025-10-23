/** uhr: generic time performance tester
 * Author: LELE
 *
 * Things to set up:
 * 0. Includes: include all files to be tested,
 * 1. Time unit: in elapsed_time,
 * 2. What to write on time_data,
 * 3. Data type and distribution of RNG,
 * 4. Additive or multiplicative stepping,
 * 5. The experiments: in outer for loop. */

#include <algorithm>
#include <cassert>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

// Include to be tested files here
#include <sdsl/suffix_arrays.hpp>
#include "rabin-karp.cpp"

inline void validate_input(int argc, char *argv[], std::int64_t& runs)
{
    if (argc != 3) {
        std::cerr << "Usage: <filename> <RUNS>" << std::endl;
        std::cerr << "<filename> is the name of the file where performance data will be written." << std::endl;
        std::cerr << "It is recommended for <filename> to have .csv extension and it should not previously exist." << std::endl;
        std::cerr << "<RUNS>: numbers of runs per test case: should be >= 32." << std::endl;
        std::cerr << "These should all be positive." << std::endl;
        std::exit(EXIT_FAILURE);
    }

    // Read command line arguments
    try {
        runs = std::stoll(argv[2]);
    } catch (std::invalid_argument const& ex) {
        std::cerr << "std::invalid_argument::what(): " << ex.what() << std::endl;
        std::exit(EXIT_FAILURE);
    } catch (std::out_of_range const& ex) {
        std::cerr << "std::out_of_range::what(): " << ex.what() << std::endl;
        std::exit(EXIT_FAILURE);
    }

    // Validate arguments
    if (runs < 4) {
        std::cerr << "<RUNS> must be at least 4." << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

inline void display_progress(std::int64_t u, std::int64_t v)
{
    const double progress = u / double(v);
    const std::int64_t width = 70;
    const std::int64_t p = width * progress;
    std::int64_t i;

    std::cout << "\033[1m[";
    for (i = 0; i < width; i++) {
        if (i < p)
            std::cout << "=";
        else if (i == p)
            std::cout << ">";
        else
            std::cout << " ";
    }
    std::cout << "] " << std::int64_t(progress * 100.0) << "%\r\033[0m";
    std::cout.flush();
}

int main(int argc, char *argv[])
{
    // Validate and sanitize input
    std::int64_t runs;
    validate_input(argc, argv, runs);

    // Set up clock variables
    std::int64_t n, i, executed_runs;
    std::vector<double> times(runs);
    std::vector<double> q;
    double mean_time;
    auto begin_time = std::chrono::high_resolution_clock::now();
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::nano> elapsed_time = end_time - begin_time;

    // Set up random number generation
    std::random_device rd;
    std::mt19937_64 rng(rd());
    std::uniform_int_distribution<std::int64_t> u_distr; // change depending on app

    // File to write time data
    std::ofstream time_data;
    time_data.open(argv[1]);


    // // Experimento (FM-index)
    // time_data << "file,pattern,pattern_length,reps,size,t_mean" << std::endl;

    // Experimento (Rabin-Karp)
    time_data << "file,pattern,pattern_length,reps,t_mean" << std::endl;


    // Begin testing
    std::cout << "\033[0;36mRunning tests...\033[0m" << std::endl << std::endl;
    executed_runs = 0;

    int count, min = 1, max, file = 0, idx_pattern = 0;

    // // Experimento en base al largo del texto
    // std::string files[] = {"FM_index/FM_sources200MB.sdsl", "FM_index/FM_dna400MB.sdsl", "FM_index/FM_english1024MB.sdsl"};
    // std::string patterns[] = {"flags", "TCTTG", "place"};

    // Experimento en base al largo del patron
    std::string files[] = {"FM_index/FM_english1024MB.sdsl"};
    std::string patterns[] = {"place", "These are ", "those playing a", "where wraps were che",
    "alcoholically boastful of", " been swung from the ceilings."};

    max = std::size(patterns);

    std::int64_t total_runs_additive = runs * (((max - min)) + 1);
    std::int64_t total_runs_multiplicative = runs * (floor(log(max / double(min))) + 1);

    for (n = min; n <= max; n += 1) {
        mean_time = 0;
        // Test configuration goes here
        std::string first_part = "FM_index/";
        std::string last_part = ".sdsl";
        std::string file_name = files[file];
        file_name = file_name.erase(file_name.find(first_part), first_part.length());
        file_name = file_name.erase(file_name.find(last_part), last_part.length());

        // // Experimento FM-index
        // sdsl::csa_wt<> fm_index;
        // sdsl::load_from_file(fm_index, files[file]);

        // Experimento Rabin-Karp
        std::string text;
        std::ifstream input_file(files[file]);
        if (input_file.is_open()){
            // Reserva memoria para evitar reasignaciones
            input_file.seekg(0, std::ios::end);
            text.reserve(input_file.tellg());
            input_file.seekg(0, std::ios::beg);
            
            // Lee el contenido del archivo a la string
            text.assign((std::istreambuf_iterator<char>(input_file)),
                         (std::istreambuf_iterator<char>()));

            input_file.close();

        } else {
            std::cerr << "Error: No se pudo abrir el archivo " << files[file] << std::endl;
            return 1;
        }


        // Run to compute elapsed time
        for (i = 0; i < runs; i++) {
            // Remember to change total depending on step type
            display_progress(++executed_runs, total_runs_additive);

            
            begin_time = std::chrono::high_resolution_clock::now();

            // Function to test goes here

            // // Experimento FM-index
            // count = sdsl::count(fm_index, patterns[idx_pattern]);

            // Experimento Rabin-Karp
            count = searchPattern(text, patterns[idx_pattern]).size();

            end_time = std::chrono::high_resolution_clock::now();

            elapsed_time = end_time - begin_time;
            times[i] = elapsed_time.count();

            mean_time += times[i];
        }
        // Compute statistics
        mean_time /= runs;


        // // Experimento (FM-index)
        // time_data << file_name << "," << patterns[idx_pattern] << "," << patterns[idx_pattern].length() << "," << count << "," << sdsl::size_in_bytes(fm_index) << "," << mean_time << std::endl;

        // Experimento (Rabin-Karp)
        time_data << file_name << "," << patterns[idx_pattern] << "," << patterns[idx_pattern].length() << "," << count << "," << mean_time << std::endl;

        file = (file + 1) % std::size(files);
        idx_pattern = (idx_pattern + 1) % std::size(patterns);

    }

    // This is to keep loading bar after testing
    std::cout << std::endl << std::endl;
    std::cout << "\033[1;32mDone!\033[0m" << std::endl;

    time_data.close();

    return 0;
}