import sys

from _pygalmesh import _get_cgal_version

from ..__about__ import __version__


def _get_version_text():
    return "\n".join(
        [
            "pygalmesh {} [Python {}.{}.{}, CGAL {}]".format(
                __version__,
                sys.version_info.major,
                sys.version_info.minor,
                sys.version_info.micro,
                _get_cgal_version(),
            ),
            "Copyright (c) 2016-2020, Nico Schlömer <nico.schloemer@gmail.com>",
        ]
    )
