#ifndef GENERATE_HPP
#define GENERATE_HPP

#include "domain.hpp"

#include <memory>

namespace loom {

void generate_mesh(
    const std::shared_ptr<loom::DomainBase> & in,
    const bool lloyd = false,
    const bool odt = false,
    const bool perturb = true,
    const bool exude = true
    );

} // namespace loom

#endif // GENERATE_HPP
