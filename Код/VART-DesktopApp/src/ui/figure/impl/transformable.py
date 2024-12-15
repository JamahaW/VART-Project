from __future__ import annotations

import math
from typing import Iterable

from ui.abc import ItemID
from ui.dpg.impl import Button
from ui.dpg.impl import Checkbox
from ui.dpg.impl import DragPoint
from ui.dpg.impl import InputInt
from ui.figure.abc import Figure
from ui.figure.canvas import Canvas
from ui.figure.statustext import FigureStatusText


class TransformableFigure(Figure):

    def __init__(self, vertices: tuple[Iterable[float], Iterable[float]], label: str) -> None:
        super().__init__(vertices, label, (100, 100))
        self.__source_vertices_x, self.__source_vertices_y = vertices

        self.__sin_angle: float = 0
        self.__cos_angle: float = 0
        self.setRotation(0)

        self.__position_point = DragPoint(self.__onPositionChanged, label="Position")
        self.__size_point = DragPoint(self.__onSizeChanged, label="Size", default_value=self.getSize())
        self.__set_controls_visible_checkbox = Checkbox(self.__onSetControlsVisibleChanged, label="Controls Visibility", default_value=True)
        self.__status_text = FigureStatusText()

    def setRotation(self, angle: int) -> None:
        angle = math.radians(angle)
        self.__sin_angle = math.sin(angle)
        self.__cos_angle = math.cos(angle)

    def getPosition(self) -> tuple[float, float]:
        return self.__position_point.getValue()

    def setPosition(self, position: tuple[float, float]) -> None:
        self.__position_point.setValue(position)

    def setSize(self, size: tuple[float, float]) -> None:
        super().setSize(size)
        size_x, size_y = size
        position_x, position_y = self.getPosition()
        self.__size_point.setValue((position_x + size_x, position_y + size_y))

    def getTransformedVertices(self) -> tuple[list[float], list[float]]:
        transformed_x = list[float]()
        transformed_y = list[float]()

        size_x, size_y = self.getSize()
        position_x, position_y = self.getPosition()

        sin_angle = self.__sin_angle
        cos_angle = self.__cos_angle

        for x, y in zip(self.__source_vertices_x, self.__source_vertices_y):
            x *= size_x
            y *= size_y

            transformed_x.append(cos_angle * x - sin_angle * y + position_x)
            transformed_y.append(sin_angle * x + cos_angle * y + position_y)

        return transformed_x, transformed_y

    def attachIntoCanvas(self, canvas: Canvas) -> None:
        canvas.axis.add(self)
        canvas.add(self.__position_point)
        canvas.add(self.__size_point)
        self.update()

    def delete(self) -> None:
        super().delete()
        self.__position_point.delete()
        self.__size_point.delete()

    def placeRaw(self, parent_id: ItemID) -> None:
        super().placeRaw(parent_id)
        self.add(self.__status_text)
        self.add(InputInt("Rotation", self.__onRotationChanged, value_range=(0, 360), step=15, default_value=0))
        self.add(self.__set_controls_visible_checkbox)
        self.add(Button("Remove", self.delete))

    def __onPositionChanged(self, new_position: tuple[float, float]) -> None:
        position_x, position_y = new_position
        scale_x, scale_y = self.getSize()
        self.__size_point.setValue((position_x + scale_x, position_y + scale_y))
        self.update()
        self.__status_text.update(new_position, self.getSize())

    def __onSizeChanged(self, new_scale: tuple[float, float]) -> None:
        scale_x, scale_y = new_scale
        position_x, position_y = position = self.getPosition()
        self.setSize((scale_x - position_x, scale_y - position_y))
        self.update()
        self.__status_text.update(position, new_scale)

    def __onRotationChanged(self, new_rotation) -> None:
        self.setRotation(new_rotation)
        self.update()

    def __onSetControlsVisibleChanged(self, is_visible: bool) -> None:
        self.__position_point.setVisible(is_visible)
        self.__size_point.setVisible(is_visible)
