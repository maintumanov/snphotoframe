# AGENTS.md

## What this is

Qt5 C++ desktop app — digital photo frame for Raspberry Pi (also runs on Windows). Reads photos from SMB network shares, displays a slideshow with crossfade, and supports an RTSP camera viewer. UI is in Russian.

## Build

- **Build system:** qmake (`DigitalPhotoFrame.pro`)
- **Qt version:** 5.12
- **Qt path:** `C:/Qt/Qt5.12.12/5.12.12/mingw73_32`
- **Modules used:** `core gui qml quick network concurrent multimedia`
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
| `main.cpp` | Entry point — creates `PhotoFrameBackend`, loads QML engine |
| `photoframe.cpp/h` | QML backend (QObject) — slideshow engine, settings, schedule, RTSP toggle |
| `playlistmanager.cpp/h` | Reads/writes `playlist.txt` (one path per line) |
| `config.cpp/h` | `SmbConfig` struct, reads/writes `photoframe.ini` via `QSettings` |
| `signalnet.cpp/h` | Simplified SignalNet TCP client — temperature, alerts, media control |
| `main.qml` | QML UI — crossfade images, settings overlay, tasks, calendar, RTSP, SignalNet, control bar |

## Runtime files (generated, not in repo)

- `photoframe.ini` — all settings (SMB creds, interval, schedule, RTSP URL)
- `playlist.txt` — cached image paths from last SMB scan
- `tasks.txt` — task list shown in tasks dialog (one task per line)

## Key architecture notes

- **Crossfade:** Two QML `Image` elements (`imageA`/`imageB`) animated with `ParallelAnimation` (opacity 0↔1).
- **Image loading:** `QtConcurrent::run` offloads `QImageReader` reads; results pushed to a `QQuickImageProvider` via `QMetaObject::invokeMethod`.
- **SMB mount:** On Linux, mounts via `mount -t cifs` to `/mnt/photoframe`. On Windows, uses `net use`. Config creds are passed on the command line — keep this in mind if adding logging.
- **DSI backlight:** Brightness is written to `/sys/class/backlight/10-0045/brightness` via `QFile` (percentage mapped to `max_brightness`). `brightness` setting in `photoframe.ini` under `display/`. Backlight dims to 5% during sleep mode and is silently skipped when the sysfs file is absent (e.g. Windows).
- **RTSP viewer:** QML `MediaPlayer` + `VideoOutput` with a retry overlay and auto-fallback to photos.
- **SignalNet integration:** TCP client connects to SignalNet home automation controller. Receives temperature, alerts (fire/water/gas), bell notifications, and media commands. Sends heartbeat pulse. Control bar shows temperature when connected.
- **Control bar:** Bottom bar with 7 buttons (time, date, temperature, video, settings, tasks, calendar). Time/date update every second via `m_tickTimer`. Temperature shows when SignalNet is connected. Bar is hidden during sleep.

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

- RTSP via `QMediaPlayer` in Qt 5.12 has limited codec support — may fail silently on H.265 streams.
- SignalNet client uses a simplified version of the snpcagent protocol (device address 3999, no memory blocks, no device info tasks).
