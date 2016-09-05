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
  explicit Polygon2D(const std::vector<std::vector<double>> & _points):
    points(vector_to_cgal_points(_points))
  {
  }

  virtual ~Polygon2D() = default;

  std::vector<K::Point_2>
  vector_to_cgal_points(const std::vector<std::vector<double>> & _points) const
  {
    std::vector<K::Point_2> points2(_points.size());
    for (size_t i = 0; i < _points.size(); i++) {
      assert(_points[i].size() == 2);
      points2[i] = K::Point_2(_points[i][0], _points[i][1]);
    }
    return points2;
  }

  double
  get_bounding_circle_squared_radius() const
  {
    double max = 0.0;
    for (const auto & pt: this->points) {
      const double norm = pt.x()*pt.x() + pt.y()*pt.y();
      if (norm > max) {
        max = norm;
      }
    }
  }

  bool
  is_inside(const std::vector<double> & point)
  {
    assert(point.size() == 2);
    K::Point_2 pt(point[0], point[1]);
    switch(CGAL::bounded_side_2(this->points.begin(), this->points.end(), pt, K())) {
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

  public:
  const std::vector<K::Point_2> points;
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
      return poly_->is_inside(x2) ? -1.0 : 1.0;
    }
    return 1.0;
  }

  virtual
  double
  get_bounding_sphere_squared_radius() const
  {
    return poly_->get_bounding_circle_squared_radius() + height_*height_;
  }

  virtual
  std::vector<std::vector<std::vector<double>>>
  get_features() const
  {
    std::vector<std::vector<std::vector<double>>> features = {};

    size_t n;

    // bottom polygon
    n = poly_->points.size();
    for (size_t i=0; i < n-1; i++) {
      features.push_back({
        {poly_->points[i].x(), poly_->points[i].y(), 0.0},
        {poly_->points[i+1].x(), poly_->points[i+1].y(), 0.0}
      });
    }
    features.push_back({
      {poly_->points[n-1].x(), poly_->points[n-1].y(), 0.0},
      {poly_->points[0].x(), poly_->points[0].y(), 0.0}
    });

    // top polygon
    n = poly_->points.size();
    for (size_t i=0; i < n-1; i++) {
      features.push_back({
        {poly_->points[i].x(), poly_->points[i].y(), height_},
        {poly_->points[i+1].x(), poly_->points[i+1].y(), height_}
      });
    }
    features.push_back({
      {poly_->points[n-1].x(), poly_->points[n-1].y(), height_},
      {poly_->points[0].x(), poly_->points[0].y(), height_}
    });

    // features connecting the top and bottom
    for (const auto & pt: poly_->points) {
      std::vector<std::vector<double>> line = {
        {pt.x(), pt.y(), 0.0},
        {pt.x(), pt.y(), height_}
      };
      features.push_back(line);
    }

    return features;
  };

  private:
  const std::shared_ptr<loom::Polygon2D> poly_;
  const double height_;
};

} // namespace loom

#endif // POLYGON2D_HPP
