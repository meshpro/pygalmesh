# pygalmesh

A Python frontend to [CGAL](https://www.cgal.org/)'s [3D mesh generation
capabilities](https://doc.cgal.org/latest/Mesh_3/index.html).

[![CircleCI](https://img.shields.io/circleci/project/github/nschloe/pygalmesh/master.svg)](https://circleci.com/gh/nschloe/pygalmesh/tree/master)
[![Code style: black](https://img.shields.io/badge/code%20style-black-000000.svg)](https://github.com/ambv/black)
[![PyPi Version](https://img.shields.io/pypi/v/pygalmesh.svg)](https://pypi.org/project/pygalmesh)
[![GitHub stars](https://img.shields.io/github/stars/nschloe/pygalmesh.svg?label=Stars&logo=github)](https://github.com/nschloe/pygalmesh)

pygalmesh makes it easy to create high-quality 3D volume and surface meshes.

### Background

CGAL offers two different approaches for mesh generation:

1. Meshes defined implicitly by level sets of functions.
2. Meshes defined by a set of bounding planes.

pygalmesh provides a front-end to the first approach, which has the following
advantages and disadvantages:

* All boundary points are guaranteed to be in the level set within any specified
  residual. This results in smooth curved surfaces.
* Sharp intersections of subdomains (e.g., in unions or differences of sets)
  need to be specified manually (via feature edges, see below), which can be
  tedious.

On the other hand, the bounding-plane approach (realized by
[mshr](https://bitbucket.org/fenics-project/mshr)), has the following
properties:

* Smooth, curved domains are approximated by a set of bounding planes,
  resulting in more of less visible edges.
* Intersections of domains can be computed automatically, so domain unions etc.
  have sharp edges where they belong.

Other Python mesh generators are [pygmsh](https://github.com/nschloe/pygmsh) (a
frontend to [gmsh](http://gmsh.info/)) and
[MeshPy](https://github.com/inducer/meshpy).
[meshzoo](https://github.com/nschloe/meshzoo) provides some basic canonical
meshes.

### Examples

#### A simple ball
<img src="https://nschloe.github.io/pygalmesh/ball.png" width="30%">

```python
import pygalmesh

s = pygalmesh.Ball([0, 0, 0], 1.0)
mesh = pygalmesh.generate_mesh(s, cell_size=0.2)

# mesh.points, mesh.cells, ...
```
You can write the mesh using [meshio](https://github.com/nschloe/meshio), e.g.,
```python
import meshio
meshio.write("out.vtk", mesh)
```
The mesh generation comes with many more options, described
[here](https://doc.cgal.org/latest/Mesh_3/). Try, for example,
```python
mesh = pygalmesh.generate_mesh(
    s,
    cell_size=0.2,
    edge_size=0.1,
    odt=True,
    lloyd=True,
    verbose=False
)
```

#### Other primitive shapes
<img src="https://nschloe.github.io/pygalmesh/tetra.png" width="30%">

pygalmesh provides out-of-the-box support for balls, cuboids, ellipsoids, tori,
cones, cylinders, and tetrahedra. Try for example
```python
import pygalmesh

s0 = pygalmesh.Tetrahedron(
    [0.0, 0.0, 0.0],
    [1.0, 0.0, 0.0],
    [0.0, 1.0, 0.0],
    [0.0, 0.0, 1.0]
)
mesh = pygalmesh.generate_mesh(s0, cell_size=0.1, edge_size=0.1)
```

#### Domain combinations
<img src="https://nschloe.github.io/pygalmesh/ball-difference.png" width="30%">

Supported are unions, intersections, and differences of all domains. As
mentioned above, however, the sharp intersections between two domains are not
automatically handled. Try for example
```python
import pygalmesh

radius = 1.0
displacement = 0.5
s0 = pygalmesh.Ball([displacement, 0, 0], radius)
s1 = pygalmesh.Ball([-displacement, 0, 0], radius)
u = pygalmesh.Difference(s0, s1)
```
To sharpen the intersection circle, add it as a feature edge polygon line,
e.g.,
```python
a = numpy.sqrt(radius**2 - displacement**2)
edge_size = 0.15
n = int(2*numpy.pi*a / edge_size)
circ = [
    [
        0.0,
        a * numpy.cos(i * 2*numpy.pi / n),
        a * numpy.sin(i * 2*numpy.pi / n)
    ] for i in range(n)
    ]
circ.append(circ[0])

mesh = pygalmesh.generate_mesh(
    u,
    feature_edges=[circ],
    cell_size=0.15,
    edge_size=edge_size,
    facet_angle=25,
    facet_size=0.15,
    cell_radius_edge_ratio=2.0
)
```
Note that the length of the polygon legs are kept in sync with the `edge_size`
of the mesh generation. This makes sure that it fits in nicely with the rest of
the mesh.

#### Domain deformations
<img src="https://nschloe.github.io/pygalmesh/egg.png" width="30%">

You can of course translate, rotate, scale, and stretch any domain. Try, for
example,
```python
import pygalmesh

s = pygalmesh.Stretch(
    pygalmesh.Ball([0, 0, 0], 1.0),
    [1.0, 2.0, 0.0]
)

mesh = pygalmesh.generate_mesh(s, cell_size=0.1)
```

#### Extrusion of 2D polygons
<img src="https://nschloe.github.io/pygalmesh/triangle-rotated.png" width="30%">

pygalmesh lets you extrude any polygon into a 3D body. It even supports
rotation alongside!
```python
import pygalmesh

p = pygalmesh.Polygon2D([[-0.5, -0.3], [0.5, -0.3], [0.0, 0.5]])
edge_size = 0.1
domain = pygalmesh.Extrude(
    p,
    [0.0, 0.0, 1.0],
    0.5 * 3.14159265359,
    edge_size
)
mesh = pygalmesh.generate_mesh(
    domain,
    cell_size=0.1,
    edge_size=edge_size,
    verbose=False
)
```
Feature edges are automatically preserved here, which is why an edge length
needs to be given to `pygalmesh.Extrude`.

#### Rotation bodies
<img src="https://nschloe.github.io/pygalmesh/circle-rotate-extr.png" width="30%">

Polygons in the x-z-plane can also be rotated around the z-axis to yield a
rotation body.
```python
import pygalmesh

p = pygalmesh.Polygon2D([[0.5, -0.3], [1.5, -0.3], [1.0, 0.5]])
edge_size = 0.1
domain = pygalmesh.RingExtrude(p, edge_size)
mesh = pygalmesh.generate_mesh(
    domain,
    cell_size=0.1,
    edge_size=edge_size,
    verbose=False
)
```

#### Your own custom level set function
<img src="https://nschloe.github.io/pygalmesh/heart.png" width="30%">

If all of the variety is not enough for you, you can define your own custom
level set function. You simply need to subclass `pygalmesh.DomainBase` and
specify a function, e.g.,
```python
import pygalmesh
class Heart(pygalmesh.DomainBase):
    def __init__(self):
        super(Heart, self).__init__()
        return

    def eval(self, x):
        return (x[0]**2 + 9.0/4.0 * x[1]**2 + x[2]**2 - 1)**3 \
            - x[0]**2 * x[2]**3 - 9.0/80.0 * x[1]**2 * x[2]**3

    def get_bounding_sphere_squared_radius(self):
        return 10.0

d = Heart()
mesh = pygalmesh.generate_mesh(d, cell_size=0.1)
```
Note that you need to specify the square of a bounding sphere radius, used as
an input to CGAL's mesh generator.

#### Surface meshes

If you're only after the surface of a body, pygalmesh has
`generate_surface_mesh` for you. It offers fewer options (obviously,
`cell_size` is gone), but otherwise works the same way:
```python
import pygalmesh

s = pygalmesh.Ball([0, 0, 0], 1.0)
mesh = pygalmesh.generate_surface_mesh(
    s,
    angle_bound=30,
    radius_bound=0.1,
    distance_bound=0.1
)
```
The output format is
[OFF](http://segeval.cs.princeton.edu/public/off_format.html) which again is
handled by [meshio](https://github.com/nschloe/meshio).

Refer to [CGAL's
documention](https://doc.cgal.org/latest/Surface_mesher/index.html) for the
options.

#### Volume meshes from surface meshes
<img src="https://nschloe.github.io/pygalmesh/elephant.png" width="30%">

If you have a surface mesh at hand (like
[elephant.off](http://nschloe.github.io/pygalmesh/elephant.vtu)),
pygalmesh generates a volume mesh via
```python
import pygalmesh

mesh = pygalmesh.generate_volume_mesh_from_surface_mesh(
    "elephant.vtu",
    facet_angle=25.0,
    facet_size=0.15,
    facet_distance=0.008,
    cell_radius_edge_ratio=3.0,
    verbose=False
)
```

### Installation

For installation, pygalmesh needs [CGAL](https://www.cgal.org/) and
[Eigen](http://eigen.tuxfamily.org/index.php?title=Main_Page) installed on your
system. They are typically available on your Linux distribution, e.g., on
Ubuntu
```
sudo apt install libcgal-dev libeigen3-dev
```
After that, pygalmesh can be [installed from the Python Package
Index](https://pypi.org/project/pygalmesh/), so with
```
pip3 install -U pygalmesh
```
you can install/upgrade.

[meshio](https://github.com/nschloe/meshio) (`pip3 install meshio`)
can be helpful in processing the meshes.

#### Manual installation

For manual installation (if you're a developer or just really keen on getting
the bleeding edge version of pygalmesh), there are two possibilities:

 * Get the sources, type `python3 setup.py install`. This does the trick
   most the time.
 * As a fallback, there's a CMake-based installation. Simply go `cmake
   /path/to/sources/` and `make`.

### Testing

To run the pygalmesh unit tests, check out this repository and type
```
pytest
```

### Distribution

To create a new release

1. bump the `__version__` number (in `setup.py` _and_ `src/pygalmesh.i`)

2. publish to PyPi and GitHub:
    ```
    make publish
    ```

### License

pygalmesh is published under the [MIT license](https://en.wikipedia.org/wiki/MIT_License).
