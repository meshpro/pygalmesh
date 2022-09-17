from _pygalmesh import _CGAL_VERSION_STR

try:
    # Python 3.8+
    from importlib import metadata
except ImportError:
    import importlib_metadata as metadata

try:
    __version__ = metadata.version("pygalmesh")
except Exception:
    __version__ = "unknown"

__cgal_version__ = _CGAL_VERSION_STR
