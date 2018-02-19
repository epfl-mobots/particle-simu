#ifndef TOOLS_HPP
#define TOOLS_HPP

#include <boost/date_time.hpp>
#include <boost/filesystem.hpp>
#include <string>

namespace tools {
    std::string timestamp()
    {
        boost::posix_time::ptime local_time = boost::posix_time::second_clock::local_time();
        return std::to_string(local_time.date().year()) + std::to_string(local_time.date().day())
            + std::to_string(local_time.date().month()) + std::to_string(local_time.time_of_day().hours())
            + std::to_string(local_time.time_of_day().minutes()) + std::to_string(local_time.time_of_day().seconds());
    }

    boost::filesystem::path create_folder(const std::string folder_name)
    {
        boost::filesystem::path folder_absolute_path = boost::filesystem::current_path().string() + "/" + folder_name;
        assert(!boost::filesystem::exists(folder_absolute_path));
        assert(boost::filesystem::create_directory(folder_absolute_path));
        return folder_absolute_path;
    }
} // namespace tools

#endif
