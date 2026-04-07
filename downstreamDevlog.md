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
- [x] **Built and installed Tier 6 (complete)**:
  - `gsettings-ubuntu-touch-schemas` 0.0.7 ✓
  - `unity-api` 8.7 ✓
  - `unity-gtk-module` 0.0.0 ✓
  - `unity-settings-daemon` 15.04.1 ✓
  - `hud` 14.10 ✓ (needed `dee-qt` PKGBUILD first)
  - `compiz` 0.9.14.2 ✓ (single-threaded build `-j1`, ~9 min)
- [x] **Built and installed extra deps**: `dee-qt` 3.3 ✓, `xpathselect` 1.4 ✓, `libindicator-gtk3` 16.10.0 ✓
- [x] **Built and installed Tier 7**: `unity` 7.7.1 ✓ (~10 min build)
- [x] **Built and installed Tier 8**: `unity-session` ✓, `session-shortcuts` ✓, `unity-greeter` 25.04.1 ✓
- [x] **Built and installed Tier 9**: All 8 indicators installed:
  - `indicator-sound` 12.10.2 ✓
  - `indicator-session` 17.3.20 ✓
  - `indicator-power` 12.10.6 ✓ (disable-powerman.diff hunk 2 failed but `|| true`)
  - `indicator-datetime` 15.10 ✓ (EDS disabled, no unity-language-pack needed)
  - `indicator-keyboard` 0.0.0 ✓ (stubbed Ubuntu-specific AccountsService props)
  - `indicator-messages` 13.10.1 ✓
  - `indicator-application` 12.10.1 ✓
  - `indicator-appmenu` 15.02.0 ✓
- [x] **Built and installed Tier 10**: All 8 lens/scope packages:
  - `unity-scope-home` 6.8.2 ✓ (data-only; Vala src not built due to removed test dir)
  - `unity-lens-applications` 7.1.0 ✓ (patched out db5.3 version check; Arch ships db 6.2)
  - `unity-lens-files` 7.1.0 ✓
  - `unity-lens-music` 6.9.1 ✓
  - `unity-lens-video` 0.3.15 ✓
  - `unity-lens-photos` 1.0 ✓ (Python/setuptools)
  - `unity-scope-extras` 7 ✓ (calculator, chromiumbookmarks, firefoxbookmarks)
  - `unity-lens-meta` 7 ✓ (metapackage)

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
Launchpad frequently resets TLS connections. `curl --retry 5 --retry-delay 5` usually works within 3 attempts. Parallel downloads increase failure rate — use sequential downloads for reliability. When makepkg fails to download, manually `wget` the tarball to the PKGBUILD directory and retry.

### unity-settings-daemon: Ubuntu-only accountsservice and test binaries
- `act_user_set_input_sources()` is a Ubuntu-specific extension to `accountsservice` — stub it out with a comment.
- `usd-test-*` binaries are installed to libexec but don't link against `libunity-settings-daemon.la` properly — remove from `Makefile.am` with sed before autoconf.
- Sed pattern must handle digits: use `[^ ]*` not `[a-z-]*` for matching binary names like `usd-test-a11y-keyboard`.

### glewmx 1.13 / modern glext.h conflict
GLEW 1.13's `glew.h` defines `PFNGL*SGIXPROC` typedefs from `GL_SGIX_fragment_lighting`. Modern xorgproto `glext.h` also defines them → conflicting declaration errors when both are included.

**Fix**: Install a wrapper `glext.h` at `/usr/include/glewmx/GL/glext.h` that:
1. Pre-defines `GL_SGIX_fragment_lighting 1` (the extension guard) when `__glew_h__` is defined (i.e., glew.h was already included)
2. Then `#include "/usr/include/GL/glext.h"` (absolute path to avoid recursion)

This allows GLsizeiptr, GLchar etc. from real glext.h to be available while suppressing the conflicting SGIX typedefs.

### libindicator: Arch has old version without indicator-ng.h
Arch's `libindicator` 12.10.1 (from extra) lacks `indicator-ng.h` which was added later. Unity's `panel-service.c` requires `IndicatorNg` from this header for loading GMenu-based indicators.

**Fix**: Build our own `libindicator-gtk3` 16.10.0 from Ubuntu Launchpad sources (`+18.04.20180321.1` UVER). This version includes `indicator-ng.h` and provides the same `indicator3-0.4` pkg-config name. The package conflicts with and replaces `libindicator` from Arch repos.

