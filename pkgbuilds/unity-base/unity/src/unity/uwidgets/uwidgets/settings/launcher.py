import os
from gi.repository import Gio, GLib


class UnityLauncher:
    launcher_settings = Gio.Settings.new('com.canonical.Unity.Launcher')
    launcher_compiz_settings = Gio.Settings.new_with_path('org.compiz.unityshell', '/org/compiz/profiles/unity/plugins/unityshell/')

    def set_launcher_position(self, position: str):
        if position == 'Bottom' or position == 'bottom':
            self.launcher_settings['launcher-position'] = 'Bottom'
        elif position == 'Left' or position == 'left':
            self.launcher_settings['launcher-position'] = 'Left'

    def set_launcher_autohide(self, hidemode: bool):
        self.launcher_compiz_settings['launcher-hide-mode'] = hidemode

# XXX: Add more settings
