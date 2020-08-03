import os
import tempfile

from _pygalmesh import (
    _generate_from_inr,
    _generate_from_inr_with_subdomain_sizing,
    _generate_from_off,
    _generate_mesh,
    _generate_periodic_mesh,
    _generate_surface_mesh,
    _generate_with_sizing_field,
    _remesh_surface,
)

import meshio


def generate_mesh(
    domain,
    feature_edges=None,
    bounding_sphere_radius=0.0,
    lloyd=False,
    odt=False,
    perturb=True,
    exude=True,
    edge_size=0.0,
    facet_angle=0.0,
    facet_size=0.0,
    facet_distance=0.0,
    cell_radius_edge_ratio=0.0,
    cell_size=0.0,
    verbose=True,
    seed=0,
):
    feature_edges = [] if feature_edges is None else feature_edges

    fh, outfile = tempfile.mkstemp(suffix=".mesh")
    os.close(fh)

    _generate_mesh(
        domain,
        outfile,
        feature_edges=feature_edges,
        bounding_sphere_radius=bounding_sphere_radius,
        lloyd=lloyd,
        odt=odt,
        perturb=perturb,
        exude=exude,
        edge_size=edge_size,
        facet_angle=facet_angle,
        facet_size=facet_size,
        facet_distance=facet_distance,
        cell_radius_edge_ratio=cell_radius_edge_ratio,
        cell_size=cell_size,
        verbose=verbose,
        seed=seed,
    )

    mesh = meshio.read(outfile)
    os.remove(outfile)
    return mesh


def generate_with_sizing_field(
    domain,
    feature_edges=None,
    bounding_sphere_radius=0.0,
    lloyd=False,
    odt=False,
    perturb=True,
    exude=True,
    edge_size=0.0,
    facet_angle=0.0,
    facet_size=0.0,
    facet_distance=0.0,
    cell_radius_edge_ratio=0.0,
    cell_size=None,
    verbose=True,
    seed=0,
):
    feature_edges = [] if feature_edges is None else feature_edges

    fh, outfile = tempfile.mkstemp(suffix=".mesh")
    os.close(fh)

    _generate_with_sizing_field(
        domain,
        outfile,
        feature_edges=feature_edges,
        bounding_sphere_radius=bounding_sphere_radius,
        lloyd=lloyd,
        odt=odt,
        perturb=perturb,
        exude=exude,
        edge_size=edge_size,
        facet_angle=facet_angle,
        facet_size=facet_size,
        facet_distance=facet_distance,
        cell_radius_edge_ratio=cell_radius_edge_ratio,
        cell_size=cell_size,
        verbose=verbose,
        seed=seed,
    )

    mesh = meshio.read(outfile)
    os.remove(outfile)
    return mesh


def generate_periodic_mesh(
    domain,
    bounding_cuboid,
    lloyd=False,
    odt=False,
    perturb=True,
    exude=True,
    edge_size=0.0,
    facet_angle=0.0,
    facet_size=0.0,
    facet_distance=0.0,
    cell_radius_edge_ratio=0.0,
    cell_size=0.0,
    number_of_copies_in_output=1,
    verbose=True,
    seed=0,
):
    fh, outfile = tempfile.mkstemp(suffix=".mesh")
    os.close(fh)

    assert number_of_copies_in_output in [1, 2, 4, 8]

    _generate_periodic_mesh(
        domain,
        outfile,
        bounding_cuboid,
        lloyd=lloyd,
        odt=odt,
        perturb=perturb,
        exude=exude,
        edge_size=edge_size,
        facet_angle=facet_angle,
        facet_size=facet_size,
        facet_distance=facet_distance,
        cell_radius_edge_ratio=cell_radius_edge_ratio,
        cell_size=cell_size,
        number_of_copies_in_output=number_of_copies_in_output,
        verbose=verbose,
        seed=seed,
    )

    mesh = meshio.read(outfile)
    os.remove(outfile)
    return mesh


