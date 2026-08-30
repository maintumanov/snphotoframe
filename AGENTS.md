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
| `setup_raspbian.sh` | One-time RPi OS setup script |
| `update_photoframe.sh` | Deploy script (installed on the Pi as `~/update_photoframe.sh`): pulls from NAS bare repo, builds, restarts the app. Secrets are read from `~/update_photoframe.conf` (NOT in git; template: `update_photoframe.conf.example`) |
| `web/` | Web UI served by WebServer |

## Runtime behavior (SD-card friendly)

- App settings `photoframe.ini`, cached `playlist.txt` — written only on user actions (settings save, SMB scan). No periodic writes.
- App log goes to RAM: `/dev/shm/photoframe.log` (survives until reboot) — never touched by continuous SignalNet telemetry logging.
- journald is volatile (RAM), no persistent journal on the SD card.

## Runtime files (generated, not in repo)

- `photoframe.ini` — all settings (SMB creds, interval, schedule, RTSP URL)
- `playlist.txt` — cached image paths from last SMB scan
- `tasks.txt` — task list shown in tasks dialog (one task per line)

## Key architecture notes

- **Crossfade:** Two QML `Image` elements (`imageA`/`imageB`) animated with `ParallelAnimation` (opacity 0↔1).
- **Image loading:** `QtConcurrent::run` offloads `QImageReader` reads; results pushed to a `QQuickImageProvider` via `QMetaObject::invokeMethod`.
- **SMB mount:** On Linux, mounts via `mount -t cifs` to `/mnt/photoframe`. On Windows, uses `net use`. Config creds are passed on the command line — keep this in mind if adding logging.
- **DSI backlight:** Brightness is written to `/sys/class/backlight/10-0045/brightness` via `QFile` (percentage mapped to `max_brightness`). `brightness` setting in `photoframe.ini` under `display/`. Backlight turns fully OFF (value 0) during sleep mode / absence; a fullscreen black overlay additionally hides content on platforms without backlight control (e.g. Windows). The sysfs write is silently skipped when the file is absent.
- **RTSP viewer:** QML `MediaPlayer` + `VideoOutput` with a retry overlay and auto-fallback to photos.
- **SignalNet integration:** TCP client connects to SignalNet home automation controller. Receives temperature, alerts (fire/water/gas), bell notifications, media commands, absence state. Sends heartbeat pulse. Indicator panel with telemetry (indoor/outdoor temperature, humidity, CO2, dust, VAR) shows top-right only when connected; each card hides when its data goes stale (freshness windows vary per metric — temperature 4h, humidity 2h, VAR 1h, CO2/dust 30min). Telemetry and pending alerts are cleared on disconnect so a reconnect never shows stale data.
- **Control bar:** Bottom bar — buttons: `⏮` (first slide), video 1/2/3 (only for enabled cameras), `A1`/`A2` SignalNet actions (when connected + `useActionButtons`), `⏭` (last slide). Time/date overlay sits top-left and updates every second via `m_tickTimer`. Bar and overlays are hidden during sleep.

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
