# AGENTS.md

## What this is

Qt5 C++ desktop app — digital photo frame for Raspberry Pi (also runs on Windows). Reads photos from SMB network shares, displays a slideshow with crossfade, and supports an RTSP camera viewer. UI is in Russian.

## Build

- **Build system:** qmake (`DigitalPhotoFrame.pro`)
- **Qt version:** 5.12.12, MinGW 32-bit
- **Qt path:** `C:/Qt/Qt5.12.12/5.12.12/mingw73_32`
- **Modules used:** `core gui widgets network concurrent multimedia multimediawidgets`
- **C++ standard:** C++14

Build with:
```
qmake DigitalPhotoFrame.pro
make
```

No tests, no CI, no linter, no formatter.

## Project structure

All source files are in the repo root (flat layout):

| File | Role |
|---|---|
| `main.cpp` | Entry point — creates `PhotoFrame` |
| `photoframe.cpp/h` | Main window, slideshow engine, settings dialog, schedule, RTSP toggle |
| `imagedisplay.cpp/h` | Custom widget that paints a scaled pixmap |
| `rtspviewer.cpp/h` | RTSP video playback via `QMediaPlayer` + `QVideoWidget` |
| `playlistmanager.cpp/h` | Reads/writes `playlist.txt` (one path per line) |
| `config.cpp/h` | `SmbConfig` struct, reads/writes `photoframe.ini` via `QSettings` |

## Runtime files (generated, not in repo)

- `photoframe.ini` — all settings (SMB creds, interval, schedule, RTSP URL)
- `playlist.txt` — cached image paths from last SMB scan
- `tasks.txt` — task list shown in tasks dialog (one task per line)

## Key architecture notes

- **Crossfade:** Two `ImageDisplay` widgets (`m_viewA`/`m_viewB`) with `QGraphicsOpacityEffect`, animated with `QPropertyAnimation`. Active view opacity toggles between 0 and 1.
- **Image loading:** `QtConcurrent::run` offloads `QImageReader` reads; results marshalled back via `QMetaObject::invokeMethod`.
- **SMB mount:** On Linux, mounts via `mount -t cifs` to `/mnt/photoframe`. On Windows, uses `net use`. Config creds are passed on the command line — keep this in mind if adding logging.
- **Sleep mode:** Black overlay widget covers the screen and hides the control bar. On Raspberry Pi, HDMI power control is commented out but ready (`vcgencmd display_power`).
- **Schedule:** Wake/sleep times can cross midnight (e.g., 23:00–07:00). The `checkSchedule` logic handles both orderings.
- **Control bar:** Bottom bar (`m_controlBar`) with 6 buttons (time, date, video, equalizer, tasks, calendar). Time/date buttons update every second via `m_clockTimer`, are non-interactive (`WA_TransparentForMouseEvents`). Bar is hidden during sleep and raised after crossfade.

## Keyboard shortcuts

| Key | Action |
|---|---|
| Left/Right | Previous/next slide |
| Space | Toggle slideshow timer |
| V | Toggle RTSP viewer |
| S / Esc | Open settings |
| T | Open tasks dialog (read-only list from `tasks.txt`) |
| C | Open calendar dialog |
| Q | Quit |

## Gotchas

- No type system or static analysis — watch for null `m_player` in `RtspViewer` (it can be destroyed during `hideEvent`).
- `config.json` in `.opencode/` says `"build_system": "cmake"` but the repo uses qmake. Ignore that field.
- `PlaylistManager::clear()` deletes the file on settings save, forcing a fresh SMB scan on next launch.
- RTSP via `QMediaPlayer` in Qt 5.12 has limited codec support — may fail silently on H.265 streams.
