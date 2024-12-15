from __future__ import annotations

from typing import Final
from typing import Iterable

from ui.abc import ItemID
from ui.custom.widgets import Border
from ui.dpg.impl import CollapsingHeader
from ui.dpg.impl import InputInt
from ui.figure.abc import Figure
from ui.figure.canvas import Canvas


class WorkAreaFigure(Figure):
    __WORK_AREA_VERTICES: Final[tuple[Iterable[float], Iterable[float]]] = (
        (0.5, 0.5, -0.5, -0.5, 0.5),
        (0.5, -0.5, -0.5, 0.5, 0.5)
    )

    def __init__(self, label: str):
        super().__init__(self.__WORK_AREA_VERTICES, label)
        self.__border = Border(self.__onSizeChanged, step=50)

        self.__left_dead_zone_input = InputInt("Left", self.__onDeadZoneChanged, step=50)
        self.__right_dead_zone_input = InputInt("Right", self.__onDeadZoneChanged, step=50)
        self.__bottom_dead_zone_input = InputInt("Bottom", self.__onDeadZoneChanged, step=50)
        self.__top_dead_zone_input = InputInt("Top", self.__onDeadZoneChanged, step=50)
        self.__vertical_offset_input = InputInt("Tool Vertical Offset", self.__onDeadZoneChanged, step=10, value_range=(-100, 100))

    def getBottomDeadZone(self) -> int:
        return self.__bottom_dead_zone_input.getValue()

    def getTopDeadZone(self) -> int:
        return self.__top_dead_zone_input.getValue()

    def getLeftDeadZone(self) -> int:
        return self.__left_dead_zone_input.getValue()

    def getRightDeadZone(self) -> int:
        return self.__right_dead_zone_input.getValue()

    def getVerticalOffset(self) -> int:
        return self.__vertical_offset_input.getValue()

    def setDeadZone(self, left: int, right: int, top: int, bottom: int, vertical_offset: int) -> None:
        self.__left_dead_zone_input.setValue(left)
        self.__right_dead_zone_input.setValue(right)
        self.__top_dead_zone_input.setValue(top)
        self.__bottom_dead_zone_input.setValue(bottom)
        self.__vertical_offset_input.setValue(vertical_offset)

    def setSize(self, size: tuple[float, float]) -> None:
        super().setSize(size)
        self.__border.setValue(size)

    def attachIntoCanvas(self, canvas: Canvas) -> None:
        canvas.axis.add(self)
        canvas.add(self.__border)

    def getTransformedVertices(self) -> tuple[list[float], list[float]]:
        size_x, size_y = self.getSize()

        left_dead_zone = self.getLeftDeadZone()
        right_dead_zone = self.getRightDeadZone()
        top_dead_zone = self.getTopDeadZone()
        bottom_dead_zone = self.getBottomDeadZone()

        area_width = size_x - left_dead_zone - right_dead_zone
        area_height = size_y - top_dead_zone - bottom_dead_zone

        offset_x = (left_dead_zone - right_dead_zone) / 2
        offset_y = (bottom_dead_zone - top_dead_zone) / 2 + self.getVerticalOffset()

        return (
            [x * area_width + offset_x for x in self.source_vertices_x],
            [y * area_height + offset_y for y in self.source_vertices_y],
        )

    def placeRaw(self, parent_id: ItemID) -> None:
        super().placeRaw(parent_id)

        dead_zone_header = CollapsingHeader("DeadZone")
        self.add(dead_zone_header)
        (
            dead_zone_header
            .add(self.__left_dead_zone_input)
            .add(self.__right_dead_zone_input)
            .add(self.__top_dead_zone_input)
            .add(self.__bottom_dead_zone_input)
            .add(self.__vertical_offset_input)
        )

    def __onSizeChanged(self, new_size: tuple[float, float]) -> None:
        super().setSize(new_size)
        new_width, new_height = new_size
        half_width = int(new_width // 2)
        half_height = int(new_height // 2)

        self.__left_dead_zone_input.setMaxValue(half_width)
        self.__right_dead_zone_input.setMaxValue(half_width)
        self.__top_dead_zone_input.setMaxValue(half_height)
        self.__bottom_dead_zone_input.setMaxValue(half_height)

        self.update()

    def __onDeadZoneChanged(self, _) -> None:
        self.update()
