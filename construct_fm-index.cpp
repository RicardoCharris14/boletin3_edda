#include <sdsl/suffix_arrays.hpp>
#include <iostream>

int main()
{
    std::string name = "english1024MB";
    std::string file_name = "texts/" + name + ".txt";
    std::string file_fm_index = "FM_index/FM_" + name + ".sdsl";
    std::string file_memory_log = "memory/FM_" + name + "_memory_log.html";
    std::string file_memory_distribution = "memory/FM_" + name + "_structure_memory.html";

    sdsl::csa_wt<> fm_index;

    sdsl::memory_monitor::start();

    sdsl::construct(fm_index, file_name, 1);

    sdsl::memory_monitor::stop();

    std::ofstream ofs1(file_memory_log);
    sdsl::memory_monitor::write_memory_log<sdsl::HTML_FORMAT>(ofs1);

    std::ofstream ofs2(file_memory_distribution);
    sdsl::write_structure<sdsl::HTML_FORMAT>(fm_index, ofs2);

    sdsl::store_to_file(fm_index, file_fm_index);
    
    std::cout << "Tamaño de la estructura en bytes: " << sdsl::size_in_bytes(fm_index) << std::endl;
}