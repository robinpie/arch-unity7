# Install script for directory: /run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "None")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "0")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/build/a11y/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/build/unity-shared/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/build/dash/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/build/launcher/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/build/data/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/build/hud/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/build/lockscreen/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/build/panel/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/build/decorations/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/build/plugins/unityshell/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/build/plugins/unity-mt-grab-handles/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/build/shortcuts/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/build/shutdown/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/build/unity-standalone/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/build/doc/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/build/services/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/build/tools/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/build/UnityCore/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/build/guides/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/build/gnome/cmake_install.cmake")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/unity/themes" TYPE FILE FILES "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/dash-widgets.json")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/share/unity/icons/album_missing.png;/usr/share/unity/icons/album_missing_preview.png;/usr/share/unity/icons/arrow_right.png;/usr/share/unity/icons/bag.svg;/usr/share/unity/icons/category_gradient.png;/usr/share/unity/icons/category_gradient_no_refine.png;/usr/share/unity/icons/close_dash.svg;/usr/share/unity/icons/close_dash_disabled.svg;/usr/share/unity/icons/close_dash_prelight.svg;/usr/share/unity/icons/close_dash_pressed.svg;/usr/share/unity/icons/dash-widgets.json;/usr/share/unity/icons/dash_bottom_border_tile.png;/usr/share/unity/icons/dash_bottom_border_tile_mask.png;/usr/share/unity/icons/dash_bottom_left_corner.png;/usr/share/unity/icons/dash_bottom_left_corner_mask.png;/usr/share/unity/icons/dash_bottom_right_corner.png;/usr/share/unity/icons/dash_bottom_right_corner_mask.png;/usr/share/unity/icons/dash_group_expand.png;/usr/share/unity/icons/dash_group_unexpand.png;/usr/share/unity/icons/dash_left_tile.png;/usr/share/unity/icons/dash_noise.png;/usr/share/unity/icons/dash_right_border_tile.png;/usr/share/unity/icons/dash_right_border_tile_mask.png;/usr/share/unity/icons/dash_sheen.png;/usr/share/unity/icons/dash_top_edge.png;/usr/share/unity/icons/dash_top_right_corner.png;/usr/share/unity/icons/dash_top_right_corner_mask.png;/usr/share/unity/icons/dash_top_tile.png;/usr/share/unity/icons/dialog_border_corner.png;/usr/share/unity/icons/dialog_border_left.png;/usr/share/unity/icons/dialog_border_top.png;/usr/share/unity/icons/dialog_close.png;/usr/share/unity/icons/dialog_close_highlight.png;/usr/share/unity/icons/dialog_close_press.png;/usr/share/unity/icons/emblem_apps.svg;/usr/share/unity/icons/emblem_books.svg;/usr/share/unity/icons/emblem_clothes.svg;/usr/share/unity/icons/emblem_music.svg;/usr/share/unity/icons/emblem_others.svg;/usr/share/unity/icons/emblem_video.svg;/usr/share/unity/icons/empty.png;/usr/share/unity/icons/hibernate.png;/usr/share/unity/icons/hibernate_highlight.png;/usr/share/unity/icons/information_icon.svg;/usr/share/unity/icons/kylin_login_activate.svg;/usr/share/unity/icons/launcher_arrow_btt_19.svg;/usr/share/unity/icons/launcher_arrow_btt_37.svg;/usr/share/unity/icons/launcher_arrow_ltr_19.svg;/usr/share/unity/icons/launcher_arrow_ltr_37.svg;/usr/share/unity/icons/launcher_arrow_outline_btt_19.svg;/usr/share/unity/icons/launcher_arrow_outline_btt_37.svg;/usr/share/unity/icons/launcher_arrow_outline_ltr_19.svg;/usr/share/unity/icons/launcher_arrow_outline_ltr_37.svg;/usr/share/unity/icons/launcher_arrow_outline_rtl_19.svg;/usr/share/unity/icons/launcher_arrow_outline_rtl_37.svg;/usr/share/unity/icons/launcher_arrow_rtl_19.svg;/usr/share/unity/icons/launcher_arrow_rtl_37.svg;/usr/share/unity/icons/launcher_arrow_ttb_19.svg;/usr/share/unity/icons/launcher_arrow_ttb_37.svg;/usr/share/unity/icons/launcher_bfb.png;/usr/share/unity/icons/launcher_bfb.svg;/usr/share/unity/icons/launcher_icon_back_150.svg;/usr/share/unity/icons/launcher_icon_back_54.svg;/usr/share/unity/icons/launcher_icon_edge_150.svg;/usr/share/unity/icons/launcher_icon_edge_54.svg;/usr/share/unity/icons/launcher_icon_glow_200.svg;/usr/share/unity/icons/launcher_icon_glow_62.png;/usr/share/unity/icons/launcher_icon_glow_62.svg.save;/usr/share/unity/icons/launcher_icon_selected_back_150.svg;/usr/share/unity/icons/launcher_icon_selected_back_54.svg;/usr/share/unity/icons/launcher_icon_shadow_200.svg;/usr/share/unity/icons/launcher_icon_shadow_62.svg;/usr/share/unity/icons/launcher_icon_shine_150.svg;/usr/share/unity/icons/launcher_icon_shine_54.svg;/usr/share/unity/icons/launcher_pip_btt_19.svg;/usr/share/unity/icons/launcher_pip_btt_37.svg;/usr/share/unity/icons/launcher_pip_ltr_19.svg;/usr/share/unity/icons/launcher_pip_ltr_37.svg;/usr/share/unity/icons/launcher_pip_rtl_19.svg;/usr/share/unity/icons/launcher_pip_rtl_37.svg;/usr/share/unity/icons/launcher_pressure_effect.png;/usr/share/unity/icons/launcher_pressure_effect_rotated.png;/usr/share/unity/icons/lens-nav-app.svg;/usr/share/unity/icons/lens-nav-file.svg;/usr/share/unity/icons/lens-nav-gwibber.svg;/usr/share/unity/icons/lens-nav-home.svg;/usr/share/unity/icons/lens-nav-music.svg;/usr/share/unity/icons/lens-nav-people.svg;/usr/share/unity/icons/lens-nav-photo.svg;/usr/share/unity/icons/lens-nav-video.svg;/usr/share/unity/icons/lock_icon.png;/usr/share/unity/icons/lockscreen.png;/usr/share/unity/icons/lockscreen_cof.png;/usr/share/unity/icons/lockscreen_highlight.png;/usr/share/unity/icons/logout.png;/usr/share/unity/icons/logout_highlight.png;/usr/share/unity/icons/maximize_dash.svg;/usr/share/unity/icons/maximize_dash_disabled.svg;/usr/share/unity/icons/maximize_dash_prelight.svg;/usr/share/unity/icons/maximize_dash_pressed.svg;/usr/share/unity/icons/minimize_dash.svg;/usr/share/unity/icons/minimize_dash_disabled.svg;/usr/share/unity/icons/minimize_dash_prelight.svg;/usr/share/unity/icons/minimize_dash_pressed.svg;/usr/share/unity/icons/next.svg;/usr/share/unity/icons/overlay_top_left_tile.png;/usr/share/unity/icons/panel_shadow.png;/usr/share/unity/icons/pattern_overlay.png;/usr/share/unity/icons/pause.svg;/usr/share/unity/icons/places-tile-bg-tilable.png;/usr/share/unity/icons/play.svg;/usr/share/unity/icons/prev.svg;/usr/share/unity/icons/preview_next.svg;/usr/share/unity/icons/preview_pause.svg;/usr/share/unity/icons/preview_play.svg;/usr/share/unity/icons/preview_previous.svg;/usr/share/unity/icons/progress_bar_fill.svg;/usr/share/unity/icons/progress_bar_trough.svg;/usr/share/unity/icons/refine_gradient_corner.png;/usr/share/unity/icons/refine_gradient_dash.png;/usr/share/unity/icons/refine_gradient_panel.png;/usr/share/unity/icons/refine_gradient_panel_single_column.png;/usr/share/unity/icons/restart.png;/usr/share/unity/icons/restart_highlight.png;/usr/share/unity/icons/round_corner_54x54.png;/usr/share/unity/icons/round_glow_62x62.png;/usr/share/unity/icons/round_glow_hl_62x62.png;/usr/share/unity/icons/round_outline_54x54.png;/usr/share/unity/icons/round_shine_54x54.png;/usr/share/unity/icons/search_circle.svg;/usr/share/unity/icons/search_close.png;/usr/share/unity/icons/search_close.svg;/usr/share/unity/icons/search_magnify.svg;/usr/share/unity/icons/search_spin.png;/usr/share/unity/icons/search_spin.svg;/usr/share/unity/icons/searchingthedashlegalnotice.html;/usr/share/unity/icons/sheet_style_close_focused.svg;/usr/share/unity/icons/sheet_style_close_focused_prelight.svg;/usr/share/unity/icons/sheet_style_close_focused_pressed.svg;/usr/share/unity/icons/shutdown.png;/usr/share/unity/icons/shutdown_highlight.png;/usr/share/unity/icons/squircle_base_54.png;/usr/share/unity/icons/squircle_base_selected_54.png;/usr/share/unity/icons/squircle_edge_54.png;/usr/share/unity/icons/squircle_glow_62.png;/usr/share/unity/icons/squircle_shadow_62.png;/usr/share/unity/icons/squircle_shine_54.png;/usr/share/unity/icons/star-outline.svg;/usr/share/unity/icons/star_deselected.png;/usr/share/unity/icons/star_highlight.png;/usr/share/unity/icons/star_selected.png;/usr/share/unity/icons/suspend.png;/usr/share/unity/icons/suspend_highlight.png;/usr/share/unity/icons/switch_user.svg;/usr/share/unity/icons/switcher_background.png;/usr/share/unity/icons/unmaximize_dash.svg;/usr/share/unity/icons/unmaximize_dash_disabled.svg;/usr/share/unity/icons/unmaximize_dash_prelight.svg;/usr/share/unity/icons/unmaximize_dash_pressed.svg;/usr/share/unity/icons/video_missing.png;/usr/share/unity/icons/warning_icon.png")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/usr/share/unity/icons" TYPE FILE FILES
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/album_missing.png"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/album_missing_preview.png"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/arrow_right.png"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/bag.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/category_gradient.png"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/category_gradient_no_refine.png"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/close_dash.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/close_dash_disabled.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/close_dash_prelight.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/close_dash_pressed.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/dash-widgets.json"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/dash_bottom_border_tile.png"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/dash_bottom_border_tile_mask.png"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/dash_bottom_left_corner.png"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/dash_bottom_left_corner_mask.png"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/dash_bottom_right_corner.png"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/dash_bottom_right_corner_mask.png"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/dash_group_expand.png"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/dash_group_unexpand.png"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/dash_left_tile.png"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/dash_noise.png"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/dash_right_border_tile.png"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/dash_right_border_tile_mask.png"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/dash_sheen.png"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/dash_top_edge.png"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/dash_top_right_corner.png"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/dash_top_right_corner_mask.png"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/dash_top_tile.png"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/dialog_border_corner.png"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/dialog_border_left.png"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/dialog_border_top.png"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/dialog_close.png"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/dialog_close_highlight.png"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/dialog_close_press.png"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/emblem_apps.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/emblem_books.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/emblem_clothes.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/emblem_music.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/emblem_others.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/emblem_video.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/empty.png"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/hibernate.png"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/hibernate_highlight.png"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/information_icon.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/kylin_login_activate.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/launcher_arrow_btt_19.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/launcher_arrow_btt_37.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/launcher_arrow_ltr_19.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/launcher_arrow_ltr_37.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/launcher_arrow_outline_btt_19.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/launcher_arrow_outline_btt_37.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/launcher_arrow_outline_ltr_19.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/launcher_arrow_outline_ltr_37.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/launcher_arrow_outline_rtl_19.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/launcher_arrow_outline_rtl_37.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/launcher_arrow_rtl_19.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/launcher_arrow_rtl_37.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/launcher_arrow_ttb_19.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/launcher_arrow_ttb_37.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/launcher_bfb.png"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/launcher_bfb.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/launcher_icon_back_150.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/launcher_icon_back_54.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/launcher_icon_edge_150.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/launcher_icon_edge_54.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/launcher_icon_glow_200.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/launcher_icon_glow_62.png"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/launcher_icon_glow_62.svg.save"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/launcher_icon_selected_back_150.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/launcher_icon_selected_back_54.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/launcher_icon_shadow_200.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/launcher_icon_shadow_62.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/launcher_icon_shine_150.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/launcher_icon_shine_54.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/launcher_pip_btt_19.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/launcher_pip_btt_37.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/launcher_pip_ltr_19.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/launcher_pip_ltr_37.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/launcher_pip_rtl_19.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/launcher_pip_rtl_37.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/launcher_pressure_effect.png"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/launcher_pressure_effect_rotated.png"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/lens-nav-app.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/lens-nav-file.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/lens-nav-gwibber.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/lens-nav-home.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/lens-nav-music.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/lens-nav-people.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/lens-nav-photo.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/lens-nav-video.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/lock_icon.png"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/lockscreen.png"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/lockscreen_cof.png"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/lockscreen_highlight.png"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/logout.png"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/logout_highlight.png"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/maximize_dash.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/maximize_dash_disabled.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/maximize_dash_prelight.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/maximize_dash_pressed.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/minimize_dash.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/minimize_dash_disabled.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/minimize_dash_prelight.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/minimize_dash_pressed.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/next.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/overlay_top_left_tile.png"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/panel_shadow.png"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/pattern_overlay.png"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/pause.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/places-tile-bg-tilable.png"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/play.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/prev.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/preview_next.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/preview_pause.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/preview_play.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/preview_previous.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/progress_bar_fill.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/progress_bar_trough.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/refine_gradient_corner.png"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/refine_gradient_dash.png"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/refine_gradient_panel.png"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/refine_gradient_panel_single_column.png"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/restart.png"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/restart_highlight.png"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/round_corner_54x54.png"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/round_glow_62x62.png"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/round_glow_hl_62x62.png"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/round_outline_54x54.png"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/round_shine_54x54.png"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/search_circle.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/search_close.png"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/search_close.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/search_magnify.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/search_spin.png"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/search_spin.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/searchingthedashlegalnotice.html"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/sheet_style_close_focused.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/sheet_style_close_focused_prelight.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/sheet_style_close_focused_pressed.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/shutdown.png"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/shutdown_highlight.png"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/squircle_base_54.png"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/squircle_base_selected_54.png"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/squircle_edge_54.png"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/squircle_glow_62.png"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/squircle_shadow_62.png"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/squircle_shine_54.png"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/star-outline.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/star_deselected.png"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/star_highlight.png"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/star_selected.png"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/suspend.png"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/suspend_highlight.png"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/switch_user.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/switcher_background.png"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/unmaximize_dash.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/unmaximize_dash_disabled.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/unmaximize_dash_prelight.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/unmaximize_dash_pressed.svg"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/video_missing.png"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/resources/warning_icon.png"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/unity" TYPE FILE FILES "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/build/unity-version.xml")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/build/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
if(CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_COMPONENT MATCHES "^[a-zA-Z0-9_.+-]+$")
    set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
  else()
    string(MD5 CMAKE_INST_COMP_HASH "${CMAKE_INSTALL_COMPONENT}")
    set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INST_COMP_HASH}.txt")
    unset(CMAKE_INST_COMP_HASH)
  endif()
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/build/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
