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
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

// Include to be tested files here
#include <sdsl/suffix_arrays.hpp>
#include "rabin-karp.cpp"

inline void validate_input(int argc, char *argv[], std::int64_t& runs,
    std::int64_t& lower, std::int64_t& upper, std::int64_t& step)
{
    if (argc != 6) {
        std::cerr << "Usage: <filename> <RUNS> <LOWER> <UPPER> <STEP>" << std::endl;
        std::cerr << "<filename> is the name of the file where performance data will be written." << std::endl;
        std::cerr << "It is recommended for <filename> to have .csv extension and it should not previously exist." << std::endl;
        std::cerr << "<RUNS>: numbers of runs per test case: should be >= 32." << std::endl;
        std::cerr << "<LOWER> <UPPER> <STEP>: range of test cases." << std::endl;
        std::cerr << "These should all be positive." << std::endl;
        std::exit(EXIT_FAILURE);
    }

    // Read command line arguments
    try {
        runs = std::stoll(argv[2]);
        lower = std::stoll(argv[3]);
        upper = std::stoll(argv[4]);
        step = std::stoll(argv[5]);
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
    if (step <= 0 or lower <= 0 or upper <= 0) {
        std::cerr << "<STEP>, <LOWER> and <UPPER> have to be positive." << std::endl;
        std::exit(EXIT_FAILURE);
    }
    if (lower > upper) {
        std::cerr << "<LOWER> must be at most equal to <UPPER>." << std::endl;
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
    std::int64_t runs, lower, upper, step;
    validate_input(argc, argv, runs, lower, upper, step);

    // Set up clock variables
    std::int64_t n, i, executed_runs;
    std::int64_t total_runs_additive = runs * (((upper - lower) / step) + 1);
    std::int64_t total_runs_multiplicative = runs * (floor(log(upper / double(lower)) / log(step)) + 1);
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
    time_data << "n,t_mean" << std::endl;

    // Begin testing
    std::cout << "\033[0;36mRunning tests...\033[0m" << std::endl << std::endl;
    executed_runs = 0;
    for (n = lower; n <= upper; n += step) {
        mean_time = 0;
        // Test configuration goes here
        int count, patt = 0;
        sdsl::csa_wt<> fm_index;
        sdsl::construct(fm_index, "texts/sources200MB.txt", 1);
        char* patterns[] = ["place", "force", "order", "sixty", "state"];
        // Run to compute elapsed time
        for (i = 0; i < runs; i++) {
            // Remember to change total depending on step type
            display_progress(++executed_runs, total_runs_additive);

            sdsl::memory_monitor::start();
            begin_time = std::chrono::high_resolution_clock::now();

            // Function to test goes here
            count = sdsl::count(fm_index, patterns[patt]);

            end_time = std::chrono::high_resolution_clock::now();
            sdsl::memory_monitor::stop();

            sdsl::memory_monitor::write_memory_log<HTML_FORMAT>(std::cout);

            elapsed_time = end_time - begin_time;
            times[i] = elapsed_time.count();

            mean_time += times[i];
        }

        std::cout << "repeticiones: " << count << std::endl;
        patt = (patt + 1) % 5;

        // Compute statistics
        mean_time /= runs;

        time_data << n << "," << mean_time  << std::endl;
    }

    // This is to keep loading bar after testing
    std::cout << std::endl << std::endl;
    std::cout << "\033[1;32mDone!\033[0m" << std::endl;

    time_data.close();

    return 0;
}