# =============================================================================
# NovaHost / LightHost — CMake build system
# -----------------------------------------------------------------------------
# Replaces the Projucer .jucer files (LightHost.jucer, NovaHost.jucer) with a
# modern CMake configuration that works natively across Windows/MSVC, macOS,
# and Linux/GCC.  Both the legacy LightHost target and the new NovaHost target
# share the common Source/ tree.
#
# JUCE is fetched via CPM on first configure (no manual submodule setup).
# Override with -DJEUCE_PATH=/path/to/juce if you have a local checkout.
# =============================================================================

cmake_minimum_required(VERSION 3.21)

# ---- Project meta ------------------------------------------------------------
project(LightHostFork LANGUAGES CXX VERSION 1.0.0)

# ---- Global settings ---------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# JUCE uses `juce_add_plugin` / `juce_add_gui_app` etc.
set(JUCE_ENABLE_MODULE_SOURCE_GROUPS 1)

# ---- Options -----------------------------------------------------------------
option(NOVAHOST_BUILD "Build the NovaHost target (primary)" ON)
option(LIGHTHOST_BUILD "Build the legacy LightHost target" OFF)
option(NOVAHOST_SMOKE_TEST "Build the headless smoke-test executable" ON)
option(NOVAHOST_FETCH_JUCE "Fetch JUCE via CPM when not found locally" ON)
option(NOVAHOST_ENABLE_PLUGIN_HOSTS "Enable all audio plugin host backends" ON)
option(NOVAHOST_ASAN "Enable AddressSanitizer (debug builds)" OFF)
option(NOVAHOST_PLUGIN_SCAN_TEST "Build the plugin scan test executable" ON)

# ---- JUCE dependency ---------------------------------------------------------
# Prefer a local checkout; fall back to CPM for CI / first-time configure.
if(NOT DEFINED JUCE_PATH AND NOT TARGET juce)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/lib/juce/CMakeLists.txt")
        set(JUCE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/lib/juce")
    elseif(NOVAHOST_FETCH_JUCE)
        include(FetchContent)
        FetchContent_Declare(
            juce
            GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
            GIT_TAG        8.0.8
        )
        FetchContent_MakeAvailable(juce)
    else()
        message(FATAL_ERROR
            "JUCE not found. Set -DJEUCE_PATH=/path/to/juce or enable "
            "-DNOVAHOST_FETCH_JUCE=ON (default) to download automatically.")
    endif()
endif()

if(NOT TARGET juce::juce)
    # FetchContent path defines the `juce` target; local path defines `juce::juce`.
    find_package(JUCE REQUIRED)
endif()

# =============================================================================
# Shared JUCE module list (from both .jucer files)
# =============================================================================
set(JUCE_MODULES
    juce_audio_basics
    juce_audio_devices
    juce_audio_formats
    juce_audio_processors
    juce_audio_utils
    juce_core
    juce_cryptography
    juce_data_structures
    juce_events
    juce_graphics
    juce_gui_basics
    juce_gui_extra
    juce_opengl
    juce_video
)

# =============================================================================
# Common compile options
# =============================================================================
if(MSVC)
    add_compile_options(/W4 /permissive- /utf-8 /Zc:__cplusplus /Zc:preprocessor)
    # Release: optimise, LTCG, static CRT (matches original useRuntimeLibDLL="0")
    add_compile_options($<$<CONFIG:Release>:/O2 /GL /arch:AVX2>)
    add_link_options($<$<CONFIG:Release>:/LTCG>)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS NOMINMAX)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
    add_compile_options($<$<CONFIG:Release>:-O3>)
    # ASIO SDK needs -fpermissive in some configs
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive")
endif()

if(NOVAHOST_ASAN)
    add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address)
endif()

# =============================================================================
# JUCE feature flags (mirrors <JUCEOPTIONS> from .jucer)
# =============================================================================
set(JUCE_WEB_BROWSER OFF)          # disabled in .jucer
set(JUCE_USE_FLAC OFF)
set(JUCE_USE_OGGVORBIS OFF)
set(JUCE_USE_CDBURNER OFF)
set(JUCE_USE_CDREADER OFF)
set(JUCE_USE_CAMERA OFF)
set(JUCE_QUICKTIME OFF)

