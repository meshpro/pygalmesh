import argparse
from sys import version_info

import meshio
from _pygalmesh import _CGAL_VERSION_STR

from .__about__ import __version__
from .main import generate_from_inr, generate_volume_mesh_from_surface_mesh
from .main import remesh_surface as rms


def cli(argv=None):
    parser = argparse.ArgumentParser(
        description="pygalmesh command-line tools",
        formatter_class=argparse.RawTextHelpFormatter,
    )

    parser.add_argument(
        "--version",
        "-v",
        action="version",
        version=get_version_text(),
        help="display version information",
    )

    subparsers = parser.add_subparsers(title="subcommands")

    parser_inr = subparsers.add_parser(
        "from-inr", help="Create volume mesh from INR voxel files"
    )
    _cli_inr(parser_inr)
    parser_inr.set_defaults(func=inr)

    parser_remesh = subparsers.add_parser("remesh-surface", help="Remesh surface mesh")
    _cli_remesh(parser_remesh)
    parser_remesh.set_defaults(func=remesh_surface)

    parser_volume_from_surface = subparsers.add_parser(
        "volume-from-surface", help="Generate volume mesh from surface mesh"
    )
    _cli_volume_from_surface(parser_volume_from_surface)
    parser_volume_from_surface.set_defaults(func=volume_from_surface)

    args = parser.parse_args(argv)
    return args.func(args)


def get_version_text():
    python_ver = f"{version_info.major}.{version_info.minor}.{version_info.micro}"
    return "\n".join(
        [
            f"pygalmesh {__version__} [Python {python_ver}, CGAL {_CGAL_VERSION_STR}]"
            + "Copyright (c) 2016-2021, Nico Schlömer <nico.schloemer@gmail.com>",
        ]
    )


def inr(args):
    mesh = generate_from_inr(
        args.infile,
        lloyd=args.lloyd,
        odt=args.odt,
        perturb=args.perturb,
        exude=args.exude,
        max_edge_size_at_feature_edges=args.max_edge_size_at_feature_edges,
        min_facet_angle=args.min_facet_angle,
        max_radius_surface_delaunay_ball=args.max_radius_surface_delaunay_ball,
        max_facet_distance=args.max_facet_distance,
        max_circumradius_edge_ratio=args.max_circumradius_edge_ratio,
        max_cell_circumradius=args.max_cell_circumradius,
        verbose=not args.quiet,
    )
    meshio.write(args.outfile, mesh)


def _cli_inr(parser):
    parser.add_argument("infile", type=str, help="input INR file")

    parser.add_argument("outfile", type=str, help="output mesh file")

    parser.add_argument(
        "--lloyd",
        "-l",
        type=bool,
        default=False,
        help="Lloyd smoothing (default: false)",
    )

    parser.add_argument(
        "--odt",
        "-o",
        action="store_true",
        default=False,
        help="ODT smoothing (default: false)",
    )

    parser.add_argument(
        "--perturb",
        "-p",
        action="store_true",
        default=False,
        help="perturb (default: false)",
    )

    parser.add_argument(
        "--exude",
        "-x",
        action="store_true",
        default=False,
        help="exude (default: false)",
    )

    parser.add_argument(
        "--max-edge-size-at-feature-edges",
        "-e",
        type=float,
        default=0.0,
        help="maximum edge size at feature edges (default: 0.0)",
    )

    parser.add_argument(
        "--min-facet-angle",
        "-a",
        type=float,
        default=0.0,
        help="minimum facet angle (default: 0.0)",
    )

    parser.add_argument(
        "--max-radius-surface-delaunay-ball",
        "-s",
        type=float,
        default=0.0,
        help="maximum radius of the surface facet Delaunay ball (default: 0.0)",
    )

    parser.add_argument(
        "--max-facet-distance",
        "-d",
        type=float,
        default=0.0,
        help="maximum facet distance (default: 0.0)",
    )

    parser.add_argument(
        "--max-circumradius-edge-ratio",
        "-r",
        type=float,
        default=0.0,
        help="cell radius/edge ratio (default: 0.0)",
    )

    parser.add_argument(
        "--max-cell-circumradius",
        "-c",
        type=float,
        default=0.0,
        help="maximum cell circumradius (default: 0.0)",
    )

    parser.add_argument(
        "--quiet",
        "-q",
        action="store_true",
        default=False,
        help="quiet mode (default: False)",
    )
    return parser


