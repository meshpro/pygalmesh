#ifndef SIZING_FIELD_HPP
#define SIZING_FIELD_HPP

#include <array>
#include <memory>

namespace pygalmesh {

class SizingFieldBase
{
  public:

  virtual ~SizingFieldBase() = default;

  virtual
  double
  eval(const std::array<double, 3> & x) const = 0;
};

} // namespace pygalmesh
#endif // SIZING_FIELD_HPP
