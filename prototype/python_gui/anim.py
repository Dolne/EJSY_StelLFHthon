from typing import Callable


def ease_in_out_quad(x: float):
    return 2 * x**2 if x < 0.5 else 1 - (-2 * x + 2) ** 2 / 2
    
class Animation:
    def __init__(self, t: Callable[[], int], start_time: int, duration: int, start_value: float, end_value: float, ease: Callable[[float], float] = ease_in_out_quad):
        self.t = t
        self.start_time = start_time
        self.duration = duration
        self.start_value = start_value
        self.end_value = end_value
        self.ease = ease
        
    def get_progress(self) -> float:
        return min(max(0, (self.t() - self.start_time) / self.duration), 1)
    
    def get_value(self) -> float:
        return self.start_value + self.ease(self.get_progress()) * (self.end_value - self.start_value)
    
    def is_done(self) -> bool:
        return self.t() >= self.start_time + self.duration