def generate_surface_mesh(
    domain,
    bounding_sphere_radius=0.0,
    angle_bound=0.0,
    radius_bound=0.0,
    distance_bound=0.0,
    verbose=True,
    seed=0,
):
    fh, outfile = tempfile.mkstemp(suffix=".off")
    os.close(fh)

    _generate_surface_mesh(
        domain,
        outfile,
        bounding_sphere_radius=bounding_sphere_radius,
        angle_bound=angle_bound,
        radius_bound=radius_bound,
        distance_bound=distance_bound,
        verbose=verbose,
        seed=seed,
    )

    mesh = meshio.read(outfile)
    os.remove(outfile)
    return mesh


def generate_volume_mesh_from_surface_mesh(
    filename,
    lloyd=False,
    odt=False,
    perturb=True,
    exude=True,
    edge_size=0.0,
    facet_angle=0.0,
    facet_size=0.0,
    facet_distance=0.0,
    cell_radius_edge_ratio=0.0,
    cell_size=0.0,
    verbose=True,
    reorient=False,
    seed=0,
):
    mesh = meshio.read(filename)

    fh, off_file = tempfile.mkstemp(suffix=".off")
    os.close(fh)
    meshio.write(off_file, mesh)

    fh, outfile = tempfile.mkstemp(suffix=".mesh")
    os.close(fh)

    _generate_from_off(
        off_file,
        outfile,
        lloyd=lloyd,
        odt=odt,
        perturb=perturb,
        exude=exude,
        edge_size=edge_size,
        facet_angle=facet_angle,
        facet_size=facet_size,
        facet_distance=facet_distance,
        cell_radius_edge_ratio=cell_radius_edge_ratio,
        cell_size=cell_size,
        verbose=verbose,
        reorient=reorient,
        seed=seed,
    )

    mesh = meshio.read(outfile)
    os.remove(off_file)
    os.remove(outfile)
    return mesh


def generate_from_inr(
    inr_filename,
    lloyd=False,
    odt=False,
    perturb=True,
    exude=True,
    edge_size=0.0,
    facet_angle=0.0,
    facet_size=0.0,
    facet_distance=0.0,
    cell_radius_edge_ratio=0.0,
    cell_size=0.0,
    verbose=True,
    seed=0,
):
    fh, outfile = tempfile.mkstemp(suffix=".mesh")
    os.close(fh)

    _generate_from_inr(
        inr_filename,
        outfile,
        lloyd=lloyd,
        odt=odt,
        perturb=perturb,
        exude=exude,
        edge_size=edge_size,
        facet_angle=facet_angle,
        facet_size=facet_size,
        facet_distance=facet_distance,
        cell_radius_edge_ratio=cell_radius_edge_ratio,
        cell_size=cell_size,
        verbose=verbose,
        seed=seed,
    )

    mesh = meshio.read(outfile)
    os.remove(outfile)
    return mesh


def generate_from_inr_with_subdomain_sizing(
    inr_filename,
    cell_sizes_map,
    lloyd=False,
    odt=False,
    perturb=True,
    exude=True,
    edge_size=0.0,
    facet_angle=0.0,
    facet_size=0.0,
    facet_distance=0.0,
    cell_radius_edge_ratio=0.0,
    verbose=True,
    seed=0,
):
    fh, outfile = tempfile.mkstemp(suffix=".mesh")
    os.close(fh)

    if "default" in cell_sizes_map.keys():
        default_cell_size = cell_sizes_map.pop("default")
    else:
        default_cell_size = 0.0

    cell_sizes = [cell_sizes_map[k] for k in cell_sizes_map.keys()]
    subdomain_labels = list(cell_sizes_map.keys())

    _generate_from_inr_with_subdomain_sizing(
        inr_filename,
        outfile,
        default_cell_size,
        cell_sizes,
        subdomain_labels,
        lloyd=lloyd,
        odt=odt,
        perturb=perturb,
        exude=exude,
        edge_size=edge_size,
        facet_angle=facet_angle,
        facet_size=facet_size,
        facet_distance=facet_distance,
        cell_radius_edge_ratio=cell_radius_edge_ratio,
        verbose=verbose,
        seed=seed,
    )

    mesh = meshio.read(outfile)
    os.remove(outfile)
    return mesh


