#ifndef FISH_GROUP_HPP
#define FISH_GROUP_HPP

#include "fish_individual.hpp"
#include "heading.hpp"

#include <algorithm>
#include <map>
#include <vector>

#include <boost/range.hpp>
#include <boost/range/algorithm_ext.hpp>
#include <boost/range/irange.hpp>

namespace samsar {
    namespace types {
        struct InvertedFishTable {
            template <typename FishType>
            std::map<int, std::vector<size_t>> operator()(const std::vector<FishType>& fish)
            {
                std::map<int, std::vector<size_t>> ipos;
                for (size_t i = 0; i < fish.size(); ++i) {
                    if (ipos.find(fish[i]->position().x) == ipos.end())
                        ipos[fish[i]->position().x] = std::vector<size_t>();
                    ipos.at(fish[i]->position().x).push_back(i);
                }
                return ipos;
            }
        };

        using namespace simulation;

        namespace defaults {
            struct WeightFunc {
                WeightFunc(const std::vector<float> w) : _w(w) {}
                virtual ~WeightFunc() {}

                virtual float operator()(const std::shared_ptr<Simulation> sim,
                    const FishIndividualPtr& ff, const FishIndividualPtr& f) const = 0;

                std::vector<float> _w;
            };
        } // namespace defaults

        class FishGroup {
        public:
            FishGroup(const std::vector<size_t>& idcs);

            bool has(int id) const;
            void clear();

            Heading sum_heading(const std::vector<FishIndividualPtr>& fish) const;
            Heading weighted_heading(const std::shared_ptr<Simulation> sim,
                const FishIndividualPtr& focal_fish,
                const std::shared_ptr<defaults::WeightFunc>& weight_func) const;

            size_t size() const;
            std::vector<size_t>& idcs();
            std::vector<size_t> idcs() const;

        private:
            std::vector<size_t> _idcs;
        };

    } // namespace types
} // namespace samsar

#endif
