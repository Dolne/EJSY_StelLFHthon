import math
import random
from typing import Callable, Union

import pygame
import shape
from anim import Animation
from paho.mqtt.client import Client

class StatusManager:
    def __init__(self, count: int, client: Client):
        self.slot_status = [False for _ in range(count)]
        self.client = client
    
    def update_status(self, index: int, status: bool):
        prev = any(self.slot_status)
        if index > 0 and index <= len(self.slot_status):
            # index starts from 1 so we -1 to make it 0-indexed
            self.slot_status[index-1] = status
        curr = any(self.slot_status)
        if curr != prev:
            self.client.publish('display/status', int(curr))

class Wheel:
    active_animation: Union[Animation, None] = None
    
    def __init__(self, t: Callable[[], int], dest: pygame.Surface, size: int, index: int, coords: tuple[int, int], shuffle: Union[bool, list[int]] = True, manager: Union[StatusManager, None] = None):
        self.t = t
        self.dest = dest
        self.surface = pygame.Surface((size, size))
        
        shapes = [shape.render_shape(s, i//2, (i+1)%2, size, i*3 + s + 1) for i in range(3) for s in range(3)]
        blank = pygame.Surface((size, size))
        blank.set_colorkey((0,0,0))
        blank.fill((0,0,0))
        shapes.insert(0, blank)
        self.shuffle_map = [i for i in range(len(shapes))]
        if shuffle == False:
            pass
        elif shuffle == True:
            random.shuffle(self.shuffle_map)
        elif len(shuffle) == len(shapes):
            self.shuffle_map = shuffle
        
        self.shapes = [shapes[self.shuffle_map.index(i)] for i in range(len(shapes))]
        self.pos = self.map_pos(0)
        self.index = index
        self.coords = coords
        
        self.manager = manager
    
    # i is the standard/pre-shuffle index of the shape
    def spin_to(self, i: int):
        i = i % len(self.shapes)
        to_pos = self.map_pos(i)
        
        # go directly to the blank
        if i == 0:
            self.active_animation = Animation(self.t, self.t(), self.pos_distance(int(self.pos), to_pos) * 200 + 1, self.pos, to_pos)
        
        # spin to shape
        elif i < len(self.shapes):
            self.active_animation = Animation(self.t, self.t(), 6000 + self.index * 250, self.pos, to_pos + len(self.shapes) * 4)

        else:
            return
        
        if self.manager != None:
            self.manager.update_status(self.index, True)
        
    def map_pos(self, pos: int):
        return self.shuffle_map[pos]
        
    def pos_distance(self, from_pos: int, to_pos: int):
        return (to_pos - from_pos) % len(self.shapes)
        
    def render(self):
        if self.active_animation != None:
            self.pos = self.active_animation.get_value() % len(self.shapes)
            if self.active_animation.is_done():
                self.active_animation = None
                if self.manager != None:
                    self.manager.update_status(self.index, False)

        bottom = math.floor(self.pos)
        top = (bottom+1) % len(self.shapes)
        height = self.surface.get_height()
        offset = (self.pos - bottom) * height
        width = self.surface.get_width()
        self.surface.fill((255,255,255))
        self.surface.blit(self.shapes[top], (0,0), (0, height - offset, width, height))
        self.surface.blit(self.shapes[bottom], (0, offset), (0, 0, width, height - offset))
        self.dest.blit(self.surface, self.coords)