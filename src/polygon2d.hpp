#ifndef POLYGON2D_HPP
#define POLYGON2D_HPP

#include "domain.hpp"

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polygon_2_algorithms.h>
#include <array>
#include <memory>
#include <vector>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;

namespace pygalmesh {

class Polygon2D {
  public:
  explicit Polygon2D(const std::vector<std::array<double, 2>> & _points):
    points(vector_to_cgal_points(_points))
  {
  }

  virtual ~Polygon2D() = default;

  std::vector<K::Point_2>
  vector_to_cgal_points(const std::vector<std::array<double, 2>> & _points) const
  {
    std::vector<K::Point_2> points2(_points.size());
    for (size_t i = 0; i < _points.size(); i++) {
      assert(_points[i].size() == 2);
      points2[i] = K::Point_2(_points[i][0], _points[i][1]);
    }
    return points2;
  }

  bool
  is_inside(const std::array<double, 2> & point)
  {
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


class Extrude: public pygalmesh::DomainBase {
  public:
  Extrude(
      const std::shared_ptr<pygalmesh::Polygon2D> & poly,
      const std::array<double, 3> & direction,
      const double alpha = 0.0,
      const double edge_size = 0.0
      ):
    poly_(poly),
    direction_(direction),
    alpha_(alpha),
    edge_size_(edge_size)
  {
  }

  virtual ~Extrude() = default;

  virtual
  double
  eval(const std::array<double, 3> & x) const
  {
    if (x[2] < 0.0 || x[2] > direction_[2]) {
      return 1.0;
    }

    const double beta = x[2] / direction_[2];

    std::array<double, 2> x2 = {
      x[0] - beta * direction_[0],
      x[1] - beta * direction_[1]
    };

    if (alpha_ != 0.0) {
      std::array<double, 2> x3;
      // turn by -beta*alpha
      const double sinAlpha = sin(beta*alpha_);
      const double cosAlpha = cos(beta*alpha_);
      x3[0] =  cosAlpha * x2[0] + sinAlpha * x2[1];
      x3[1] = -sinAlpha * x2[0] + cosAlpha * x2[1];
      x2 = x3;
    }

    return poly_->is_inside(x2) ? -1.0 : 1.0;
  }

  virtual
  double
  get_bounding_sphere_squared_radius() const
  {
    double max = 0.0;
    for (const auto & pt: poly_->points) {
      // bottom polygon
      const double nrm0 = pt.x()*pt.x() + pt.y()*pt.y();
      if (nrm0 > max) {
        max = nrm0;
      }

      // TODO rotation

      // top polygon
      const double x = pt.x() + direction_[0];
      const double y = pt.y() + direction_[1];
      const double z = direction_[2];
      const double nrm1 = x*x + y*y + z*z;
      if (nrm1 > max) {
        max = nrm1;
      }
    }
    return max;
  }

  virtual
  std::vector<std::vector<std::array<double, 3>>>
  get_features() const
  {
    std::vector<std::vector<std::array<double, 3>>> features = {};

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

    // top polygon, R*x + d
    n = poly_->points.size();
    const double sinAlpha = sin(alpha_);
    const double cosAlpha = cos(alpha_);
    for (size_t i=0; i < n-1; i++) {
      features.push_back({
        {
        cosAlpha * poly_->points[i].x() - sinAlpha * poly_->points[i].y() + direction_[0],
        sinAlpha * poly_->points[i].x() + cosAlpha * poly_->points[i].y() + direction_[1],
        direction_[2]
        },
        {
        cosAlpha * poly_->points[i+1].x() - sinAlpha * poly_->points[i+1].y() + direction_[0],
        sinAlpha * poly_->points[i+1].x() + cosAlpha * poly_->points[i+1].y() + direction_[1],
        direction_[2]
        }
      });
    }
    features.push_back({
      {
      cosAlpha * poly_->points[n-1].x() - sinAlpha * poly_->points[n-1].y() + direction_[0],
      sinAlpha * poly_->points[n-1].x() + cosAlpha * poly_->points[n-1].y() + direction_[1],
      direction_[2]
      },
      {
      cosAlpha * poly_->points[0].x() - sinAlpha * poly_->points[0].y() + direction_[0],
      sinAlpha * poly_->points[0].x() + cosAlpha * poly_->points[0].y() + direction_[1],
      direction_[2]
      }
    });

    // features connecting the top and bottom
    if (alpha_ == 0) {
      for (const auto & pt: poly_->points) {
        std::vector<std::array<double, 3>> line = {
          {pt.x(), pt.y(), 0.0},
          {pt.x() + direction_[0], pt.y() + direction_[1], direction_[2]}
        };
        features.push_back(line);
      }
    } else {
      // Alright, we need to chop the lines on which the polygon corners are
      // sitting into pieces. How long? About edge_size. For the starting point
      // (x0, y0, z0) height h and angle alpha, the lines are given by
      //
      // f(beta) = (
      //   cos(alpha*beta) x0 - sin(alpha*beta) y0,
      //   sin(alpha*beta) x0 + cos(alpha*beta) y0,
      //   z0 + beta * h
      //   )
      //
      // with beta in [0, 1]. The length from beta0 till beta1 is then
      //
      //  l = sqrt(alpha^2 (x0^2 + y0^2) + h^2) * (beta1 - beta0).
      //
      const double height = direction_[2];
      for (const auto & pt: poly_->points) {
        const double l = sqrt(alpha_*alpha_ * (pt.x()*pt.x() + pt.y()*pt.y()) + height*height);
        assert(edge_size_ > 0.0);
        const size_t n = int(l / edge_size_ - 0.5) + 1;
        std::vector<std::array<double, 3>> line = {
          {pt.x(), pt.y(), 0.0},
        };
        for (size_t i=0; i < n; i++) {
          const double beta = double(i+1) / n;
          const double sinAB = sin(alpha_*beta);
          const double cosAB = cos(alpha_*beta);
          line.push_back({
              cosAB * pt.x() - sinAB * pt.y(),
              sinAB * pt.x() + cosAB * pt.y(),
              beta * height
              });
        }
        features.push_back(line);
      }
    }

    return features;
  };

  private:
  const std::shared_ptr<pygalmesh::Polygon2D> poly_;
  const std::array<double, 3> direction_;
  const double alpha_;
  const double edge_size_;
};


class ring_extrude: public pygalmesh::DomainBase {
  public:
  ring_extrude(
      const std::shared_ptr<pygalmesh::Polygon2D> & poly,
      const double edge_size
      ):
    poly_(poly),
    edge_size_(edge_size)
  {
    assert(edge_size > 0.0);
  }

  virtual ~ring_extrude() = default;

  virtual
  double
  eval(const std::array<double, 3> & x) const
  {
    const double r = sqrt(x[0]*x[0] + x[1]*x[1]);
    const double z = x[2];

    return poly_->is_inside({r, z}) ? -1.0 : 1.0;
  }

  virtual
  double
  get_bounding_sphere_squared_radius() const
  {
    double max = 0.0;
    for (const auto & pt: poly_->points) {
      const double nrm1 = pt.x()*pt.x() + pt.y()*pt.y();
      if (nrm1 > max) {
        max = nrm1;
      }
    }
    return max;
  }

  virtual
  std::vector<std::vector<std::array<double, 3>>>
  get_features() const
  {
    std::vector<std::vector<std::array<double, 3>>> features = {};

    for (const auto & pt: poly_->points) {
      const double r = pt.x();
      const double circ = 2 * 3.14159265359 * r;
      const size_t n = int(circ / edge_size_ - 0.5) + 1;
      std::vector<std::array<double, 3>> line;
      for (size_t i=0; i < n; i++) {
        const double alpha = (2 * 3.14159265359 * i) / n;
        line.push_back({
          r * cos(alpha),
          r * sin(alpha),
          pt.y()
        });
      }
      line.push_back(line.front());
      features.push_back(line);
    }
    return features;
  }

  private:
  const std::shared_ptr<pygalmesh::Polygon2D> poly_;
  const double edge_size_;
};

} // namespace pygalmesh

#endif // POLYGON2D_HPP