def remesh_surface(args):
    mesh = rms(
        args.infile,
        # lloyd=args.lloyd,
        # odt=args.odt,
        # perturb=args.perturb,
        # exude=args.exude,
        max_edge_size_at_feature_edges=args.max_edge_size_at_feature_edges,
        min_facet_angle=args.min_facet_angle,
        max_radius_surface_delaunay_ball=args.max_radius_surface_delaunay_ball,
        max_facet_distance=args.max_facet_distance,
        verbose=not args.quiet,
    )
    meshio.write(args.outfile, mesh)


def _cli_remesh(parser):
    parser.add_argument("infile", type=str, help="input mesh file")
    parser.add_argument("outfile", type=str, help="output mesh file")

    parser.add_argument(
        "--max-edge-size-at-feature-edges",
        "-e",
        type=float,
        default=0.0,
        help="maximum edge size at feature edges (default: 0.0)",
    )

    parser.add_argument(
        "--min-facet-angle",
        "-a",
        type=float,
        default=0.0,
        help="minimum facet angle (default: 0.0)",
    )

    parser.add_argument(
        "--max-radius-surface-delaunay-ball",
        "-s",
        type=float,
        default=0.0,
        help="maximum radius of the surface facet Delaunay ball (default: 0.0)",
    )

    parser.add_argument(
        "--max-facet-distance",
        "-d",
        type=float,
        default=0.0,
        help="maximum facet distance (default: 0.0)",
    )

    parser.add_argument(
        "--quiet",
        "-q",
        action="store_true",
        default=False,
        help="quiet mode (default: False)",
    )
    return parser


def volume_from_surface(args):
    mesh = generate_volume_mesh_from_surface_mesh(
        args.infile,
        lloyd=args.lloyd,
        odt=args.odt,
        perturb=args.perturb,
        exude=args.exude,
        max_edge_size_at_feature_edges=args.max_edge_size_at_feature_edges,
        min_facet_angle=args.min_facet_angle,
        max_radius_surface_delaunay_ball=args.max_radius_surface_delaunay_ball,
        max_facet_distance=args.max_facet_distance,
        max_circumradius_edge_ratio=args.max_circumradius_edge_ratio,
        max_cell_circumradius=args.max_cell_circumradius,
        reorient=args.reorient,
        verbose=not args.quiet,
    )
    meshio.write(args.outfile, mesh)


def _cli_volume_from_surface(parser):
    parser.add_argument("infile", type=str, help="input mesh file")

    parser.add_argument("outfile", type=str, help="output mesh file")

    parser.add_argument(
        "--lloyd",
        "-l",
        type=bool,
        default=False,
        help="Lloyd smoothing (default: false)",
    )

    parser.add_argument(
        "--odt",
        "-o",
        action="store_true",
        default=False,
        help="ODT smoothing (default: false)",
    )

    parser.add_argument(
        "--perturb",
        "-p",
        action="store_true",
        default=False,
        help="perturb (default: false)",
    )

    parser.add_argument(
        "--exude",
        "-x",
        action="store_true",
        default=False,
        help="exude (default: false)",
    )

    parser.add_argument(
        "--max-edge-size-at-feature-edges",
        "-e",
        type=float,
        default=0.0,
        help="maximum edge size at feature edges (default: 0.0)",
    )

    parser.add_argument(
        "--min-facet-angle",
        "-a",
        type=float,
        default=0.0,
        help="minimum facet angle (default: 0.0)",
    )

    parser.add_argument(
        "--max-radius-surface-delaunay-ball",
        "-s",
        type=float,
        default=0.0,
        help="maximum radius of the surface facet Delaunay ball (default: 0.0)",
    )

    parser.add_argument(
        "--max-facet-distance",
        "-d",
        type=float,
        default=0.0,
        help="maximum facet distance (default: 0.0)",
    )

    parser.add_argument(
        "--max-circumradius-edge-ratio",
        "-r",
        type=float,
        default=0.0,
        help="cell radius/edge ratio (default: 0.0)",
    )

    parser.add_argument(
        "--max-cell-circumradius",
        "-c",
        type=float,
        default=0.0,
        help="maximum cell circumradius (default: 0.0)",
    )

    parser.add_argument(
        "--reorient",
        "-t",
        action="store_true",
        default=False,
        help="automatically fix face orientation (default: False)",
    )

    parser.add_argument(
        "--quiet",
        "-q",
        action="store_true",
        default=False,
        help="quiet mode (default: False)",
    )
    return parser