def remesh_surface(
    filename,
    edge_size=0.0,
    facet_angle=0.0,
    facet_size=0.0,
    facet_distance=0.0,
    verbose=True,
    seed=0,
):
    mesh = meshio.read(filename)

    fh, off_file = tempfile.mkstemp(suffix=".off")
    os.close(fh)
    meshio.write(off_file, mesh)

    fh, outfile = tempfile.mkstemp(suffix=".off")
    os.close(fh)

    _remesh_surface(
        off_file,
        outfile,
        edge_size=edge_size,
        facet_angle=facet_angle,
        facet_size=facet_size,
        facet_distance=facet_distance,
        verbose=verbose,
        seed=seed,
    )

    mesh = meshio.read(outfile)
    os.remove(off_file)
    os.remove(outfile)
    return mesh


def saveinr(vol, h, fname):
    """
    Save a volume (described as a numpy array) to INR format.
    Code inspired by iso2mesh (http://iso2mesh.sf.net) by Q. Fang
    INPUTS:
    - vol: volume as numpy array
    - h: voxel sizes as list or numpy array
    - fname: filename for saving the inr file
    """

    fid = open(fname, "wb")
    vol_type = vol.dtype

    if vol_type == "uint8":
        btype = "unsigned fixed"
        bitlen = 8
    elif vol_type == "uint16":
        btype = "unsigned fixed"
        bitlen = 16
    elif vol_type == "float32":
        btype = "float"
        bitlen = 32
    elif vol_type == "float64":
        btype = "float"
        bitlen = 64
    else:
        raise (vol_type)

    header = (
        "#INRIMAGE-4#{8:s}\nXDIM={0:d}\nYDIM={1:d}\nZDIM={2:d}\nVDIM=1\nTYPE={3:s}\n"
        + "PIXSIZE={4:d} bits\nCPU=decm\nVX={5:f}\nVY={6:f}\nVZ={7:f}\n"
    ).format(*vol.shape, btype, bitlen, h[0], h[1], h[2], "{")

    header = header + "\n" * (256 - 4 - len(header)) + "##}\n"

    fid.write(header.encode("ascii"))
    fid.write(vol.tobytes(order="F"))


def generate_from_array(
    vol,
    h,
    lloyd=False,
    odt=False,
    perturb=True,
    exude=True,
    edge_size=0.0,
    facet_angle=0.0,
    facet_size=0.0,
    facet_distance=0.0,
    cell_radius_edge_ratio=0.0,
    cell_size=0.0,
    verbose=True,
):
    fh, inr_filename = tempfile.mkstemp(suffix=".inr")
    os.close(fh)
    saveinr(vol, h, inr_filename)
    mesh = generate_from_inr(
        inr_filename,
        lloyd,
        odt,
        perturb,
        exude,
        edge_size,
        facet_angle,
        facet_size,
        facet_distance,
        cell_radius_edge_ratio,
        cell_size,
        verbose,
    )
    os.remove(inr_filename)
    return mesh


def generate_from_array_with_subdomain_sizing(
    vol,
    h,
    cell_sizes_map,
    lloyd=False,
    odt=False,
    perturb=True,
    exude=True,
    edge_size=0.0,
    facet_angle=0.0,
    facet_size=0.0,
    facet_distance=0.0,
    cell_radius_edge_ratio=0.0,
    verbose=True,
    seed=0,
):
    assert vol.dtype in ["uint8", "uint16"]
    fh, inr_filename = tempfile.mkstemp(suffix=".inr")
    os.close(fh)
    saveinr(vol, h, inr_filename)
    mesh = generate_from_inr_with_subdomain_sizing(
        inr_filename,
        cell_sizes_map,
        lloyd,
        odt,
        perturb,
        exude,
        edge_size,
        facet_angle,
        facet_size,
        facet_distance,
        cell_radius_edge_ratio,
        verbose,
        seed,
    )
    os.remove(inr_filename)
    return mesh
