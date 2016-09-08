# frentos

[The meshmaker.](https://youtu.be/JqgqgcE8Zck)

[![Build Status](https://travis-ci.org/nschloe/frentos.svg?branch=master)](https://travis-ci.org/nschloe/frentos)
[![PyPi Version](https://img.shields.io/pypi/v/frentos.svg)](https://pypi.python.org/pypi/frentos)
[![GitHub stars](https://img.shields.io/github/stars/nschloe/frentos.svg?style=social&label=Star&maxAge=2592000)](https://github.com/nschloe/frentos)

This is frentos, a Python frontend to CGAL's 3D mesh generation capabilities.
frentos aims to make it easy to create high-quality 3D volume and surface
meshes.

CGAL offers two different approaches for mesh generation:

1. Meshes defined implicitly by level sets of functions.
2. Meshes defined by a collection of bounding planes.

frentos provides a front-end to the first approach, which has the following
advantages and disadvantages:

* All boundary points are guaranteed to be in the level set within any specified
  residual. This results in smooth curved surfaces.
* Sharp intersections of subdomains (e.g., in unions or differences of sets)
  need to be specified manually (via features edges, see below), which can be
  tedious.

Accordingly the bounding-plane approach, realized by
[mshr](https://bitbucket.org/fenics-project/mshr), has the following advantages
and disadvantages:

* Smooth, curved domains are approximated by a set of bounding planes,
  resulting in more of less visible edges.
* Intersections of domains can be computed automatically, so domain unions etc.
  have sharp edges where they belong.

Other Python mesh generators are [pygmsh](https://github.com/nschloe/pygmsh) (a
frontend to [gmsh](http://gmsh.info/)) and
[MeshPy](https://github.com/inducer/meshpy).
[meshzoo](https://github.com/nschloe/meshzoo) features some examples.

### Examples

#### A simple ball
<p align="center">
<img src="https://nschloe.github.io/frentos/ball.png"/>
</p>
```python
import frentos

s = frentos.Ball([0, 0, 0], 1.0)
frentos.generate_mesh(s, 'out.mesh', cell_size=0.2)
```
CGAL's mesh generator returns Medit-files, which can be processed by, e.g.,
[meshio](https://github.com/nschloe/meshio).
```python
import meshio
vertices, cells, _, _, _ = meshio.read('out.mesh')
```
The mesh generation comes with many more options, described
[here](http://doc.cgal.org/latest/Mesh_3/). Try, for example,
```python
frentos.generate_mesh(
    s,
    'out.mesh',
    cell_size=0.2,
    edge_size=0.1,
    odt=True,
    lloyd=True,
    verbose=False
    )
```

#### Other primitive shapes
<p align="center">
<img src="https://nschloe.github.io/frentos/tetra.png"/>
</p>

frentos provides out-of-the-box support for balls, cuboids, ellpsoids, tori,
cones, cylinders, and tetrahedra. Try for example
```python
import frentos

s0 = frentos.Tetrahedron(
        [0.0, 0.0, 0.0],
        [1.0, 0.0, 0.0],
        [0.0, 1.0, 0.0],
        [0.0, 0.0, 1.0]
        )
frentos.generate_mesh(
        s0, 'out.mesh', cell_size=0.1, edge_size=0.1
        )
```

#### Domain combinations
<p align="center">
<img src="https://nschloe.github.io/frentos/ball-difference.png"/>
</p>

Supported are unions, intersections, and differences of all domains. As
mentioned above, however, the sharp intersections between two domains are not
automatically handled. Try for example
```python
import frentos

radius = 1.0
displacement = 0.5
s0 = frentos.Ball([displacement, 0, 0], radius)
s1 = frentos.Ball([-displacement, 0, 0], radius)
u = frentos.Difference(s0, s1)
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

frentos.generate_mesh(
        u,
        'out.mesh',
        feature_edges=[circ],
        cell_size=0.15,
        edge_size=edge_size,
        facet_angle=25,
        facet_size=0.15,
        cell_radius_edge_ratio=2.0
        )
```
Note that the length of the polygon legs are keps in sync with the `edge_size`
of the mesh generation. This makes sure that it fits in nicely with the rest of
the mesh.

#### Domain deformations
<p align="center">
<img src="https://nschloe.github.io/frentos/egg.png"/>
</p>

You can of coure translate, rotate, scale, and stretch any domain. Try, for
example,
```
import frentos

s = frentos.Stretch(
        frentos.Ball([0, 0, 0], 1.0),
        [1.0, 2.0, 0.0]
        )

frentos.generate_mesh(
        s,
        'out.mesh',
        cell_size=0.1
        )
```

#### Extrusion of 2D polygons
<p align="center">
<img src="https://nschloe.github.io/frentos/triangle-rotated.png"/>
</p>

frentos lets you extrude any polygon into a 3D body. It even supports
rotation alongside!
```python
import frentos

p = frentos.Polygon2D([[-0.5, -0.3], [0.5, -0.3], [0.0, 0.5]])
edge_size = 0.1
domain = frentos.Extrude(
        p,
        [0.0, 0.0, 1.0],
        0.5 * 3.14159265359,
        edge_size
        )
frentos.generate_mesh(
        domain,
        'out.mesh',
        cell_size=0.1,
        edge_size=edge_size,
        verbose=False
        )
```
Feature edges are automatically preserved here, which is why an edge length
needs to be given to `frentos.Extrude`.

#### Rotation bodies
<p align="center">
<img src="https://nschloe.github.io/frentos/circle-rotate-extr.png"/>
</p>

Polygons in the x-z-plane can also be rotated around the z-axis to yield a
rotation body.
```python
import frentos

p = frentos.Polygon2D([[0.5, -0.3], [1.5, -0.3], [1.0, 0.5]])
edge_size = 0.1
domain = frentos.ring_extrude(p, edge_size)
frentos.generate_mesh(
        domain,
        'out.mesh',
        cell_size=0.1,
        edge_size=edge_size,
        verbose=False
        )
```

#### Your own custom level set function
<p align="center">
<img src="https://nschloe.github.io/frentos/heart.png"/>
</p>
If all of the variety is not enough for you, you can define your own custom
level set function. You simply need to subclass `frentos.DomainBase` and
specify a function, e.g.,
```python
import frentos
class Heart(frentos.DomainBase):
    def __init__(self):
        super(Heart, self).__init__()
        return

    def eval(self, x):
        return (x[0]**2 + 9.0/4.0 * x[1]**2 + x[2]**2 - 1)**3 \
            - x[0]**2 * x[2]**3 - 9.0/80.0 * x[1]**2 * x[2]**3

    def get_bounding_sphere_squared_radius(self):
        return 10.0

d = Heart()
frentos.generate_mesh(d, 'out.mesh', cell_size=0.1)
```
Note that you need to specify the square of a bounding sphere radius, used as
an input to CGAL's mesh generator.

#### Surface meshes

If you're only after the surface of a body, frentos has
`generate_surface_mesh` for you. It offers fewer options (obviously,
`cell_size` is gone), but otherwise works the same way:
```python
import frentos

s = frentos.Ball([0, 0, 0], 1.0)
frentos.generate_surface_mesh(
        s,
        'out.off',
        angle_bound=30,
        radius_bound=0.1,
        distance_bound=0.1
        )
```
The output format is
[OFF](http://segeval.cs.princeton.edu/public/off_format.html) which again is
handled by [meshio](https://github.com/nschloe/meshio).

Refer to [CGAL's
documention](http://doc.cgal.org/latest/Surface_mesher/index.html) for the
options.

#### Meshes from OFF files
<p align="center">
<img src="https://nschloe.github.io/frentos/elephant.png"/>
</p>

If you have an OFF file at hand (like
[elephant.off](https://raw.githubusercontent.com/CGAL/cgal-swig-bindings/master/examples/data/elephant.off)
or [these](https://github.com/CGAL/cgal/tree/master/Surface_mesher/demo/Surface_mesher/inputs)),
frentos generates the mesh via
```python
import frentos

frentos.generate_from_off(
        'elephant.off',
        'out.mesh',
        facet_angle=25.0,
        facet_size=0.15,
        facet_distance=0.008,
        cell_radius_edge_ratio=3.0,
        verbose=False
        )
```

### Installation

For installation, frentos needs [CGAL](http://www.cgal.org/) and
[Eigen](http://eigen.tuxfamily.org/index.php?title=Main_Page) installed on your
system. They are typically available on your Linux distribution, e.g., on
Ubuntu
```
sudo apt install libcgal-dev libeigen3-dev
```
[meshio](https://github.com/nschloe/meshio) can be helpful in processing the
meshes.

#### PyPi
[frentos](https://pypi.python.org/pypi/frentos) is available via PyPi, so
with [pip](https://pypi.python.org/pypi/pip) installed (`sudo apt install
python-pip`) you can simply type
```
sudo -H pip install frentos
```
to get started.

#### Manual installation

For manual installation (if you're a developer or just really keen on getting
the bleeding edge version of frentos), there are two possibilities:

 * Get the sources, type `sudo python setup.py install`. This does the trick
   most the time.
 * As a fallback, there's a CMake-based installation. Simply go `cmake
   /path/to/sources/` and `make`.

### Distribution

To create a new release

1. bump the `__version__` number,

2. publish to PyPi and GitHub:
    ```
    $ make publish
    ```


### License

frentos is published under the [MIT license](https://en.wikipedia.org/wiki/MIT_License).
