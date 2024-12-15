from __future__ import annotations

from abc import ABC
from abc import abstractmethod
from typing import Final
from typing import Iterable

from ui.dpg.impl import LineSeries


class Figure(LineSeries, ABC):
    from ui.figure.canvas import Canvas

    def __init__(self, vertices: tuple[Iterable[float], Iterable[float]], label: str, size: tuple[int, int] = (0, 0)) -> None:
        super().__init__(label)
        source_x, source_y = vertices
        self.source_vertices_x: Final[Iterable[float]] = source_x
        self.source_vertices_y: Final[Iterable[float]] = source_y
        self.__size = size

    @abstractmethod
    def getTransformedVertices(self) -> tuple[list[float], list[float]]:
        pass

    @abstractmethod
    def attachIntoCanvas(self, canvas: Canvas) -> None:
        pass

    def getSize(self) -> tuple[float, float]:
        return self.__size

    def setSize(self, size: tuple[float, float]) -> None:
        self.__size = size

    def update(self) -> None:
        self.setValue(self.getTransformedVertices())
