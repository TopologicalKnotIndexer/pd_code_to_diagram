from .main import pd_code_diagram_sanity
from .to_image import diagram_to_image, diagram_to_png


def get_diagram_from_pd_code(*args, **kwargs):
    from .main import get_diagram_from_pd_code as func

    return func(*args, **kwargs)


def get_diagram_str_from_pd_code(*args, **kwargs):
    from .main import get_diagram_str_from_pd_code as func

    return func(*args, **kwargs)


def diagram_to_pd_code(*args, **kwargs):
    from .from_diagram import diagram_to_pd_code as func

    return func(*args, **kwargs)

__all__ = [
    "get_diagram_from_pd_code",
    "get_diagram_str_from_pd_code",
    "diagram_to_pd_code",
    "diagram_to_image",
    "diagram_to_png",
    "pd_code_diagram_sanity"
]
