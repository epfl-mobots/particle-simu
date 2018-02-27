#ifndef TOOLS_HPP
#define TOOLS_HPP

#include <boost/date_time.hpp>
#include <boost/filesystem.hpp>
#include <string>

namespace samsar {
    namespace tools {
        std::string timestamp()
        {
            char date[30];
            time_t date_time;
            time(&date_time);
            strftime(date, 30, "%Y-%m-%d_%H_%M_%S", localtime(&date_time));
            return std::string(date);
        }

        boost::filesystem::path create_folder(const std::string folder_name)
        {
            boost::filesystem::path folder_absolute_path
                = boost::filesystem::current_path().string() + "/" + folder_name;
            assert(!boost::filesystem::exists(folder_absolute_path));
            assert(boost::filesystem::create_directory(folder_absolute_path));
            return folder_absolute_path;
        }
    } // namespace tools
} // namespace samsar

#endif
