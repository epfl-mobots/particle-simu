#ifndef SIMU_TOOLS_FILESYSTEM_HPP
#define SIMU_TOOLS_FILESYSTEM_HPP

#include <boost/date_time.hpp>
#include <boost/filesystem.hpp>

namespace simu {
    namespace tools {
        std::string timestamp();
        boost::filesystem::path create_folder(const std::string folder_name);
    } // namespace tools
} // namespace simu

#endif
