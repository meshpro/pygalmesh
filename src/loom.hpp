#ifndef LOOM_HPP
#define LOOM_HPP

#include <memory>

#include "domain.hpp"

void generate_mesh(
    const std::shared_ptr<loom::DomainBase> & in,
    const bool lloyd = false,
    const bool odt = false,
    const bool perturb = true,
    const bool exude = true
    );

#endif // LOOM_HPP
