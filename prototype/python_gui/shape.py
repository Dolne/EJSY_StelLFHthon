import math
from typing import Union
import pygame

TRIANGLE = 0
SQUARE = 1

CONTENT_SCALE = 0.8
SMALL_SCALE = 0.6

def square_pts(width: int, scale: float):
    w = int(width * scale)
    pts = [(0, 0), (0, w), (w, w), (w, 0)]
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
    
    fill = (0, 85, 255) if colour == 0 else (255, 128, 54)
    border = (0, 66, 198) if colour == 0 else (237, 96, 15)
    scale = 1 if size == 1 else SMALL_SCALE
    
    pts: list[tuple[int,int]] = []
    if shape == TRIANGLE:
        pts = triangle_pts(width, 0.8 * scale)
    elif shape == SQUARE:
        pts = square_pts(width, 0.7 * scale)
    
    pygame.draw.polygon(surface, fill, pts)
    pygame.draw.polygon(surface, border, pts, width=int(width * 0.1))
    
    # the border is drawn centered, but we only want to inner border
    # as the border does not join outer corners
    # so we mask out anything outside the shape
    mask = pygame.Surface((width, width))
    mask.set_colorkey((255, 255, 255))
    mask.fill((0,0,0))
    pygame.draw.polygon(mask, (255, 255, 255), pts)
    surface.blit(mask, (0,0))
    
    # temporary display shape index for debugging/testing
    text = pygame.font.SysFont('Arial', 32).render(str(i), False, (200, 200, 200))
    surface.blit(text, (width // 10, width // 10))
    
    return surface
