# Arch Linux Unity7 Downstream Development Log

## Project Overview

Porting [`gentoo-unity7`](https://github.com/c4pp4/gentoo-unity7) — a complete Gentoo overlay providing the Unity7 desktop environment — to Arch Linux.

**Goal**: Create local PKGBUILDs for all Unity7 components, get the desktop environment building and running on Arch Linux, then eventually publish to the AUR.

**Source overlay**: This repo is a fresh fork of `gentoo-unity7`.
**Upstream Unity7**: Ubuntu's Unity7 is maintained at `https://launchpad.net/ubuntu/+source/unity`
**Host**: Everyday-use Arch Linux (x86_64), snapshots available for recovery.

---

## Overlay Structure Summary

The overlay provides **97 packages** across 17 categories:

| Category | Count | Notable packages |
|---|---|---|
| `unity-base/` | 22 | unity, compiz, nux, bamf, hud, unity-api, etc. |
| `unity-indicators/` | 13 | indicator-sound, indicator-session, indicator-appmenu, etc. |
| `dev-libs/` | 13 | dee, libunity, libindicator, libappindicator, ido, etc. |
| `unity-lens/` | 8 | lens-applications, lens-files, lens-music, etc. |
| `unity-extra/` | 9 | unity-greeter, unity-tweak-tool, diodon, etc. |
| `x11-themes/` | 10 | yaru-theme, ubuntu-themes, humanity-icon-theme, etc. |
| `gnome-extra/` | 4 | zeitgeist, activity-log-manager, etc. |
| Other | 18 | fonts, accessibility, Java support, etc. |

Custom infrastructure:
- `eclass/ubuntu-versionator.eclass` — handles Launchpad source URLs and Ubuntu patch series
- `ehooks/` — patches 40+ Gentoo tree packages at build time (global menu, session files, etc.)

---

## Arch-Specific Design Decisions

### 1. No USE flags → hardcoded options
Gentoo USE flags are replaced with hardcoded sensible defaults in all PKGBUILDs:
- `gles2=no` → use standard OpenGL/GLEW
- `debug=no` → release builds only
- `doc=no` → skip documentation generation
- `test=no` → skip test suites (speeds up builds, avoids D-Bus/X11 test infrastructure requirements)
- `systemd=yes` → always enabled (Arch default)
- `introspection=yes` → always enabled (needed for Python/Vala bindings)

### 2. Ubuntu versioning in PKGBUILDs
Gentoo ebuilds use `UVER`/`UREV` variables to encode Ubuntu's versioning suffix (e.g., `+25.10.20250930-0ubuntu3`). In PKGBUILDs:
- `pkgver` = upstream version (e.g., `0.5.6`)
- `_uver` = Ubuntu version string (e.g., `+22.04.20220217-0ubuntu6`)
- Launchpad orig tarball URL: `https://launchpad.net/ubuntu/+archive/primary/+files/${pkgname}_${pkgver}${_uver}.orig.tar.gz`
- Debian patch tarball URL: same with `.debian.tar.xz` suffix

### 3. Debian patch series application
The eclass automatically applies Ubuntu's Debian patch series. Equivalent `prepare()` function in PKGBUILDs:
```bash
prepare() {
  cd "$pkgname-$pkgver"
  # Apply upstream patches first
  # Then apply debian patches from series file
  if [ -f ../debian/patches/series ]; then
    while IFS= read -r patch || [ -n "$patch" ]; do
      [[ "$patch" =~ ^#.*$ || -z "$patch" ]] && continue
      patch -p1 -i "../debian/patches/$patch"
    done < ../debian/patches/series
  fi
}
```

### 4. glewmx challenge
GLEW 2.0 removed MX (multi-context) support. `nux` requires `glewmx`. Solution: Package GLEW 1.13.0 with `GLEW_MX=1` build flag, installed as `libGLEWmx.so` alongside regular GLEW. Source: SourceForge GLEW 1.13.0 release.

### 5. Ehooks equivalents for session integration
The Gentoo ehooks system patches tree packages. For Arch, initial approach:
- **gnome-session**: Install Unity `.session` file directly (no patching of gnome-session PKGBUILD needed)
- **gtk3 global menu**: Deferred — requires maintaining a patched `gtk3` (marked TODO)
- **lightdm**: Install greeter config file manually
- **Other ehooks** (Firefox/Thunderbird menus, etc.): Optional; document as not yet ported

### 6. Vala version
Overlay requires Vala 0.56 API. Arch has `vala 0.56.19-1` — exact match. ✓

### 7. libdbusmenu
`libdbusmenu-glib` and `libdbusmenu-gtk3` are in Arch official `extra/` repo — no PKGBUILD needed. ✓

---

## Dependency Map

### Available in Arch official repos (no PKGBUILD needed)
| Package | Arch name | Version |
|---|---|---|
| glib2 | `glib2` | 2.86.5 ✓ |
| dbus | `dbus` | 1.16.2 ✓ |
| dbus-glib | `dbus-glib` | 0.114 ✓ |
| gtk3 | `gtk3` | 3.24.52 ✓ |
| libdbusmenu-glib | `libdbusmenu-glib` | 18.10 ✓ |
| libdbusmenu-gtk3 | `libdbusmenu-gtk3` | 18.10 ✓ |
| libsigc++ | `libsigc++` | 2.12.1 ✓ |
| libwnck3 | `libwnck3` | 43.3 ✓ |
| libgtop | `libgtop` | 2.41.3 ✓ |
| boost | `boost` | 1.90.0 ✓ |
| boost-libs | `boost-libs` | 1.90.0 ✓ |
| pcre2 | `pcre2` | 10.47 ✓ |
| glew (standard) | `glew` | 2.3.1 ✓ |
| mesa | `mesa` | 26.0.4 ✓ |
| libglvnd | `libglvnd` | 1.7.0 ✓ |
| startup-notification | `startup-notification` | 0.12 ✓ |
| xapian-core | `xapian-core` | 1.4.30 ✓ |
| gobject-introspection | `gobject-introspection` | 1.86.0 ✓ |
| vala | `vala` | 0.56.19 ✓ |
| intltool | `intltool` | 0.51.0 ✓ |
| cmake | `cmake` | 4.3.1 ✓ |
| meson | `meson` | 1.10.2 ✓ |
| gnome-session | `gnome-session` | 49.2 ✓ |
| xorg-server-xvfb | `xorg-server-xvfb` | 21.1.21 ✓ |
| lightdm | needs install | — |
| pam | `linux-pam` | available ✓ |
| json-glib | `json-glib` | available ✓ |
| libpng | `libpng` | available ✓ |
| protobuf | `protobuf` | available ✓ |
| libxslt | `libxslt` | available ✓ |
| pango | `pango` | available ✓ |
| cairo | `cairo` | available ✓ |

### Available in AUR (use or reference as starting point)
| Package | AUR name | Notes |
|---|---|---|
| cmake-extras | `cmake-extras-git` | Use as build dep |
| compiz 0.9.14.2 | `compiz` | May differ in patches; write own |
| libunity 7.2.6 | `libunity` | Wrong version; write own at 7.1.4 |
| grail 3.1.1 | `grail` | Can use AUR or write own |
| libindicator (gtk2) | `libindicator-gtk2` | Need gtk3 version instead |
| libappindicator (gtk2) | `libappindicator-gtk2` | Need gtk3 version instead |

### Need PKGBUILDs (build order)
| Tier | Package | Version | Build system | Notes |
|---|---|---|---|---|
| 0 | cmake-extras | 1.9 | cmake | Build dep for several packages |
| 1 | grail | 3.1.1 | autotools | Gesture lib |
| 1 | geis | 2.2.17 | autotools | Gesture synthesis, depends on grail |
| 2 | dee | 1.2.7 | autotools | Core model/IPC lib |
| 2 | libzeitgeist | 0.3.18 | autotools | Activity logging lib |
| 2 | ido | 13.10.0 | autotools | Indicator display objects |
| 3 | libindicator | 16.10.0 | autotools | gtk3 slot only |
| 3 | libappindicator | 12.10.1 | autotools | gtk3 slot only |
| 3 | libcolumbus | 1.1.0 | cmake | Spell correction for libunity |
| 4 | libunity-misc | 4.0.5 | autotools | Unity utility lib |
| 4 | libunity | 7.1.4 | autotools | Core Unity API lib |
| 5 | glewmx | 1.13.0 | make | Old GLEW with MX support |
| 6 | nux | 4.0.8 | autotools | Visual rendering toolkit |
| 6 | bamf | 0.5.6 | autotools | App matching framework |
| 6 | gsettings-ubuntu-touch-schemas | 0.0.7 | cmake | GSettings schemas |
| 6 | unity-settings-daemon | 15.04.1 | autotools | Settings daemon |
| 6 | unity-api | 8.7 | cmake | Unity API headers |
| 6 | unity-gtk-module | 0.0.0 | autotools | GTK module |
| 6 | compiz | 0.9.14.2 | cmake | Window manager (Unity-patched) |
| 6 | hud | 14.10 | cmake | Head-up display |
| 7 | unity | 7.7.0 | cmake | Main Unity shell |
| 8 | unity-settings | 7 | metadata | GSettings defaults |
| 8 | unity-greeter | 25.04.1 | meson | LightDM greeter |
| 9 | indicator-application | 12.10.1 | autotools | App indicator service |
| 9 | indicator-appmenu | 15.02.0 | autotools | App menu |
| 9 | indicator-bluetooth | 0.0.6 | cmake | Bluetooth |
| 9 | indicator-datetime | 15.10 | cmake | Date/time |
| 9 | indicator-keyboard | 0.0.0 | cmake | Keyboard layout |
| 9 | indicator-messages | 13.10.1 | autotools | Messaging menu |
| 9 | indicator-power | 12.10.6 | cmake | Battery/power |
| 9 | indicator-printers | 0.1.7 | autotools | Printers |
| 9 | indicator-session | 17.3.20 | cmake | Session management |
| 9 | indicator-sound | 12.10.2 | cmake | Volume/sound |
| 10 | unity-lens-applications | 7.1.0 | cmake | App search |
| 10 | unity-lens-files | 7.1.0 | cmake | File search |
| 10 | unity-lens-music | 6.9.1 | cmake | Music search |

---

## Build Log

### Session 1 — 2026-04-05

**Status**: Initial setup. Devlog created, PKGBUILDs being written.

**Environment snapshot**:
- Arch Linux kernel 6.19.11-arch1-1
- Vala 0.56.19 ✓
- GLib 2.86.5
- GTK3 3.24.52
- GLEW 2.3.1 (standard; glewmx PKGBUILD needed)

**Actions**:
- [x] Created `downstreamDevlog.md`
- [x] Explored full overlay structure (97 packages, 17 categories)
- [x] Mapped all dependencies to Arch equivalents
- [x] Determined build order (10 tiers)
- [x] Created `pkgbuilds/` directory with PKGBUILDs
- [x] Installed Tier 0 build dependencies
- [x] **Built and installed Tier 0 deps**: `frame` 2.5.0 ✓
- [x] **Built and installed Tier 1**: `grail` 3.1.1 ✓, `geis` 2.2.17 ✓
- [x] **Built and installed Tier 2**: `dee` 1.2.7 ✓, `ido` 13.10.0 ✓, `libzeitgeist` 0.3.18 ✓
- [x] **Built and installed Tier 3**: `libcolumbus` 1.1.0 ✓, `libindicator` (from repos) ✓, `libappindicator` (from repos) ✓
- [x] **Built and installed Tier 4**: `libunity-misc` 4.0.5 ✓, `libunity` 7.1.4 ✓
- [x] **Built and installed Tier 5**: `glewmx` 1.13.0 ✓
- [x] **Built and installed Tier 6 (partial)**: `bamf` 0.5.6 ✓, `nux` 4.0.8 ✓
- [ ] Building remaining Tier 6: `gsettings-ubuntu-touch-schemas`, `unity-settings-daemon`, `compiz`, `hud`, `unity-api`, `unity-gtk-module`
- [ ] Building Tier 7: `unity`

---

## Lessons Learned / Build Notes

### Source Tarball Structure Patterns
Several packages use `S="${WORKDIR}"` (Gentoo) meaning the orig.tar.gz extracts **flat** into $srcdir:
- `frame`, `dee`, `ido`, `bamf`, `libunity`, `nux` — all flat extraction
- `grail`, `geis`, `libzeitgeist`, `libunity-misc`, `libcolumbus` — subdirectory extraction

Detection method: `tar -tzf orig.tar.gz | head -1` — if starts with `./`, it's flat.

### Quilt State in diff.gz Files
Some diff.gz files contain a **quilt state** (`.pc/` directory) showing patches already applied. If you try to re-apply `debian/patches/series` after the diff, patches will fail as "already applied". Fix: check for `.pc/applied-patches` before applying debian series.

### GLEW MX Headers
GLEW 2.0+ removed `GLEWContext` and other MX-specific types. The `glewmx` package installs GLEW 1.13 headers to `/usr/include/glewmx/` (separate from system `/usr/include/GL/`) and sets correct `-DGLEW_MX -I/usr/include/glewmx` via pkg-config. This avoids conflicts with the system `glew` package.

### ubuntu-private.h (GTK3 Ubuntu extension)
`ido` depends on `<gtk/ubuntu-private.h>` for `idomenuitemfactory.c` — a file that Ubuntu adds to their patched gtk3 to support custom menu item factories. This file **does not exist** in standard Arch gtk3.

**Workaround applied**: `idomenuitemfactory.c` removed from ido's build (breaks custom indicator menu items). `libido.c`'s call to `ido_menu_item_factory_get_type()` also patched out.

**Proper fix** (TODO): Create a patched `gtk3-unity` package applying `ubuntu_gtk_custom_menu_items.patch` from Ubuntu's `gtk+3.0_3.24.50-1ubuntu2.debian.tar.xz`. This patch adds `gtk/ubuntu-private.h`, `gtk/ubuntumenuitemfactory.h`, `gtk/ubuntumenuitemfactory.c` and integrates with `gtkmenushell.c`.

### Old autotools / configure.ac Issues
- `GNOME_COMMON_INIT` macro fails with modern `gnome-common` → remove it from `configure.ac` (bamf)
- `gtk-doc.make` conflicts with `--disable-gtk-doc` → remove the `doc` subdirectory from `Makefile.am` (libzeitgeist)
- Old CMake `cmake_minimum_required` versions → add `-DCMAKE_POLICY_VERSION_MINIMUM=3.5` (libcolumbus)
- `AM_CXXFLAGS-` typo in nux diff → benign (configure still runs), but noted

### Launchpad Download Reliability
Launchpad frequently resets TLS connections. `curl --retry 5 --retry-delay 5` usually works within 3 attempts. Parallel downloads increase failure rate — use sequential downloads for reliability.

---

## Known Issues / Blockers

### glewmx
- GLEW 2.0 removed multi-context (MX) support
- `nux` links against `libGLEWmx.so`
- Fix: build GLEW 1.13.0 with `GLEW_MX=1`; install as `glewmx` package alongside system GLEW

### gtk3 global menu (ehooks not yet ported)
- The Gentoo overlay patches `gtk3` via ehooks to enable global menu (AppMenu) support
- This is required for Unity's app menu bar in the panel
- Not yet ported — Unity will work without it but apps won't show their menus in the panel
- Long-term fix: maintain a patched `gtk3-unity` package (ABS/custom PKGBUILD)
- Tracking: TODO in Phase 2

### Boost Python
- nux and compiz require Boost with Python bindings
- Arch `boost` includes Python support — verify at build time

### libcolumbus
- Ubuntu-specific spell correction library used only by libunity
- No Arch/AUR package exists
- Must build from Launchpad source

### properties-cpp / xpathselect
- Ubuntu-specific utility headers
- Small, no build system complexity — straightforward PKGBUILDs

---

## Lessons Learned

*(updated as builds proceed)*

---

## References

- Upstream Unity7: https://launchpad.net/ubuntu/+source/unity
- Gentoo overlay: https://github.com/c4pp4/gentoo-unity7
- Ubuntu Launchpad package search: https://launchpad.net/ubuntu/+archive/primary/+files/
- Arch AUR: https://aur.archlinux.org/
- gentoo-unity7 build docs: `docs/build_instructions.md`
- gentoo-unity7 integration notes: `docs/notes.txt`
