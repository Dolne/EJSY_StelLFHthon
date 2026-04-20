from typing import Callable

import pygame
from wheel import StatusManager, Wheel

class Slot:
    def __init__(self, t: Callable[[], int], dest: pygame.Surface, width: int, index: int, x: int, manager: StatusManager):
        self.t = t
        self.dest = dest
        wheel_size = 0.9
        wheel_x = x + int(width * (1 - wheel_size) / 2)
        wheel_y = int((dest.get_height() - (width * wheel_size)) / 2)
        self.wheel = Wheel(t, dest, int(width * wheel_size), index, (wheel_x, wheel_y), manager=manager)
        self.width = width
        self.x = x
        self.index = index
        self.selected = False
        self.arrow_selected = render_arrow(width // 2, (255, 231, 74), (245, 215, 16))
        self.arrow_unselected = render_arrow(width // 2, (240, 240, 240), (236, 236, 236))
        
    def spin_to(self, i: int):
        self.wheel.spin_to(i)
        
    def set_selected(self, index: int):
        self.selected = index == self.index
    
    def render(self):
        self.dest.blit(self.arrow_selected if self.selected else self.arrow_unselected, (self.width // 4 + self.x, int(self.dest.get_height() / 2 - self.width * 0.8)))
        self.wheel.render()
        
def render_arrow(width: int, fill: tuple[int, int, int], border: tuple[int, int, int]):
    height = int(width * 0.6)
    pts = [(0,0), (width, 0), (width//2, height)]
    
    surface = pygame.Surface((width, height))
    surface.set_colorkey((0,0,0))
    surface.fill((0,0,0))
    
    pygame.draw.polygon(surface, fill, pts)
    pygame.draw.polygon(surface, border, pts, width // 10)
    
    mask = pygame.Surface((width, height))
    mask.set_colorkey((255, 255, 255))
    mask.fill((0,0,0))
    pygame.draw.polygon(mask, (255, 255, 255), pts)
    surface.blit(mask, (0, 0))
    
    return surface