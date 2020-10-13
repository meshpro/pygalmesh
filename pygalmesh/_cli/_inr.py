import argparse

import meshio

from ..main import generate_from_inr
from .helpers import _get_version_text


def inr(argv=None):
    parser = _get_inr_parser()
    args = parser.parse_args(argv)

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


def _get_inr_parser():
    parser = argparse.ArgumentParser(
        description="Generate volume mesh from INR voxel file.",
        formatter_class=argparse.RawTextHelpFormatter,
    )

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

    parser.add_argument(
        "--version", "-v", action="version", version=_get_version_text()
    )
    return parser
