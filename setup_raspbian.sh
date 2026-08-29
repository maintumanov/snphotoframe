#!/bin/bash
# ============================================================================
# snphotoframe — полная установка на чистый Raspbian
# ============================================================================
# Запускать от root:  sudo bash setup_raspbian.sh
#
# Что делает:
#   1. Устанавливает системные пакеты и Qt5
#   2. Клонирует проект из git
#   3. Компилирует приложение
#   4. Создаёт systemd-сервис snphotoframe
#   5. Настраивает CIFS-монтирование фотографий
#   6. Настраивает автозапуск на дисплее (без X)
#   7. Отключает скринсейвер иPower Management
# ============================================================================

set -euo pipefail

# --- Настройки (можно изменить) ---
GIT_REPO="https://github.com/maintumanov/snphotoframe.git"
INSTALL_DIR="/opt/snphotoframe"
SERVICE_NAME="snphotoframe"
APP_USER="pi"

# SMB-параметры по умолчанию (настраиваются через веб-интерфейс)
SMB_SERVER="192.168.1.33"
SMB_SHARE="photo"
SMB_MOUNT="/mnt/photoframe"
SMB_GUEST=true
SMB_VERSION="3.0"

# Веб-интерфейс
WEB_PORT=8080

# Цвета для вывода
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

log()    { echo -e "${GREEN}[✓]${NC} $1"; }
warn()   { echo -e "${YELLOW}[!]${NC} $1"; }
error()  { echo -e "${RED}[✗]${NC} $1"; }
step()   { echo -e "\n${BLUE}═══ $1 ═══${NC}"; }

# ============================================================================
# Проверка прав root
# ============================================================================
if [[ $EUID -ne 0 ]]; then
    error "Запустите скрипт от root: sudo bash $0"
    exit 1
fi

step "1/7  Системные пакеты"

apt-get update -qq
apt-get install -y -qq \
    build-essential \
    git \
    wget \
    curl \
    ca-certificates \
    gnupg \
    cifs-utils \
    network-manager \
    > /dev/null 2>&1

log "Базовые пакеты установлены"

# ============================================================================
# Qt5 и зависимости
# ============================================================================
step "2/7  Qt5 и библиотеки"

apt-get install -y -qq \
    qtbase5-dev \
    qtdeclarative5-dev \
    qtmultimedia5-dev \
    libqt5quick5 \
    libqt5quickcontrols2-5 \
    libqt5multimedia5 \
    libqt5network5 \
    qml-module-qtquick2 \
    qml-module-qtquick-controls2 \
    qml-module-qtquick-layouts \
    qml-module-qtquick-window2 \
    qml-module-qtmultimedia \
    qml-module-qt-labs-folderlistmodel \
    libqt5serialport5-dev \
    gstreamer1.0-plugins-base \
    gstreamer1.0-plugins-good \
    gstreamer1.0-plugins-bad \
    gstreamer1.0-libav \
    > /dev/null 2>&1

log "Qt5 и мультимедиа установлены"

# ============================================================================
# Клонирование проекта
# ============================================================================
step "3/7  Клонирование проекта"

if [[ -d "$INSTALL_DIR/.git" ]]; then
    warn "Проект уже существует в $INSTALL_DIR — обновляю..."
    cd "$INSTALL_DIR"
    git pull --ff-only || {
        warn "git pull не удался, делаю fetch + reset"
        git fetch origin
        git reset --hard origin/main
    }
else
    rm -rf "$INSTALL_DIR"
    git clone "$GIT_REPO" "$INSTALL_DIR"
    cd "$INSTALL_DIR"
fi

log "Проект склонирован в $INSTALL_DIR"

# ============================================================================
# Компиляция
# ============================================================================
step "4/7  Компиляция"

# Очистка предыдущей сборки
make clean 2>/dev/null || true
rm -f Makefile .qmake.stash

qmake DigitalPhotoFrame.pro CONFIG+=release
make -j$(nproc)

if [[ ! -f DigitalPhotoFrame ]]; then
    error "Компиляция не удалась!"
    exit 1
fi

chmod +x DigitalPhotoFrame
log "Компиляция завершена: $(ls -lh DigitalPhotoFrame | awk '{print $5}')"

# ============================================================================
# Systemd сервис
# ============================================================================
step "5/7  Systemd сервис"

cat > /etc/systemd/system/${SERVICE_NAME}.service << EOF
[Unit]
Description=Digital Photo Frame (snphotoframe)
After=network-online.target graphical.target
Wants=network-online.target

[Service]
Type=simple
User=${APP_USER}
Group=${APP_USER}
WorkingDirectory=${INSTALL_DIR}
Environment=QT_QPA_PLATFORM=eglfs
Environment=QT_IM_MODULE=evdev
Environment=DISPLAY=:0
Environment=HOME=/home/${APP_USER}
ExecStartPre=/bin/sleep 5
ExecStart=${INSTALL_DIR}/DigitalPhotoFrame
Restart=on-failure
RestartSec=10
StandardOutput=journal
StandardError=journal
KillMode=mixed
TimeoutStopSec=10

