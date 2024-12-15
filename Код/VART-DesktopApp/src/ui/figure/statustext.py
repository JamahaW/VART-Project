from __future__ import annotations

from ui.abc import ItemID
from ui.dpg.impl import Text


class FigureStatusText(Text):

    @staticmethod
    def __getString(position: tuple[float, float], size: tuple[float, float]) -> str:
        px, py = position
        sx, sy = size
        return f"Position: {px:.1f}x{py:.1f}, Size: {sx:.1f}x{sy:.1f}"

    def update(self, position: tuple[float, float], size: tuple[float, float]) -> None:
        self.setValue(self.__getString(position, size))

    def placeRaw(self, parent_id: ItemID) -> None:
        super().placeRaw(parent_id)
        self.update((0, 0), (0, 0))
