#ifndef DESCRIPTOR_BASE_HPP
#define DESCRIPTOR_BASE_HPP

#include <memory>
#include <vector>

namespace samsar {

    namespace simulation {
        class Simulation;
    }

    namespace desc {
        class DescriptorBase {
        public:
            DescriptorBase() {}
            virtual ~DescriptorBase() {}

            virtual void operator()(const std::shared_ptr<simulation::Simulation>) = 0;
            virtual std::vector<double> get() { return _desc; }

        protected:
            std::vector<double> _desc;
        };

        using DescriptorBasePtr = std::shared_ptr<DescriptorBase>;
    } // namespace desc
} // namespace samsar

#endif