[Install]
WantedBy=multi-user.target
EOF

systemctl daemon-reload
systemctl enable ${SERVICE_NAME}

log "Сервис ${SERVICE_NAME} создан и включён в автозапуск"

# ============================================================================
# CIFS монтирование
# ============================================================================
step "6/7  CIFS-монтирование фотографий"

mkdir -p "$SMB_MOUNT"

if [[ "$SMB_GUEST" == "true" ]]; then
    SMB_OPTS="guest,vers=${SMB_VERSION},noperm,nofail,auto,_netdev"
else
    SMB_OPTS="vers=${SMB_VERSION},noperm,nofail,auto,_netdev"
fi

# Добавляем в fstab если ещё нет
FSTAB_LINE="//${SMB_SERVER}/${SMB_SHARE} ${SMB_MOUNT} cifs ${SMB_OPTS} 0 0"
if ! grep -q "$SMB_MOUNT" /etc/fstab 2>/dev/null; then
    echo "$FSTAB_LINE" >> /etc/fstab
    log "fstab обновлён"
else
    warn "fstab уже содержит $SMB_MOUNT"
fi

# Монтируем сейчас
if ! mountpoint -q "$SMB_MOUNT" 2>/dev/null; then
    mount "$SMB_MOUNT" 2>/dev/null && log "CIFS смонтирован: $SMB_MOUNT" || warn "CIFS не смонтирован (сеть может быть недоступна)"
else
    log "CIFS уже смонтирован: $SMB_MOUNT"
fi

# ============================================================================
# Настройка дисплея и автозапуска
# ============================================================================
step "7/7  Дисплей и автозапуск"

# Отключаем скринсейвер и power management
cat > /etc/xdg/autostart/screensaver-off.desktop << 'EOF'
[Desktop Entry]
Type=Application
Name=Disable Screensaver
Exec=bash -c "xset s off; xset -dpms; xset s noblank"
Hidden=true
EOF

# Отключаем логин через GUI (автологин)
if [[ -d /etc/lightdm/lightdm.conf ]]; then
    sed -i 's/^#autologin-user=.*/autologin-user='"$APP_USER"'/' /etc/lightdm/lightdm.conf 2>/dev/null || true
fi

# Создаём скрипт запуска для EGLFS (без X)
cat > "${INSTALL_DIR}/run.sh" << RUNEOF
#!/bin/bash
export QT_QPA_PLATFORM=eglfs
export QT_IM_MODULE=evdev
export HOME=/home/${APP_USER}
cd ${INSTALL_DIR}
exec ./DigitalPhotoFrame
RUNEOF
chmod +x "${INSTALL_DIR}/run.sh"

# N戮 display kernel parameter (DSL / HDMI)
if ! grep -q "dtoverlay=vc4-kms-v3d" /boot/config.txt 2>/dev/null && \
   ! grep -q "dtoverlay=vc4-fkms-v3d" /boot/config.txt 2>/dev/null; then
    warn "Не найден dtoverlay для GPU — добавьте в /boot/config.txt если нужен HW-accel"
fi

# Отключаем GPU memory split для лучшей производительности
if ! grep -q "gpu_mem=128" /boot/config.txt 2>/dev/null; then
    warn "Рекомендуется добавить gpu_mem=128 в /boot/config.txt"
fi

# ============================================================================
# Firewall — открываем порт веб-интерфейса
# ============================================================================
if command -v ufw &> /dev/null; then
    ufw allow ${WEB_PORT}/tcp 2>/dev/null || true
    log "Порт ${WEB_PORT} открыт в firewall"
fi

# ============================================================================
# Готово
# ============================================================================
echo ""
echo -e "${GREEN}════════════════════════════════════════════════════════════${NC}"
echo -e "${GREEN}  Установка завершена!${NC}"
echo -e "${GREEN}════════════════════════════════════════════════════════════${NC}"
echo ""
echo "  Директория:     ${INSTALL_DIR}"
echo "  Сервис:         systemctl status ${SERVICE_NAME}"
echo "  Логи:           journalctl -u ${SERVICE_NAME} -f"
echo "  Веб-интерфейс:  http://$(hostname -I | awk '{print $1}'):${WEB_PORT}"
echo "  Конфиг:         ${INSTALL_DIR}/photoframe.ini (создаётся автоматически)"
echo ""
echo "  Управление:"
echo "    systemctl start ${SERVICE_NAME}     # запуск"
echo "    systemctl stop ${SERVICE_NAME}      # остановка"
echo "    systemctl restart ${SERVICE_NAME}   # перезапуск"
echo ""
echo "  Ручной запуск (для отладки):"
echo "    cd ${INSTALL_DIR} && ./run.sh"
echo ""
echo "  Первый запуск:"
echo "    1. Откройте http://$(hostname -I | awk '{print $1}'):${WEB_PORT} в браузере"
echo "    2. Настройте подключение к SMB-серверу с фотографиями"
echo "    3. Настройте SignalNet (если используется)"
echo "    4. Сохраните — приложение перезапустится"
echo ""
