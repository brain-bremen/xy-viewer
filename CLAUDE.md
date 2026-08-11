# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this is

An Open Ephys GUI plugin ("XY Viewer") that plots one continuous channel against another in real time (phase-plane / XY style plotting), with a fading-trail line renderer. It builds as a shared library (`xy-viewer.dll`/`.so`/bundle) that the Open Ephys GUI loads at runtime.

This repo is one of several sibling plugin repos under `D:\open-ephys\OEPlugins\` (e.g. `amplitude-estimator`, `event-triggered-analysis`, `network-events`, `online-psth`, `ttl-panels`). It depends on the Open Ephys GUI source tree, which lives as a sibling checkout at `D:\open-ephys\plugin-GUI` (i.e. `../../plugin-GUI` relative to this repo — see `GUI_BASE_DIR` below).

## Build

This is a CMake project following the standard Open Ephys plugin layout (`Source/` glob-included automatically — no need to edit `CMakeLists.txt` when adding/removing source files).

```
cd Build
cmake -G "Visual Studio 16 2019" -A 64 ..    # Windows; adjust generator/version as needed
```

Then build via the generated `OE_PLUGIN_xy-viewer.sln` in Visual Studio, or:

```
cmake --build . --config Debug
```

- `GUI_BASE_DIR` defaults to `../../plugin-GUI` relative to this repo, or can be overridden via the `GUI_BASE_DIR` environment variable / CMake cache variable. It must point at a plugin-GUI checkout with a built `open-ephys.lib`/binary (the plugin links against it).
- `CMAKE_EXPORT_COMPILE_COMMANDS` is enabled, producing `compile_commands.json` for clangd/LSP use.
- On Windows the built DLL is installed to `${GUI_BASE_DIR}/Build/<Config>/plugins`; on Linux/macOS the equivalent GUI plugins directory is used. There is no separate "install" step for local dev beyond the normal `cmake --build . --target install`.
- No test suite exists in this repo. Verifying changes means building the plugin and loading it in the Open Ephys GUI (add "XY Viewer" from the Sinks category in the signal chain).

Note: `CMakeLists.txt` currently has an uncommitted typo (`target_compile_opthons` instead of `target_compile_options`) which silently disables the `/sdl /W4 /MP /external:...` compiler flags on MSVC — be aware of this when debugging warnings/build flags on Windows.

## Code style

Formatting is enforced via `.clang-format` (Allman braces, 4-space indent, no tabs, JUCE-derived style, `ColumnLimit: 0`, sorted includes). Run clang-format before committing; there's a "Auto-format" commit convention already used in this repo's history for standalone formatting passes (keep formatting changes in their own commit rather than mixed with logic changes).

## Architecture

All plugin code lives under `Source/` in the `XYViewerPlugin` namespace. It follows the standard Open Ephys GUI plugin pattern: a `GenericProcessor` subclass for signal-chain processing, a `VisualizerEditor` subclass for the toolbar UI, and a `Visualizer` (canvas) subclass for the actual plot, wired together via `OpenEphysLib.cpp`.

- **`OpenEphysLib.cpp`** — the plugin's ABI entry point (`getLibInfo`/`getPluginInfo`) that registers `XYViewer` as a `SINK`-type processor with the GUI's plugin manager. Edit this if the plugin's display name or type changes.

- **`XYViewer` (`XYViewer.h/.cpp`)** — the `GenericProcessor`. Owns the signal-chain-facing state:
  - `m_channels`: flat list of all incoming continuous channels (name/streamID/sampleRate), rebuilt in `updateSettings()` whenever the upstream signal chain changes.
  - `m_xChannelIndex`/`m_yChannelIndex`: indices into `m_channels` selected as the current X/Y trace source (set via `setActiveXChannel`/`setActiveYChannel`, called from the editor's combo boxes).
  - `process()` pulls the raw sample pointers for the selected X/Y channel indices out of the audio buffer each callback and pushes them into `m_xyBuffer`.
  - `m_xyBuffer` (a `CircularXYBuffer`) is the thread-safety boundary between the audio/processing thread (`push`, called from `process()`) and the UI/painting thread (`getXYData` → `CircularXYBuffer::getLatest`, called from the canvas's `refresh()`). All cross-thread access is mutex-guarded inside `CircularXYBuffer`.
  - The `keep_window_length` processor parameter (ms of trace history to retain/display) is defined once in `ParameterNames` and flows: parameter UI → `parameterValueChanged()` → `XYViewerCanvas::setRetentionPeriodMs()`.

- **`CircularXYBuffer` (`CircularXYBuffer.h`)** — a fixed-capacity ring buffer (subclass of `juce::AudioBuffer<float>` with channel 0 = X, channel 1 = Y) sized at 60s of samples at 30kHz by default. `push()` writes incoming samples; `getLatest()` copies out the most recent N samples oldest-to-newest for plotting. This is the only place cross-thread synchronization happens in the plugin.

- **`XYViewerEditor` (`XYViewerEditor.h/.cpp`)** — the `VisualizerEditor` shown in the signal chain toolbar. Hosts the X/Y channel selection combo boxes and the retention-period parameter text box. `selectedStreamHasChanged()` repopulates both combo boxes from `XYViewer::getChannelsForStream()` whenever the user picks a different input stream; `comboBoxChanged()` pushes the selection back into the processor.

- **`XYViewerCanvas` (`XYViewerCanvas.h/.cpp`)** — the `Visualizer` that owns the actual plot (`XYFadingTracePlot`, an `InteractivePlot` subclass from the GUI's plotting library). `refresh()` (called on the UI timer) pulls the latest X/Y vectors from the processor and rebuilds an `XyLineFading` line object each frame — there's no incremental update, the whole trace is replotted per refresh. `XyLineFading::draw()` implements the fade effect by interpolating opacity/width along the line from `startOpacity/startWidth` (older samples) to `endOpacity/endWidth` (most recent sample).

### Data flow summary

```
process() [audio thread]
  -> CircularXYBuffer::push()
       (mutex)
XYViewerCanvas::refresh() [UI thread, on timer]
  -> XYViewer::getXYData() -> CircularXYBuffer::getLatest()
  -> builds XyLineFading -> XYFadingTracePlot::plot()
```

Channel selection UI (`XYViewerEditor`) and retention-period parameter both flow through `XYViewer` into `XYViewerCanvas` via the setter methods (`setPlotTitle`, `setRetentionPeriodMs`) rather than the canvas reading processor state directly.
