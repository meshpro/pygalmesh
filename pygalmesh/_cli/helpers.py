import sys

from _pygalmesh import _CGAL_VERSION_STR

from ..__about__ import __version__


def _get_version_text():
    return "\n".join(
        [
            "pygalmesh {} [Python {}.{}.{}, CGAL {}]".format(
                __version__,
                sys.version_info.major,
                sys.version_info.minor,
                sys.version_info.micro,
                _CGAL_VERSION_STR,
            ),
            "Copyright (c) 2016-2020, Nico Schlömer <nico.schloemer@gmail.com>",
        ]
    )
