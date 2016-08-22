#ifndef GENERATE_HPP
#define GENERATE_HPP

#include "domain.hpp"

#include <memory>

namespace loom {

void generate_mesh(
    const std::shared_ptr<loom::DomainBase> & in,
    const double bounding_sphere_radius,
    const bool lloyd = false,
    const bool odt = false,
    const bool perturb = true,
    const bool exude = true,
    const double boundary_precision = 1.0e-4
    );

} // namespace loom

#endif // GENERATE_HPP
