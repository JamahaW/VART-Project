"""Приложение для маркерного плоттера"""

from __future__ import annotations

import math
from pathlib import Path

from dearpygui import dearpygui as dpg

from ui.dpg.impl import Button
from ui.dpg.impl import FileDialog
from ui.dpg.impl import Menu
from ui.figure.canvas import Canvas
from ui.figure.impl.transformable import TransformableFigure
from ui.figure.impl.workarea import WorkAreaFigure


class FigureRegistry:
    """
    Реестр фигур, расположенных на холсте.
    Добавить, получить фигуры
    """

    def __init__(self, canvas: Canvas) -> None:
        self.__temp_items_count: int = 0
        self.canvas = canvas
        self.figures = list[TransformableFigure]()

    def add(self, figure: TransformableFigure) -> None:
        """
        Добавить фигуру на холст
        :param figure:
        """
        self.figures.append(figure)
        self.canvas.attachFigure(figure)

    def demoAdd(self) -> None:
        """
        Добавить демо-фигуру
        """
        r = range(0, 271, 1)
        vertices = (
            [math.cos(math.radians(i)) for i in r],
            [math.sin(math.radians(i)) for i in r]
        )

        circle = TransformableFigure(vertices, f"Figure: Test:{self.__temp_items_count}")
        self.__temp_items_count += 1
        self.add(circle)


class App:
    """
    Приложение
    """

    def __init__(self) -> None:
        self.file_dialog = FileDialog(
            "Select Image file", self.onImageFileSelected,
            (("png", "Image"),),
            r"A:\Program\Python3\CablePlotterApp\res\images"
        )

        self.work_area = WorkAreaFigure("Work Area")
        self.figure_registry = FigureRegistry(Canvas())

    @staticmethod
    def onImageFileSelected(paths: tuple[Path, ...]) -> None:
        """
        Callback при открытии файла
        :param paths:
        """
        print(paths)

    def build(self) -> None:
        """
        Построить UI приложения
        """
        with dpg.window() as main_window:
            dpg.set_primary_window(main_window, True)

            with dpg.menu_bar():
                Menu("File").place().add(Button("Open", self.file_dialog.show))

                (
                    Menu("dev").place()
                    .add(Button("app demo figure", self.figure_registry.demoAdd))
                )

                dpg.add_separator()

                (
                    Menu("Show").place()
                    .add(Button("show_implot_demo", dpg.show_implot_demo))
                    .add(Button("show_font_manager", dpg.show_font_manager))
                    .add(Button("show_style_editor", dpg.show_style_editor))
                    .add(Button("show_imgui_demo", dpg.show_imgui_demo))
                    .add(Button("show_item_registry", dpg.show_item_registry))
                    .add(Button("show_metrics", dpg.show_metrics))
                    .add(Button("show_debug", dpg.show_debug))
                )

            with dpg.tab_bar() as tabs:
                with dpg.tab(label="Canvas"):
                    self.figure_registry.canvas.place()

                with dpg.tab(label="Foo"):
                    Button("bar", lambda: print(dpg.get_value(tabs))).place()

        self.figure_registry.canvas.attachFigure(self.work_area)

        self.work_area.setDeadZone(150, 150, 100, 300, -120)
        self.work_area.setSize((1200, 1000))

        with dpg.theme() as global_theme:
            with dpg.theme_component(dpg.mvAll):
                dpg.add_theme_style(dpg.mvStyleVar_FrameRounding, 6)
                dpg.add_theme_style(dpg.mvStyleVar_WindowRounding, 6)
                dpg.add_theme_style(dpg.mvStyleVar_GrabRounding, 6)
                dpg.add_theme_style(dpg.mvStyleVar_ScrollbarRounding, 6)
                dpg.add_theme_style(dpg.mvStyleVar_PopupRounding, 6)

        dpg.bind_theme(global_theme)
