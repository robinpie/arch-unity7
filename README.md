# Arch Unity⁷

A port of the Unity7 desktop environment to Arch Linux, packaged as a collection of PKGBUILDs.

Forked from [gentoo-unity7] by c4pp4, which itself descends from [unity-gentoo] by shiznix. The original project is a Gentoo overlay; this is a re-packaging effort targeting Arch Linux.

## Status

Work in progress. The core stack builds and Unity starts. Expect rough edges.

## What's included

PKGBUILDs for the full Unity7 stack, organized by tier:

- **unity-base/** — core components: compiz, nux, unity, bamf, unity-settings-daemon, etc.
- **unity-indicators/** — system indicators: sound, session, power, datetime, keyboard, messages, application, appmenu
- **unity-lens/** — dash lenses and scopes: applications, files, music, video, photos, scope-home, scope-extras
- **dev-libs/** — supporting libraries: libdbusmenu, libindicator, libunity, libcolumbus, etc.

## Building

Build order matters, as packages have dependencies on each other. The general tier order is documented in `downstreamDevlog.md` (yes it's claude vomit, will clean it up later). More user-friendly documentation and AUR packages coming once this is in a more stable state.

Each package builds with standard makepkg:

```
cd pkgbuilds/<category>/<pkgname>
makepkg -si
```

## Upstream

- [gentoo-unity7] — the Gentoo overlay this was forked from
- [Unity7 on Wikipedia][wiki]

[gentoo-unity7]: https://github.com/c4pp4/gentoo-unity7
[unity-gentoo]: https://github.com/shiznix/unity-gentoo
