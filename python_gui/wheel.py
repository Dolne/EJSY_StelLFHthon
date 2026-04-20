import math
from pathlib import Path
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
        
    def status(self):
        return any(self.slot_status)
    
    def update_status(self, index: int, status: bool):
        prev = self.status()
        if index > 0 and index <= len(self.slot_status):
            # index starts from 1 so we -1 to make it 0-indexed
            self.slot_status[index-1] = status
        curr = self.status()
        if curr != prev:
            self.client.publish('display/status', int(curr))

class Wheel:
    active_animation: Union[Animation, None] = None
    
    def __init__(self, t: Callable[[], int], dest: pygame.Surface, size: int, index: int, coords: tuple[int, int], shuffle: Union[bool, list[int]] = True, manager: Union[StatusManager, None] = None):
        self.t = t
        self.dest = dest
        self.surface = pygame.Surface((size, size))
        self.surface.set_colorkey((0,0,0))
        self.surface.fill((0,0,0))
        
        self.border = pygame.Surface((size, size), pygame.SRCALPHA)
        pygame.draw.rect(self.border, (0, 0, 0, 12), (0, 0, size, size), size // 25, size // 10)
        
        blank = pygame.Surface((size, size))
        blank.set_colorkey((0,0,0))
        blank.fill((0,0,0))
        
        image = pygame.image.load(Path(__file__).parent / 'img' / f'{index}.png').convert()
        num = pygame.transform.scale(image, (size, size))
        num.set_colorkey((0,0,0))
        
        shapes = [blank] + [shape.render_shape(a, b, c, size, a * 4 + b * 2 + c + 1) for a in range(2) for b in range(2) for c in range(2)] + [num]
        
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
        
        # go directly to the blank or number
        if i == 0 or i == len(self.shapes) - 1:
            dist = self.pos_distance(int(self.pos), to_pos)
            # animate to self.pos + dist instead of to_pos to ensure wheel always spins the same way
            self.active_animation = Animation(self.t, self.t(), dist * 200 + 1, self.pos, self.pos + dist)
        
        # spin to shape
        elif i < len(self.shapes) - 1:
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
                    
        width = self.surface.get_width()
        height = self.surface.get_height()
        pygame.draw.rect(self.surface, (234, 234, 234), (0, 0, width, height), 0, width // 10)

        bottom = math.floor(self.pos)
        top = (bottom+1) % len(self.shapes)
        offset = (self.pos - bottom) * height
        if offset > 0:
            self.surface.blit(self.shapes[top], (0,0), (0, height - offset, width, height))
        self.surface.blit(self.shapes[bottom], (0, offset), (0, 0, width, height - offset))
        
        self.surface.blit(self.border, (0,0))
        
        self.dest.blit(self.surface, self.coords)