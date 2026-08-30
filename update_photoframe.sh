#!/bin/bash
# update_photoframe.sh — обновление фоторамки с NAS-сервера (SMB + git),
# сборка и перезапуск. Скрипт живёт в репозитории (под контролем git).
#
# Использование:  ./update_photoframe.sh
# Лог:            ~/update_photoframe.log
# Лог приложения уходит в RAM (/dev/shm/photoframe.log), чтобы не изнашивать SD-карту
# непрерывной записью телеметрии (сигналы SignalNet идут каждые несколько секунд).
# Содержимое сохраняется до перезагрузки/следующего обновления.
#
# Секреты НЕ хранятся в git. Скрипт подгружает ~/update_photoframe.conf
# (шаблон — update_photoframe.conf.example). Содержимое conf:
#   SMB_USER="пользователь_шары"
#   SMB_PASS="пароль_шары"
#   SMB_VERS="3.0"
#   SUDO_PASS="пароль_pi"
version=2.1

set -u

# ---------- Настройки (по умолчанию; переопределяются в $HOME/update_photoframe.conf) ----------
APP_DIR="$HOME/snphotoframe"          # каталог с исходниками фоторамки
RUNNER="$APP_DIR/run.sh"               # скрипт запуска приложения
GIT_MNT="/mnt/git"                     # точка монтирования шары с git
REPO_SMB="//192.168.1.33/git"          # SMB-шара с репозиториями
BARE_REPO="$GIT_MNT/snphotoframe.git"  # bare-репозиторий
GIT_BRANCH="main"                      # ветка обновления

SMB_USER="stanislav"                   # пользователь SMB-шары
SMB_PASS=""                            # пароль SMB-шары (из conf)
SMB_VERS="3.0"                         # версия SMB
SUDO_PASS=""                           # пароль для sudo (из conf)
# --------------------------------------------------------------------------------------------

LOG="$HOME/update_photoframe.log"
PLOG="/dev/shm/photoframe.log"          # лог самой фоторамки (в RAM, без записи на SD)

CONF="$HOME/update_photoframe.conf"
if [ -f "$CONF" ]; then
    # shellcheck disable=SC1090
    . "$CONF"
fi

ts() { date '+%F %T'; }
log() { echo "$(ts)  $*" | tee -a "$LOG"; }

# sudo с паролем из stdin в неинтерактивном режиме
sudo_sh() { echo "$SUDO_PASS" | sudo -S "$@" 2>>"$LOG" >>"$LOG"; }

if [ -z "$SMB_PASS" ] || [ -z "$SUDO_PASS" ]; then
    log "ОШИБКА: SMB_PASS/SUDO_PASS не заданы. Создайте $CONF (см. update_photoframe.conf.example)"
    exit 1
fi

log "=== Обновление фоторамки (скрипт v$version) ==="
log "Хост: $(hostname), ветка: $GIT_BRANCH"

# ============================================================
# 1. Монтирование SMB-шары с git-репозиториями
# ============================================================
if grep -q " $GIT_MNT " /proc/mounts; then
    log "Шара $GIT_MNT уже смонтирована"
else
    sudo_sh mkdir -p "$GIT_MNT"
    if ! sudo_sh mount -t cifs "$REPO_SMB" "$GIT_MNT" \
           -o "username=$SMB_USER,password=$SMB_PASS,vers=$SMB_VERS,uid=$(id -u),gid=$(id -g),noperm,file_mode=0755,dir_mode=0755"; then
        log "ОШИБКА: не удалось смонтировать $REPO_SMB -> $GIT_MNT"
        exit 1
    fi
    log "Смонтирована шара $REPO_SMB -> $GIT_MNT"
fi

if [ ! -d "$BARE_REPO" ]; then
    log "ОШИБКА: не найден репозиторий $BARE_REPO"
    exit 1
fi

# Доверяем репозиторию на cifs (файлы могут принадлежать root/другому uid)
git config --global --add safe.directory "$BARE_REPO" 2>/dev/null || true