if(NOVAHOST_ENABLE_PLUGIN_HOSTS)
    set(JUCE_PLUGINHOST_VST     ON)
    set(JUCE_PLUGINHOST_VST3    ON)
    set(JUCE_PLUGINHOST_AU      ON)
    set(JUCE_PLUGINHOST_AU_AIRMUSIC ON)
    set(JUCE_PLUGINHOST_LADSPA  ON)
    set(JUCE_PLUGINHOST_LV2     ON)
    set(JUCE_PLUGINHOST_AAX     ON)
    set(JUCE_PLUGINHOST_ARA     ON)
endif()

# Audio API
set(JUCE_WASAPI ON)
set(JUCE_WASAPI_EXCLUSIVE ON)
set(JUCE_DIRECTSOUND ON)
set(JUCE_DIRECTSOUND_PREFERRED_BIT_DEPTH 32)
set(JUCE_ASIO ON)
set(JUCE_ALSA ON)

set(JUCE_STRICT_REFCOUNTEDPOINTER ON)

# =============================================================================
# Shared source library (headers + sources from Source/)
# The original .jucer files compiled:
#   Source/PluginWindow.cpp
#   Source/HostStartup.cpp
#   Source/IconMenu.cpp
# =============================================================================
set(NOVAHOST_SOURCES
    Source/PluginWindow.cpp
    Source/HostStartup.cpp
    Source/IconMenu.cpp
)

set(NOVAHOST_HEADERS
    Source/PluginWindow.h
    Source/IconMenu.hpp
    Source/SafePluginScanner.h
    Source/GPUAccelerationManager.h
    Source/SplashScreen.h
    Source/ThreadPool.h
    Source/GPUAccelerationManager.cpp
    Source/SplashScreen.cpp
)

# =============================================================================
# NovaHost GUI application  (primary target)
# =============================================================================
if(NOVAHOST_BUILD)
    juce_add_gui_app(NovaHost
        PRODUCT_NAME "Nova Host"
        COMPANY_NAME "NovaHost Developers"
        COMPANY_WEBSITE "https://github.com/NovaHost"
        COMPANY_EMAIL "info@novahost.com"
        VERSION 1.0.0
        VERSION_CODENAME ""
        PLUGIN_CODE "NHST"
        BUNDLE_IDENTIFIER "com.novahost.app"
    )

    target_sources(NovaHost
        PRIVATE
            ${NOVAHOST_SOURCES}
            ${NOVAHOST_HEADERS}
            Resources/icon.png
            Resources/menu_icon.png
            Resources/menu_icon_white.png
    )

    target_link_libraries(NovaHost PRIVATE ${JUCE_MODULES})

    target_compile_definitions(NovaHost PRIVATE
        JUCE_APP_NAME="Nova Host"
        JUCE_APP_VERSION="1.0.0"
        JUCE_GLOBAL_MODULE_SETTINGS=1
    )

    set_target_properties(NovaHost PROPERTIES
        WINDOWS_EXPORT_ALL_SYMBOLS OFF
    )

    # Post-build: copy manifest (matches original postbuildCommand)
    if(WIN32)
        add_custom_command(TARGET NovaHost POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE_DIR:NovaHost>/Installer/NovaHost.manifest
                $<TARGET_FILE:NovaHost>.manifest
            COMMENT "Copying NovaHost.manifest to output directory"
        )
    endif()

    # macOS bundle settings
    if(APPLE)
        set_target_properties(NovaHost PROPERTIES
            MACOSX_BUNDLE TRUE
            MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Installer/NovaHost.plist.in"
        )
    endif()
endif()

