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
        
    def spin_to(self, i: int):
        self.wheel.spin_to(i)
        
    def set_selected(self, index: int):
        self.selected = index == self.index
    
    def render(self):
        pygame.draw.polygon(self.dest, (0,240,0) if self.selected else (64,64,64), arrow_pts(self.width // 2, (self.width // 4 + self.x, int(self.dest.get_height() / 2 - self.width * 1.1))))
        self.wheel.render()
        

def arrow_pts(width: int, coords: tuple[int, int]):
    w = width // 6
    arrow = [(2 * w,0), (4 * w, 0), (4 * w, 4 * w), (6 * w, 4 * w), (3 * w, 7 * w), (0, 4 * w), (2 * w, 4 * w)]
    return [(x + coords[0], y + coords[1]) for x, y in arrow]