# ============================================================
# 2. Обновление исходников
# ============================================================
if [ ! -d "$APP_DIR/.git" ]; then
    log "ОШИБКА: $APP_DIR не является git-репозиторием"
    exit 1
fi
cd "$APP_DIR" || exit 1

log "Текущая ревизия: $(git rev-parse --short HEAD)"

if ! git fetch "$BARE_REPO" "$GIT_BRANCH" 2>>"$LOG" >>"$LOG"; then
    log "ОШИБКА: git fetch не удался"
    exit 1
fi

NEW_REV=$(git log -1 --format='%H' FETCH_HEAD)
if [ "$NEW_REV" = "$(git rev-parse HEAD)" ]; then
    log "Изменений в git нет (уже $(git rev-parse --short HEAD))"
else
    log "Найдена новая ревизия: $(git log -1 --format='%h %s' FETCH_HEAD)"
    # Страховка: локальные правки сохраняем в stash перед сбросом.
    # ВНИМАНИЕ: без -u, чтобы не трогать untracked-файлы устройства (например run.sh).
    if ! git diff --quiet || ! git diff --cached --quiet; then
        git stash push -m "before-deploy $(ts)" 2>>"$LOG" >>"$LOG" \
            && log "Локальные правки сохранены в git stash (восстановить: git stash pop)"
    fi
    git reset --hard FETCH_HEAD 2>>"$LOG" >>"$LOG" || { log "ОШИБКА: git reset --hard"; exit 1; }
    log "Рабочий каталог обновлён до $(git rev-parse --short HEAD)"
fi

# ============================================================
# 3. Сборка
# ============================================================
log "Сборка..."
if ! qmake DigitalPhotoFrame.pro 2>>"$LOG" >>"$LOG"; then
    log "ОШИБКА: qmake"
    tail -n 30 "$LOG"
    exit 1
fi
if ! make -j4 2>>"$LOG" >>"$LOG"; then
    log "ОШИБКА: make (ошибки сборки)"
    tail -n 40 "$LOG"
    exit 1
fi
if [ ! -x "$APP_DIR/DigitalPhotoFrame" ]; then
    log "ОШИБКА: бинарник DigitalPhotoFrame не создан"
    exit 1
fi
log "Сборка успешна: $(stat -c '%s' "$APP_DIR/DigitalPhotoFrame") байт"

# ============================================================
# 4. Перезапуск приложения
# ============================================================
# run.sh — локальный файл устройства (не в git). Если пропал — создаём заново.
if [ ! -f "$RUNNER" ]; then
    log "run.sh отсутствует — создаю заново"
    cat > "$RUNNER" <<'RUN_EOF'
#!/bin/bash
sleep 3
export QT_QPA_PLATFORM=wayland-egl
export QT_WAYLAND_DISABLE_WINDOWDECORATION=1
export XDG_RUNTIME_DIR=/run/user/1000
export WAYLAND_DISPLAY=wayland-0
cd /home/pi/snphotoframe
exec ./DigitalPhotoFrame
RUN_EOF
    chmod +x "$RUNNER"
fi

# Паттерн [D]igitalPhotoFrame ловит и "./DigitalPhotoFrame" и полный путь,
# но не сам pkill/bash (классический трюк с квадратными скобками).
pkill -f "[D]igitalPhotoFrame" 2>/dev/null && sleep 1
pkill -f "[D]igitalPhotoFrame" 2>/dev/null && sleep 1
pkill -f "[D]igitalPhotoFrame" 2>/dev/null && sleep 1

nohup "$RUNNER" >>"$PLOG" 2>&1 &
BGPID=$!
disown
log "Фоторамка запущена (pid $BGPID), лог: $PLOG"

sleep 8
if pgrep -f "[D]igitalPhotoFrame" >/dev/null 2>&1; then
    log "OK: процесс фоторамки работает"
else
    log "ВНИМАНИЕ: процесс не найден. Хвост $PLOG:"
    tail -n 15 "$PLOG"
fi

log "=== Обновление завершено ==="