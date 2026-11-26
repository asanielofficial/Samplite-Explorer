# Samplite Explorer

A Windows 95-style browser for audio samples and synth presets, written in C++ with JUCE 8.

## Build

You need CMake 3.22+, Git and a C++17 compiler. The first configure downloads JUCE.

- **Windows:** Visual Studio 2022 with "Desktop development with C++"
- **macOS:** Xcode command line tools
- **Linux:** `sudo apt install g++ cmake git pkg-config libasound2-dev libx11-dev libxext-dev libxinerama-dev libxrandr-dev libxcursor-dev libfreetype-dev libfontconfig1-dev libgl1-mesa-dev`

Then, in this folder:

    cmake -B build -DCMAKE_BUILD_TYPE=Release
    cmake --build build --config Release

The app is under `build/SampliteExplorer_artefacts/`. Tests build as `ExplorerTests`.

## Build with GitHub Actions

Put the contents of this folder at the root of a GitHub repository, including the hidden `.github` folder:

    git init -b main
    git add .
    git commit -m "Add Samplite Explorer"
    git remote add origin https://github.com/YOUR-NAME/YOUR-REPO.git
    git push -u origin main

The **Build** workflow runs on every push and pull request, or by hand from the Actions tab.
It builds and tests on Windows, macOS and Linux. Download the apps from the **Artifacts**
section of a finished run.

The builds are not signed with a developer certificate, so Windows SmartScreen or macOS Gatekeeper
may warn on first launch. On macOS, allow it under System Settings > Privacy & Security.

## Use

- **Add Folder...** or drop a folder onto the window.
- Click or use the arrow keys to audition; **Space** plays and stops; click the waveform to seek.
- Drag a row out of the window into your DAW, synth or file manager. Files are copied or referenced, never moved.
- Filter by text, type, BPM range and key. Click column headers to sort, including by tuning.

## Notes

- **BPM and key** come from the filename if present, otherwise from analysis of the first 30 s.
  A `~` marks an estimate. Estimates can be wrong, including half or double tempo, and relative major/minor.
- **Tuning** is the A4 reference in Hz (440, 432 and so on), from the filename (`432Hz`) or estimated.
  Estimates cover about 428 to 453 Hz. Noisy or drum material is usually left blank.
- **Presets** are listed with metadata only; they are not played (that would require hosting the synth).
  - `.vital`: name, author, style
  - `.fxp`: program name and plugin ID (Serum 1 is recognised)
  - `.fxb`, `.vstpreset`: filename and plugin/class ID
  - `.SerumPreset`: based on an assumed file layout; falls back to the filename
- **Formats:** WAV, AIFF, FLAC, Ogg and MP3, plus whatever the operating system's codecs add.
- Settings and the analysis cache live in a `SampliteExplorer` folder in your user application data.
- JUCE is licensed under AGPLv3 or a commercial licence; check this before distributing the app.
