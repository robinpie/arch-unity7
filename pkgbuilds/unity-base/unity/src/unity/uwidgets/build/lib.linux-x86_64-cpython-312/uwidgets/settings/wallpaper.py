import os
from gi.repository import Gio, GLib

import requests


class UnityWallpaper:
    cache_dir = GLib.get_user_cache_dir()
    pictures_dir = GLib.get_user_special_dir(GLib.USER_DIRECTORY_PICTURES)
    splash_wallpaper_file_path = os.path.join(
        cache_dir,
        "org.unityd.wallpapers"
    )
    background_settings = Gio.Settings.new("org.gnome.desktop.background")

    def set_wallpaper_from_file_uri(self, file_uri: str):
        self.background_settings['picture-uri'] = f"file:///{file_uri}"

    def write_image_url_to_wallpaper_file(self, image_url: str):
        response = requests.get(image_url, stream=True)
        if response.ok:
            with open(self.splash_wallpaper_file_path, "wb") as wallpaper_path:
                wallpaper_path.write(response.raw.read())

    def set_wallpaper_from_url(self, image_url: str):
        self.write_image_url_to_wallpaper_file(image_url)
        self.set_wallpaper_from_file_uri(self.splash_wallpaper_file_path)
