import pygalmesh


def test_cli():
    pygalmesh._cli.inr(["--version"])
    pygalmesh._cli.volume_from_surface(["--version"])
    pygalmesh._cli.remesh_surface(["--version"])
