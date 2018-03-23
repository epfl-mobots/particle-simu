#ifndef STAT_BASE_HPP
#define STAT_BASE_HPP

#include <cassert>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>

namespace samsar {

    namespace simulation {
        class Simulation;
    }

    namespace stat {

        class StatBase {
        public:
            StatBase() {}
            virtual ~StatBase();

            virtual void operator()(const std::shared_ptr<simulation::Simulation>) = 0;

        protected:
            void _create_log_file(
                std::shared_ptr<simulation::Simulation> s, const std::string& filename);

            mutable std::shared_ptr<std::ofstream> _log_file;
        };

        using StatBasePtr = std::shared_ptr<StatBase>;
    } // namespace stat
} // namespace samsar

#endif
