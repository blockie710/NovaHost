# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.0.0] - 2026-10-05
### Added
- Tray plugin host (IconMenu) providing a system tray/menu bar icon for managing the plugin chain (Source/IconMenu.hpp, Source/IconMenu.cpp).
- Ability to add, remove, bypass, and reorder plugins in the audio processing chain.
- Access to audio device settings via the tray menu.
- Option to exit the application from the tray menu.
- Dynamic icon color (black/white) on Windows and Linux.
- Hides the dock icon on macOS.
- Support for VST, VST3, Audio Unit (AU, macOS only), Audio Unit v3 (AUv3, macOS only), LADSPA, LV2 (Linux only), AAX, and ARA (requires ARA-capable plugins).
- Plugin scanning performed on a background thread with a splash screen during startup.
- Blacklist feature to skip known problematic plugins.
- Workaround for AAX plugin support: ensure AAX SDK is installed and set vstFolderPC/vstFolderMac in Projucer.
- LV2 plugin support limited to Linux builds due to JUCE implementation.
- Support for Windows 7 and later (tested on Windows 10/11), macOS 10.12 and later (tested on macOS Ventura), Linux (Ubuntu 18.04+, Fedora 30+).
- Tray/menu bar behavior: Windows system tray, macOS menu bar (dock icon hidden), Linux system tray (DE-dependent).
- GPU-accelerated rendering for plugin UIs (optional, enabled by default).
- Completely redesigned plugin scanning system to prevent crashes and improve stability.
- New professional Windows installer with VST/VST3 file associations.
- Improved High DPI handling on Windows 10/11 and modern macOS.
- Enhanced UI with better plugin organization and modern visual design.
- Added command line option: `-multi-instance`.
### Changed
- Project and application name changed from "Light Host" to "Nova Host".
### Fixed
- Fixed memory leaks and improved stability.
### Deprecated
- None
### Removed
- None
### Security
- None

[1.0.0]: https://github.com/blockie710/NovaHost/releases/tag/v1.0.0