### indicator3-0.4 version check in unity services/CMakeLists.txt
Unity's `services/CMakeLists.txt` requires `indicator3-0.4>=12.10.2`. Our `libindicator-gtk3` 16.10.0 reports version `12.10.2` (from the pkg-config file). Relax with sed: `s/>=12.10.2/>=12.10.1/`.

### indicator_desktop_shortcuts_nick_exec_with_context
Arch's `libindicator` doesn't have `indicator_desktop_shortcuts_nick_exec_with_context()` (added in Ayatana fork). Unity's `launcher/ApplicationLauncherIcon.cpp` calls it with a launch context arg. **Fix**: sed to fall back to `indicator_desktop_shortcuts_nick_exec()` (drops the launch context — minor feature loss).

### dee-qt pkgconfig path
`dee-qt` installs `libdee-qt5.pc` to the path `${CMAKE_LIBRARY_ARCHITECTURE}/pkgconfig`. With empty `CMAKE_LIBRARY_ARCHITECTURE` it goes to `/usr/pkgconfig/` (non-standard). Fix: set `-DCMAKE_LIBRARY_ARCHITECTURE=lib` so the pc file lands at `/usr/lib/pkgconfig/`.

### gsettings-qt includedir
`gsettings-qt.pc` on Arch has `includedir=/usr/include/qt5/QGSettings` — but code does `#include <QGSettings/qgsettings.h>` which requires the parent dir. Fix in PKGBUILD: add `-DCMAKE_CXX_FLAGS="-I/usr/include/qt5"` to cmake.

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

### Indicator tarball extraction is flat (not subdirectoried)
Most indicator orig.tar.gz files extract flat into `$srcdir` (just like indicator-sound). Don't assume `cd ${pkgname}-${pkgver}${_uver}`. Always use `cd "$srcdir"` and `cmake -S "$srcdir"` for cmake builds, or `./configure` directly from `$srcdir` for autotools.

### Ubuntu-specific AccountsService properties in indicator-keyboard
`indicator-keyboard` uses `Act.User.input_sources` and `Act.User.xkeyboard_layouts` which are Ubuntu-patched extensions to accountsservice not present in Arch's standard package. Fixed by using Python regex in prepare() to remove the loops that read these properties and replace `var layouts = ...` with `var layouts = new string[0];` — the function then falls through to LightDM fallback which works fine.

### indicator-power disable-powerman.diff partially fails
The `disable-powerman.diff` patch has a hunk that can't apply cleanly against the upstream source. Use `|| true` — the essential part (removing the statistics action from GActionEntry) still applies; the other hunk is cosmetic.

### indicator-datetime doesn't actually need unity-language-pack
The ebuild lists it as DEPEND (build-time) for po compilation, but since we comment out `add_subdirectory(po)` in prepare(), it's never used. Remove from makedepends.

### indicator-keyboard Vala patch ordering matters
When applying multiple patches to a .vala file (optional-fcitx.patch modifies main.vala), don't use a traditional diff patch with absolute line numbers for subsequent patches — the line numbers will be wrong. Use Python regex substitution instead, which is position-independent.

### unity-lens-applications requires db 5.3, but Arch ships db 6.2
The C source has a `#error` compile-time assertion requiring exactly `DB_VERSION_MAJOR==5, DB_VERSION_MINOR==3`. Arch's `db` package is 6.2. The API is compatible for our use, so patch out the `#if DB_VERSION_MAJOR != 5` ... `#endif` block in `src/unity-ratings-db.c` in prepare(). The `db.h` includes work fine with 6.2.

### unity-scope-home: debian tarball adds test subdirectory reference
After applying the debian patch series, `Makefile.am` gets `SUBDIRS = src/unit data po` where `src/unit` doesn't exist. Must `sed -i 's:src/unit::g' Makefile.am` before autoreconf. Also the overlay patches have one hunk that fails (already applied to source) — use `|| true`.

### indicator-appmenu SRC_URI is a combined .tar.gz (not orig+diff)
The source extracts to `${pkgname}.git/` (including `.git` in the dirname). Set the build directory accordingly: `cd "${pkgname}.git"` in build/package(). The configure check for `dbusmenu-jsonloader` (test tools) fails but that's fine — tools are disabled by default.

---

## References

- Upstream Unity7: https://launchpad.net/ubuntu/+source/unity
- Gentoo overlay: https://github.com/c4pp4/gentoo-unity7
- Ubuntu Launchpad package search: https://launchpad.net/ubuntu/+archive/primary/+files/
- Arch AUR: https://aur.archlinux.org/
- gentoo-unity7 build docs: `docs/build_instructions.md`
- gentoo-unity7 integration notes: `docs/notes.txt`
