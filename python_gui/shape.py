import math
from typing import Union
import pygame

SQUARE = 0
DIAMOND = 1
TRIANGLE = 2

CONTENT_SCALE = 0.8
SMALL_SCALE = 0.6

def square_pts(width: int, scale: float):
    w = int(width * scale)
    pts = [(0, 0), (0, w), (w, w), (w, 0)]
    offset = (width - w) // 2
    return offset_pts(pts, offset, offset)

def diamond_pts(width: int, scale: float):
    w = int(width * scale)
    pts = [(w//2, 0), (w, w//2), (w//2, w), (0, w//2)]
    offset = (width - w) // 2
    return offset_pts(pts, offset, offset)

def triangle_pts(width: int, scale: float):
    w = int(width * scale)
    h = int(math.sin(math.pi/3) * w)
    pts = [(w//2, 0), (0, h), (w, h)]
    hpad = (w-h) // 2
    offset = (width - w) // 2
    return offset_pts(pts, offset, offset + hpad)

def offset_pts(pts: list[tuple[int, int]], x: Union[int, float], y: Union[int, float]):
    return [(int(x) + x0, int(y) + y0) for x0, y0 in pts]
        
def render_shape(shape: int, colour: int, size: int, width: int, i: int) -> pygame.Surface:
    surface = pygame.Surface((width, width))
    surface.set_colorkey((0,0,0))
    surface.fill((0,0,0))
    
    col = 'blue' if colour == 0 else 'orange'
    scale = 1 if size == 1 else SMALL_SCALE
    
    pts: list[tuple[int,int]] = []
    if shape == SQUARE:
        pts = square_pts(width, 0.7 * scale)
    elif shape == DIAMOND:
        pts = diamond_pts(width, 0.9 * scale)
    elif shape == TRIANGLE:
        pts = triangle_pts(width, 0.8 * scale)
    
    pygame.draw.polygon(surface, col, pts)
    
    # temporary display shape index for debugging/testing
    text = pygame.font.SysFont('Arial', 32).render(str(i), False, 'red')
    surface.blit(text, (0,0))
    
    return surface
        
