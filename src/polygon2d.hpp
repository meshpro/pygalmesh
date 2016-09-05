#ifndef POLYGON2D_HPP
#define POLYGON2D_HPP

#include "domain.hpp"

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polygon_2_algorithms.h>
#include <memory>
#include <vector>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;

namespace loom {

class Polygon2D {
  public:
  explicit Polygon2D(const std::vector<std::vector<double>> & points):
    points_(vector_to_cgal_points(points))
  {
  }

  virtual ~Polygon2D() = default;

  std::vector<K::Point_2>
  vector_to_cgal_points(const std::vector<std::vector<double>> & points)
  {
    std::vector<K::Point_2> points2(points.size());
    for (size_t i = 0; i < points.size(); i++) {
      assert(points[i].size() == 2);
      points2[i] = K::Point_2(points[i][0], points[i][1]);
    }
    return points2;
  }

  bool
  is_inside(const std::vector<double> & point)
  {
    assert(point.size() == 2);
    K::Point_2 pt(point[0], point[1]);
    switch(CGAL::bounded_side_2(points_.begin(), points_.end(), pt, K())) {
      case CGAL::ON_BOUNDED_SIDE:
        return true;
      case CGAL::ON_BOUNDARY:
        return true;
      case CGAL::ON_UNBOUNDED_SIDE:
        return false;
      default:
        return false;
    }
    return false;
  }

  private:
  const std::vector<K::Point_2> points_;
};


class Extrude: public loom::DomainBase {
  public:
  explicit Extrude(
      const std::shared_ptr<loom::Polygon2D> & poly,
      const double height
      ):
    poly_(poly),
    height_(height)
  {
  }

  virtual ~Extrude() = default;

  virtual
  double
  eval(const std::vector<double> & x) const
  {
    if (0.0 <= x[2] && x[2] <= height_) {
      const std::vector<double> x2 = {x[0], x[1]};
      const double a = poly_->is_inside(x2) ? -1.0 : 1.0;
      return poly_->is_inside(x2) ? -1.0 : 1.0;
    }
    return 1.0;
  }

  virtual
  double
  get_bounding_sphere_squared_radius() const
  {
    // TODO
    return 2.0;
  }

  // virtual
  // std::vector<std::vector<std::vector<double>>>
  // get_features() const
  // {
  //   return {};
  // };

  private:
  const std::shared_ptr<loom::Polygon2D> poly_;
  const double height_;
};

} // namespace loom

#endif // POLYGON2D_HPP
