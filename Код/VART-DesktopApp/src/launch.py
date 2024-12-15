"""
Запустить приложение для плоттера
"""
from dearpygui import dearpygui as dpg

from app.application import App

if __name__ == '__main__':
    dpg.create_context()
    dpg.create_viewport(title="Cable Plotter App", width=1280, height=720)
    App().build()
    dpg.setup_dearpygui()
    dpg.show_viewport()
    dpg.start_dearpygui()
    dpg.destroy_context()
