import re
from typing import Callable, Union

import pygame
from pygame.color import Color

def render_lights_old(t: int, size: int, width: int, count: int, offset: float, bg: Union[tuple[int, int, int], None]):
    blur_radius = size // 2
    surface = pygame.Surface((width + blur_radius * 2, size + blur_radius * 2), pygame.SRCALPHA if bg == None else 0)
    if bg:
        surface.fill(bg)
    r = size // 2
    interval = int((width - size) / (count - 1)) if count > 1 else 0
    
    hues = [(i, int((t / 1200 - offset * i) % 1 * 360)) for i in range(count)]
    
    for i, h in hues:
        pygame.draw.circle(surface, pygame.color.Color.from_hsla(h, 100, 50, 100), (r + interval * i + blur_radius, r + blur_radius), r)
        
    surface = pygame.transform.box_blur(surface, blur_radius)
    
    for i, h in hues:
        pygame.draw.aacircle(surface, pygame.color.Color.from_hsla(h, 100, 85, 100), (r + interval * i + blur_radius, r + blur_radius), r // 2)
        
    return surface

class ColourStep:
    def __init__(self, p: int, h: int, s: int, l: int):
        self.p = p
        self.h = h
        self.s = s
        self.l = l
        
    def __repr__(self):
        return f"({self.p}% {self.h} {self.s} {self.l})"
    
    def moved(self, p: int):
        return ColourStep(p, self.h, self.s, self.l)

class LightOptions:
    colours: list[ColourStep] = [ColourStep(0, 0, 100, 50), ColourStep(100, 360, 100, 50)]
    period: int = 1200
    travel: float = 0.5
    segments: int = 16
    active = False
    
    def __init__(self, t: Callable[[], int]):
        self.start_time = t()
        self.t = t
    
    def set(self, topic: str, payload: bytes):
        if len(payload) == 0:
            return
        if topic == 'colours':
            self.set_colours(payload.decode())
        elif topic == 'period':
            self.period = int(payload)
        elif topic == 'travel':
            self.travel = float(payload)
        elif topic == 'segments':
            self.segments = int(payload)
        else:
            return
        self.start_time = self.t()
            
    def set_colours(self, payload: str):
        strings = payload.split(',')
        if len(strings) == 0:
            return
        ex = r"(?P<p>\d+)%\s+(?P<h>\d+)(?:\s+(?P<s>\d+)(?:\s+(?P<l>\d+))?)?"
        colours: list[ColourStep] = []
        for str in strings:
            match = re.match(ex, str.strip())
            if not match:
                return
            p = int(match.group('p'))
            if p < 0 or p > 100:
                return
            h = int(match.group('h'))
            s = int(match.group('s') or 100)
            l = int(match.group('l') or 50)
            if len(colours) == 0 or colours[-1].p <= p:
                colours.append(ColourStep(p, h, s, l))
        if len(colours) == 0:
            return
        if colours[0].p != 0:
            colours.insert(0, colours[0].moved(0))
        if colours[-1].p != 100:
            colours.append(colours[-1].moved(100))
        self.colours = colours
        
    def get_progress(self, segment: int):
        return ((self.t() - self.start_time) / self.period - segment / self.segments * self.travel) % 1
        
    def get_colour(self, segment: int) -> tuple[int,int,int]:
        percent = self.get_progress(segment) * 100
        start = self.colours[0]
        end = self.colours[-1]
        for point in self.colours:
            if point.p >= percent:
                end = point
                break
            start = point
        p = (percent - start.p) / (end.p - start.p) if end.p > start.p else 0
        h = int(p * (end.h - start.h) + start.h) % 360
        s = min(max(0, int(p * (end.s - start.s) + start.s)), 100)
        l = min(max(0, int(p * (end.l - start.l) + start.l)), 100)
        return (h, s, l)
    
    def is_static(self):
        f = self.colours[0]
        for c in self.colours[1:]:
            if c.h != f.h or c.s != f.s or c.l != f.l:
                return False
        return True

def render_lights(width: int, size: int, bg: tuple[int, int, int], opts: LightOptions):
    surface = pygame.Surface((width, size * 2), pygame.SRCALPHA)
    surface.fill(bg)
    
    for i in range(opts.segments):
        colour = opts.get_colour(i)
        pygame.draw.rect(surface, Color.from_hsla(*colour, 100), (i / opts.segments * width, 0, (i + 1) / opts.segments * width, size))
    
    surface = pygame.transform.box_blur(surface, size)
    
    return surface.subsurface((0, size, width, size))
    