# =============================================================================
# Legacy LightHost GUI application
# =============================================================================
if(LIGHTHOST_BUILD)
    juce_add_gui_app(LightHost
        PRODUCT_NAME "Light Host"
        COMPANY_NAME "Rolando Islas"
        COMPANY_WEBSITE "https://www.rolandoislas.com"
        COMPANY_EMAIL "admin@rolandoislas.com"
        VERSION 1.3.0
        PLUGIN_CODE "Abcd"
        BUNDLE_IDENTIFIER "com.rolandoislas.lighthost"
    )

    # LightHost .jucer only compiled PluginWindow + HostStartup + IconMenu;
    # the NovaHost additions (SafePluginScanner, SplashScreen, GPU accel)
    # are not part of the legacy LightHost build.
    set(LIGHTHOST_SOURCES
        Source/PluginWindow.cpp
        Source/HostStartup.cpp
        Source/IconMenu.cpp
    )

    target_sources(LightHost PRIVATE ${LIGHTHOST_SOURCES})
    target_link_libraries(LightHost PRIVATE ${JUCE_MODULES})

    if(WIN32)
        add_custom_command(TARGET LightHost POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                Installer/LightHost.manifest
                $<TARGET_FILE:LightHost>.manifest
        )
    endif()
endif()

# =============================================================================
# Smoke test (headless — validates JUCE initialisation & module linking)
# =============================================================================
if(NOVAHOST_SMOKE_TEST)
    enable_testing()
    add_custom_target(smoke_test)

    juce_add_gui_app(NovaHostSmokeTest
        PRODUCT_NAME "NovaHostSmokeTest"
        VERSION 1.0.0
        IS_HEADLESS True
    )

    target_sources(NovaHostSmokeTest PRIVATE
        Tests/smoke_test_main.cpp
        ${NOVAHOST_SOURCES}
    )

    target_link_libraries(NovaHostSmokeTest PRIVATE ${JUCE_MODULES})

    # Headless runs must not require a display
    if(APPLE)
        set_target_properties(NovaHostSmokeTest PROPERTIES
            MACOSX_BUNDLE TRUE
        )
    endif()

    # Register with CTest
    add_test(
        NAME NovaHostSmokeTest
        COMMAND NovaHostSmokeTest --smoke-test
    )

    add_dependencies(smoke_test NovaHostSmokeTest)

    # Coverage target (Linux/GCC — optional, opt-in)
    option(NOVAHOST_ENABLE_COVERAGE "Enable code coverage instrumentation" OFF)
    if(NOVAHOST_ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        target_compile_options(NovaHost PRIVATE --coverage -O0 -g)
        target_link_options(NovaHost PRIVATE --coverage)
        set(CTEST_COVERAGE_COMMAND gcov)
    endif()
endif()

# =============================================================================
# Plugin Scan Test (validates SafePluginScanner integration)
# =============================================================================
if(NOVAHOST_PLUGIN_SCAN_TEST)
    enable_testing()
    add_custom_target(plugin_scan_test)

    juce_add_gui_app(NovaHostPluginScanTest
        PRODUCT_NAME "NovaHostPluginScanTest"
        VERSION 1.0.0
        IS_HEADLESS True
    )

    target_sources(NovaHostPluginScanTest PRIVATE
        Tests/plugin_scan_test_main.cpp
        ${NOVAHOST_SOURCES}
    )

    target_link_libraries(NovaHostPluginScanTest PRIVATE ${JUCE_MODULES})

    # Headless runs must not require a display
    if(APPLE)
        set_target_properties(NovaHostPluginScanTest PROPERTIES
            MACOSX_BUNDLE TRUE
        )
    endif()

    # Register with CTest
    add_test(
        NAME NovaHostPluginScanTest
        COMMAND NovaHostPluginScanTest --scan-test
    )

    add_dependencies(plugin_scan_test NovaHostPluginScanTest)
endif()

# =============================================================================
# Package configuration (CPack)
# =============================================================================
set(CPACK_PACKAGE_NAME "NovaHost")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_DESCRIPTION "NovaHost — Audio Plugin Host")
if(WIN32)
    set(CPACK_GENERATOR "ZIP;NSIS")
elseif(APPLE)
    set(CPACK_GENERATOR "ZIP;DragNDrop")
else()
    set(CPACK_GENERATOR "ZIP;TGZ")
endif()
include(CPack)
