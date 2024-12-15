from dearpygui import dearpygui as dpg

from ui.abc import ItemID
from ui.dpg.impl import Axis
from ui.dpg.impl import Plot


class Canvas(Plot):
    from ui.figure.abc import Figure

    def __init__(self) -> None:
        super().__init__()
        self.axis = Axis(dpg.mvXAxis)

    def placeRaw(self, parent_id: ItemID) -> None:
        super().placeRaw(parent_id)
        self.add(self.axis)

    def attachFigure(self, figure: Figure) -> None:
        figure.attachIntoCanvas(self)
