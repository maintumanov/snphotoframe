import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.5
import QtMultimedia 5.12

ApplicationWindow {
    id: root
    visible: true
    visibility: Window.FullScreen
    color: "black"
    title: "DigitalPhotoFrame"

    property bool showingA: true
    property bool loading: true
    property string loadingText: "\u0417\u0430\u0433\u0440\u0443\u0437\u043a\u0430..."

    Connections {
        target: backend
        onImageChanged: {
            loading = false
            if (showingA) {
                imageB.source = backend.currentImagePath
                imageB.opacity = 0.0
                animB.start()
            } else {
                imageA.source = backend.currentImagePath
                imageA.opacity = 0.0
                animA.start()
            }
            showingA = !showingA
        }
        onSleepChanged: { sleepOverlay.visible = sleeping }
        onRtspPlay: {
            rtspPlayer.source = url
            rtspPlayer.play()
        }
        onRtspStopPlayer: {
            rtspPlayer.stop()
        }
        onRtspShowOverlay: {
            connectingOverlay.visible = true
        }
        onRtspHideOverlay: {
            connectingOverlay.visible = false
        }
        onRtsp2Play: { rtspPlayer2.source = url; rtspPlayer2.play() }
        onRtsp2StopPlayer: { rtspPlayer2.stop() }
        onRtsp2ShowOverlay: { connectingOverlay2.visible = true }
        onRtsp2HideOverlay: { connectingOverlay2.visible = false }
        onRtsp3Play: { rtspPlayer3.source = url; rtspPlayer3.play() }
        onRtsp3StopPlayer: { rtspPlayer3.stop() }
        onRtsp3ShowOverlay: { connectingOverlay3.visible = true }
        onRtsp3HideOverlay: { connectingOverlay3.visible = false }
        onSignalNetAlertChanged: {
            if (backend.signalNetAlert.length > 0) {
                snAlertBanner.visible = true
                snAlertBanner.opacity = 1
            }
        }
    }

    Item {
        anchors.fill: parent
        z: 0

        Image {
            id: imageA
            anchors.fill: parent
            fillMode: Image.PreserveAspectFit
            smooth: true
            opacity: 1.0
            visible: (backend.rtspState || 0) !== 2
        }

        Image {
            id: imageB
            anchors.fill: parent
            fillMode: Image.PreserveAspectFit
            smooth: true
            opacity: 0.0
            visible: (backend.rtspState || 0) !== 2
        }

        ParallelAnimation { id: animA
            NumberAnimation { target: imageA; property: "opacity"; from: 0.0; to: 1.0; duration: 1500; easing.type: Easing.InOutQuad }
            NumberAnimation { target: imageB; property: "opacity"; from: 1.0; to: 0.0; duration: 1500; easing.type: Easing.InOutQuad }
        }
        ParallelAnimation { id: animB
            NumberAnimation { target: imageB; property: "opacity"; from: 0.0; to: 1.0; duration: 1500; easing.type: Easing.InOutQuad }
            NumberAnimation { target: imageA; property: "opacity"; from: 1.0; to: 0.0; duration: 1500; easing.type: Easing.InOutQuad }
        }

        MediaPlayer {
            id: rtspPlayer
            autoPlay: false
            onErrorChanged: {
                if (error !== MediaPlayer.NoError)
                    backend.onRtspError(errorString)
            }
            onPlaybackStateChanged: {
                if (playbackState === MediaPlayer.PlayingState)
                    backend.onRtspPlaying()
            }
        }

        VideoOutput {
            anchors.fill: parent
            source: rtspPlayer
            visible: (backend.rtspState || 0) === 2 && rtspPlayer.playbackState === MediaPlayer.PlayingState
        }

        Rectangle {
            id: connectingOverlay
            anchors.fill: parent
            color: "black"
            visible: false
            z: 1
            opacity: 0
            Behavior on opacity { NumberAnimation { duration: 300 } }
            onVisibleChanged: { if (visible) opacity = 1; else opacity = 0 }

            // Animated camera icon
            Column {
                anchors.centerIn: parent
                spacing: 24

                Item { width: 120; height: 120; anchors.horizontalCenter: parent.horizontalCenter
                    // Pulsing ring
                    Rectangle {
                        id: pulseRing
                        width: 120; height: 120; radius: 60
                        color: "transparent"
                        border.color: (backend.rtspState || 0) === 3 ? "#f55" : "#5af"
                        border.width: 3
                        anchors.centerIn: parent
                        SequentialAnimation on scale {
                            running: connectingOverlay.visible
                            loops: Animation.Infinite
                            NumberAnimation { from: 0.8; to: 1.3; duration: 1200; easing.type: Easing.InOutQuad }
                            NumberAnimation { from: 1.3; to: 0.8; duration: 1200; easing.type: Easing.InOutQuad }
                        }
                        SequentialAnimation on opacity {
                            running: connectingOverlay.visible
                            loops: Animation.Infinite
                            NumberAnimation { from: 0.8; to: 0.2; duration: 1200; easing.type: Easing.InOutQuad }
                            NumberAnimation { from: 0.2; to: 0.8; duration: 1200; easing.type: Easing.InOutQuad }
                        }
                    }
                    // Camera icon
                    Text {
                        text: (backend.rtspState || 0) === 3 ? "\u26a0" : "\u25b6"
                        color: (backend.rtspState || 0) === 3 ? "#f55" : "#5af"
                        font.pixelSize: 48
                        anchors.centerIn: parent
                        SequentialAnimation on opacity {
                            running: connectingOverlay.visible
                            loops: Animation.Infinite
                            NumberAnimation { from: 1.0; to: 0.4; duration: 800; easing.type: Easing.InOutQuad }
                            NumberAnimation { from: 0.4; to: 1.0; duration: 800; easing.type: Easing.InOutQuad }
                        }
                    }
                }

                // Status text
                Text {
                    id: statusText
                    text: (backend.rtspState || 0) === 3 ? backend.rtspErrorMsg : backend.rtspErrorMsg
                    color: "white"
                    font.pixelSize: 24
                    font.bold: true
                    anchors.horizontalCenter: parent.horizontalCenter
                    opacity: 1
                    SequentialAnimation on opacity {
                        running: connectingOverlay.visible && (backend.rtspState || 0) !== 3
                        loops: Animation.Infinite
                        NumberAnimation { from: 1.0; to: 0.5; duration: 1000; easing.type: Easing.InOutQuad }
                        NumberAnimation { from: 0.5; to: 1.0; duration: 1000; easing.type: Easing.InOutQuad }
                    }
                }

                // Animated dots
                Row {
                    anchors.horizontalCenter: parent.horizontalCenter
                    spacing: 12
                    visible: (backend.rtspState || 0) !== 3
                    Repeater {
                        model: 3
                        Rectangle {
                            width: 10; height: 10; radius: 5
                            color: "#5af"
                            property int idx: index
                            SequentialAnimation on opacity {
                                running: connectingOverlay.visible
                                loops: Animation.Infinite
                                PauseAnimation { duration: idx * 300 }
                                NumberAnimation { from: 0.2; to: 1.0; duration: 400 }
                                PauseAnimation { duration: 200 }
                                NumberAnimation { from: 1.0; to: 0.2; duration: 400 }
                                PauseAnimation { duration: 600 - idx * 300 }
                            }
                        }
                    }
                }

                // Retry hint on error
                Text {
                    text: "\u041d\u0430\u0436\u043c\u0438\u0442\u0435 \u0434\u043b\u044f \u043f\u043e\u0432\u0442\u043e\u0440\u0430"
                    color: "#888"
                    font.pixelSize: 18
                    anchors.horizontalCenter: parent.horizontalCenter
                    visible: (backend.rtspState || 0) === 3
                }
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    if (backend.useRtsp && backend.rtspUrl.length > 0)
                        backend.reconnectRtsp()
                }
            }
        }

        // Camera 2
        MediaPlayer {
            id: rtspPlayer2; autoPlay: false
            onErrorChanged: { if (error !== MediaPlayer.NoError) backend.onRtsp2Error(errorString) }
            onPlaybackStateChanged: { if (playbackState === MediaPlayer.PlayingState) backend.onRtsp2Playing() }
        }
        VideoOutput { anchors.fill: parent; source: rtspPlayer2; visible: (backend.rtsp2State || 0) === 2 && rtspPlayer2.playbackState === MediaPlayer.PlayingState }
        Rectangle {
            id: connectingOverlay2; anchors.fill: parent; color: "black"; visible: false; z: 1; opacity: 0
            Behavior on opacity { NumberAnimation { duration: 300 } }
            onVisibleChanged: { if (visible) opacity = 1; else opacity = 0 }
            Column { anchors.centerIn: parent; spacing: 16
                Item { width: 80; height: 80; anchors.horizontalCenter: parent.horizontalCenter
                    Rectangle { width: 80; height: 80; radius: 40; color: "transparent"; border.color: (backend.rtsp2State || 0) === 3 ? "#f55" : "#5af"; border.width: 3; anchors.centerIn: parent
                        SequentialAnimation on scale { running: connectingOverlay2.visible; loops: Animation.Infinite
                            NumberAnimation { from: 0.8; to: 1.3; duration: 1200; easing.type: Easing.InOutQuad }
                            NumberAnimation { from: 1.3; to: 0.8; duration: 1200; easing.type: Easing.InOutQuad }
                        }
                    }
                    Image { width: 28; height: 28; anchors.centerIn: parent; source: "data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 24 24' fill='%235af'%3E%3Cpath d='M17 10.5V7c0-.55-.45-1-1-1H4c-.55 0-1 .45-1 1v10c0 .55.45 1 1 1h12c.55 0 1-.45 1-1v-3.5l4 4v-11l-4 4z'/%3E%3C/svg%3E" }
                }
                Text { text: (backend.rtsp2State || 0) === 3 ? backend.rtsp2ErrorMsg : "\u041f\u043e\u0434\u043a\u043b\u044e\u0447\u0435\u043d\u0438\u0435..."; color: "white"; font.pixelSize: 20; anchors.horizontalCenter: parent.horizontalCenter }
            }
            MouseArea { anchors.fill: parent; onClicked: { if (backend.useRtsp2 && backend.rtspUrl2.length > 0) backend.reconnectRtsp2() } }
        }

        // Camera 3
        MediaPlayer {
            id: rtspPlayer3; autoPlay: false
            onErrorChanged: { if (error !== MediaPlayer.NoError) backend.onRtsp3Error(errorString) }
            onPlaybackStateChanged: { if (playbackState === MediaPlayer.PlayingState) backend.onRtsp3Playing() }
        }
        VideoOutput { anchors.fill: parent; source: rtspPlayer3; visible: (backend.rtsp3State || 0) === 2 && rtspPlayer3.playbackState === MediaPlayer.PlayingState }
        Rectangle {
            id: connectingOverlay3; anchors.fill: parent; color: "black"; visible: false; z: 1; opacity: 0
            Behavior on opacity { NumberAnimation { duration: 300 } }
            onVisibleChanged: { if (visible) opacity = 1; else opacity = 0 }
            Column { anchors.centerIn: parent; spacing: 16
                Item { width: 80; height: 80; anchors.horizontalCenter: parent.horizontalCenter
                    Rectangle { width: 80; height: 80; radius: 40; color: "transparent"; border.color: (backend.rtsp3State || 0) === 3 ? "#f55" : "#5af"; border.width: 3; anchors.centerIn: parent
                        SequentialAnimation on scale { running: connectingOverlay3.visible; loops: Animation.Infinite
                            NumberAnimation { from: 0.8; to: 1.3; duration: 1200; easing.type: Easing.InOutQuad }
                            NumberAnimation { from: 1.3; to: 0.8; duration: 1200; easing.type: Easing.InOutQuad }
                        }
                    }
                    Image { width: 28; height: 28; anchors.centerIn: parent; source: "data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 24 24' fill='%235af'%3E%3Cpath d='M17 10.5V7c0-.55-.45-1-1-1H4c-.55 0-1 .45-1 1v10c0 .55.45 1 1 1h12c.55 0 1-.45 1-1v-3.5l4 4v-11l-4 4z'/%3E%3C/svg%3E" }
                }
                Text { text: (backend.rtsp3State || 0) === 3 ? backend.rtsp3ErrorMsg : "\u041f\u043e\u0434\u043a\u043b\u044e\u0447\u0435\u043d\u0438\u0435..."; color: "white"; font.pixelSize: 20; anchors.horizontalCenter: parent.horizontalCenter }
            }
            MouseArea { anchors.fill: parent; onClicked: { if (backend.useRtsp3 && backend.rtspUrl3.length > 0) backend.reconnectRtsp3() } }
        }

    }

    property int settingsTab: 0

    // SETTINGS OVERLAY
    Rectangle {
        anchors.fill: parent
        color: "#ee111111"
        visible: backend.pageIndex === 1
        z: 10

        // Tab bar
        Row {
            id: tabBar
            anchors.top: parent.top; anchors.left: parent.left; anchors.right: parent.right
            height: 56; z: 2
            Repeater {
                model: ["\u0421\u0435\u0442\u044c", "\u0420\u0430\u0441\u043f\u0438\u0441\u0430\u043d\u0438\u0435", "RTSP", "SignalNet"]
                Rectangle {
                    width: tabBar.width / 4; height: 56
                    color: settingsTab === index ? "#333" : "#1a1a1a"
                    border.color: settingsTab === index ? "#5af" : "#333"
                    border.width: settingsTab === index ? 2 : 1
                    Text {
                        text: modelData; color: settingsTab === index ? "#5af" : "#aaa"
                        font.pixelSize: 20; font.bold: settingsTab === index
                        anchors.centerIn: parent
                    }
                    MouseArea { anchors.fill: parent; onClicked: settingsTab = index }
                }
            }
        }

        StackLayout {
            anchors.top: tabBar.bottom; anchors.left: parent.left; anchors.right: parent.right; anchors.bottom: parent.bottom
            currentIndex: settingsTab

            // === TAB 0: Сеть ===
            Flickable {
                contentHeight: netCol.height + 40; clip: true
                Column {
                    id: netCol; width: parent.width - 80; x: 40; y: 20; spacing: 0

                    Text { text: "\u0421\u0435\u0442\u044c"; color: "#aaa"; font.pixelSize: 22; font.bold: true; bottomPadding: 8 }

                Row { spacing: 16; width: parent.width
                    Text { text: "\u0421\u0435\u0440\u0432\u0435\u0440:"; color: "white"; font.pixelSize: 20; width: 160; anchors.verticalCenter: parent.verticalCenter }
                    TextField { id: srvField; text: backend.server; onTextChanged: backend.server = text; width: parent.width - 176; color: "white"; font.pixelSize: 20; padding: 10; background: Rectangle { radius: 8; color: "#222"; border.color: srvField.activeFocus ? "#aaa" : "#555" } }
                }
                Row { spacing: 16; width: parent.width
                    Text { text: "\u041f\u0430\u043f\u043a\u0430:"; color: "white"; font.pixelSize: 20; width: 160; anchors.verticalCenter: parent.verticalCenter }
                    TextField { id: shrField; text: backend.share; onTextChanged: backend.share = text; width: parent.width - 176; color: "white"; font.pixelSize: 20; padding: 10; background: Rectangle { radius: 8; color: "#222"; border.color: shrField.activeFocus ? "#aaa" : "#555" } }
                }
                Row { spacing: 16; width: parent.width
                    Text { text: "\u041b\u043e\u0433\u0438\u043d:"; color: "white"; font.pixelSize: 20; width: 160; anchors.verticalCenter: parent.verticalCenter }
                    TextField { id: usrField; text: backend.user; onTextChanged: backend.user = text; width: parent.width - 176; color: "white"; font.pixelSize: 20; padding: 10; background: Rectangle { radius: 8; color: "#222"; border.color: usrField.activeFocus ? "#aaa" : "#555" } }
                }
                Row { spacing: 16; width: parent.width
                    Text { text: "\u041f\u0430\u0440\u043e\u043b\u044c:"; color: "white"; font.pixelSize: 20; width: 160; anchors.verticalCenter: parent.verticalCenter }
                    TextField { id: pswField; text: backend.pass; onTextChanged: backend.pass = text; echoMode: TextInput.Password; enabled: !guestChk.checked; width: parent.width - 176; color: "white"; font.pixelSize: 20; padding: 10; background: Rectangle { radius: 8; color: guestChk.checked ? "#111" : "#222"; border.color: pswField.activeFocus ? "#aaa" : "#555" } }
                }
                Row { spacing: 12; width: parent.width
                    CheckBox {
                        id: guestChk
                        text: "\u0413\u043e\u0441\u0442\u0435\u0432\u043e\u0439 \u0434\u043e\u0441\u0442\u0443\u043f"
                        checked: backend.useGuest ? true : false
                        onCheckedChanged: backend.useGuest = checked
                        contentItem: Text { text: guestChk.text; color: "white"; font.pixelSize: 20; leftPadding: guestChk.indicator.width + 8; verticalAlignment: Text.AlignVCenter }
                        indicator: Rectangle { width: 24; height: 24; radius: 6; color: guestChk.checked ? "#5a5" : "#222"; border.color: "#888"; anchors.verticalCenter: parent.verticalCenter }
                    }
                }
                Row { spacing: 16; width: parent.width
                    Text { text: "\u0421\u043c\u0431 (\u0432\u0435\u0440\u0441\u0438\u044f):"; color: "white"; font.pixelSize: 20; width: 160; anchors.verticalCenter: parent.verticalCenter }
                    ComboBox {
                        id: smbCombo
                        model: ["1.0", "2.0", "2.1", "3.0", "3.1.1"]
                        currentIndex: model.indexOf(backend.smbVers)
                        onActivated: backend.smbVers = currentText
                        width: 200
                        background: Rectangle { radius: 8; color: "#222"; border.color: "#555" }
                        contentItem: Text { text: smbCombo.displayText; color: "white"; font.pixelSize: 20; verticalAlignment: Text.AlignVCenter; leftPadding: 12 }
                        delegate: ItemDelegate {
                            width: smbCombo.width
                            contentItem: Text { text: modelData; color: "white"; font.pixelSize: 20; verticalAlignment: Text.AlignVCenter; leftPadding: 12 }
                            background: Rectangle { color: index === smbCombo.currentIndex ? "#444" : "#222" }
                        }
                    }
                }
                Row { spacing: 16; width: parent.width
                    Text { text: "\u0418\u043d\u0442\u0435\u0440\u0432\u0430\u043b (\u0441\u0435\u043a):"; color: "white"; font.pixelSize: 20; width: 160; anchors.verticalCenter: parent.verticalCenter }
                    SpinBox {
                        id: intervalSpin
                        from: 2
                        to: 3600
                        value: backend.interval
                        onValueModified: backend.interval = value
                        width: 200
                        background: Rectangle { radius: 8; color: "#222"; border.color: "#555" }
                        contentItem: TextInput {
                            text: intervalSpin.value
                            color: "white"
                            font.pixelSize: 20
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                    }
                }
                Row { spacing: 12; width: parent.width
                    CheckBox {
                        id: shuffleChk
                        text: "\u0421\u043b\u0443\u0447\u0430\u0439\u043d\u044b\u0439 \u043f\u043e\u0440\u044f\u0434\u043e\u043a"
                        checked: backend.shuffle
                        onCheckedChanged: backend.shuffle = checked
                        contentItem: Text { text: shuffleChk.text; color: "white"; font.pixelSize: 20; leftPadding: shuffleChk.indicator.width + 8; verticalAlignment: Text.AlignVCenter }
                        indicator: Rectangle { width: 24; height: 24; radius: 6; color: shuffleChk.checked ? "#5a5" : "#222"; border.color: "#888"; anchors.verticalCenter: parent.verticalCenter }
                    }
                }

                }
            }

            // === TAB 1: Расписание ===
            Flickable {
                contentHeight: schedCol.height + 40; clip: true
                Column {
                    id: schedCol; width: parent.width - 80; x: 40; y: 20; spacing: 0

                    Text { text: "\u0420\u0430\u0441\u043f\u0438\u0441\u0430\u043d\u0438\u0435"; color: "#aaa"; font.pixelSize: 22; font.bold: true; bottomPadding: 8 }

                Row { spacing: 12; width: parent.width
                    CheckBox {
                        id: schedChk
                        text: "\u0412\u043a\u043b\u044e\u0447\u0438\u0442\u044c \u0441\u043f\u044f\u0449\u0438\u0439 \u0440\u0435\u0436\u0438\u043c"
                        checked: backend.useSchedule
                        onCheckedChanged: backend.useSchedule = checked
                        contentItem: Text { text: schedChk.text; color: "white"; font.pixelSize: 20; leftPadding: schedChk.indicator.width + 8; verticalAlignment: Text.AlignVCenter }
                        indicator: Rectangle { width: 24; height: 24; radius: 6; color: schedChk.checked ? "#5a5" : "#222"; border.color: "#888"; anchors.verticalCenter: parent.verticalCenter }
                    }
                }
                Row { spacing: 16; width: parent.width
                    Text { text: "\u041f\u0440\u043e\u0431\u0443\u0436\u0434\u0435\u043d\u0438\u0435:"; color: "white"; font.pixelSize: 20; width: 160; anchors.verticalCenter: parent.verticalCenter }
                    TextField { id: wakeField; text: backend.wakeTime; onEditingFinished: backend.wakeTime = text; width: 120; color: "white"; font.pixelSize: 20; padding: 10; inputMethodHints: Qt.ImhTime; background: Rectangle { radius: 8; color: "#222"; border.color: "#555" } }
                }
                Row { spacing: 16; width: parent.width
                    Text { text: "\u0421\u043d:"; color: "white"; font.pixelSize: 20; width: 160; anchors.verticalCenter: parent.verticalCenter }
                    TextField { id: sleepField; text: backend.sleepTime; onEditingFinished: backend.sleepTime = text; width: 120; color: "white"; font.pixelSize: 20; padding: 10; inputMethodHints: Qt.ImhTime; background: Rectangle { radius: 8; color: "#222"; border.color: "#555" } }
                }

                }
            }

            // === TAB 2: RTSP ===
            Flickable {
                contentHeight: rtspCol.height + 40; clip: true
                Column {
                    id: rtspCol; width: parent.width - 80; x: 40; y: 20; spacing: 0

                    Text { text: "RTSP"; color: "#aaa"; font.pixelSize: 22; font.bold: true; bottomPadding: 8 }

                Row { spacing: 12; width: parent.width
                    CheckBox {
                        id: rtspChk
                        text: "\u041f\u043e\u043a\u0430\u0437\u044b\u0432\u0430\u0442\u044c \u0432\u0438\u0434\u0435\u043e \u0441 RTSP \u043a\u0430\u043c\u0435\u0440\u044b"
                        checked: backend.useRtsp
                        onCheckedChanged: backend.useRtsp = checked
                        contentItem: Text { text: rtspChk.text; color: "white"; font.pixelSize: 20; leftPadding: rtspChk.indicator.width + 8; verticalAlignment: Text.AlignVCenter }
                        indicator: Rectangle { width: 24; height: 24; radius: 6; color: rtspChk.checked ? "#5a5" : "#222"; border.color: "#888"; anchors.verticalCenter: parent.verticalCenter }
                    }
                }
                Row { spacing: 16; width: parent.width
                    Text { text: "URL:"; color: "white"; font.pixelSize: 20; width: 160; anchors.verticalCenter: parent.verticalCenter }
                    TextField { id: rtspField; text: backend.rtspUrl; onTextChanged: backend.rtspUrl = text; placeholderText: "rtsp://user:pass@ip:port/stream"; width: parent.width - 176; color: "white"; font.pixelSize: 20; padding: 10; background: Rectangle { radius: 8; color: "#222"; border.color: rtspField.activeFocus ? "#aaa" : "#555" } }
                }
                Row { spacing: 16; width: parent.width
                    Text { text: "\u0412\u0440\u0435\u043c\u044f \u043f\u043e\u043a\u0430\u0437\u0430 (\u0441\u0435\u043a):"; color: "white"; font.pixelSize: 20; width: 160; anchors.verticalCenter: parent.verticalCenter }
                    SpinBox { id: camDurSpin; from: 1; to: 3600; value: backend.cameraDuration; onValueModified: backend.cameraDuration = value; width: 200
                        background: Rectangle { radius: 8; color: "#222"; border.color: "#555" }
                        contentItem: TextInput { text: camDurSpin.value; color: "white"; font.pixelSize: 20; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                    }
                }

                Rectangle { width: parent.width; height: 1; color: "#333" }
                Text { text: "\u041a\u0430\u043c\u0435\u0440\u0430 2"; color: "#aaa"; font.pixelSize: 20; font.bold: true }

                Row { spacing: 12; width: parent.width
                    CheckBox { id: rtspChk2; text: "\u041f\u043e\u043a\u0430\u0437\u044b\u0432\u0430\u0442\u044c \u0432\u0438\u0434\u0435\u043e \u0441 \u043a\u0430\u043c\u0435\u0440\u044b 2"; checked: backend.useRtsp2; onCheckedChanged: backend.useRtsp2 = checked
                        contentItem: Text { text: rtspChk2.text; color: "white"; font.pixelSize: 20; leftPadding: rtspChk2.indicator.width + 8; verticalAlignment: Text.AlignVCenter }
                        indicator: Rectangle { width: 24; height: 24; radius: 6; color: rtspChk2.checked ? "#5a5" : "#222"; border.color: "#888"; anchors.verticalCenter: parent.verticalCenter }
                    }
                }
                Row { spacing: 16; width: parent.width
                    Text { text: "URL:"; color: "white"; font.pixelSize: 20; width: 160; anchors.verticalCenter: parent.verticalCenter }
                    TextField { id: rtspField2; text: backend.rtspUrl2; onTextChanged: backend.rtspUrl2 = text; placeholderText: "rtsp://..."; width: parent.width - 176; color: "white"; font.pixelSize: 20; padding: 10; background: Rectangle { radius: 8; color: "#222"; border.color: rtspField2.activeFocus ? "#aaa" : "#555" } }
                }
                Row { spacing: 16; width: parent.width
                    Text { text: "\u0412\u0440\u0435\u043c\u044f \u043f\u043e\u043a\u0430\u0437\u0430 (\u0441\u0435\u043a):"; color: "white"; font.pixelSize: 20; width: 160; anchors.verticalCenter: parent.verticalCenter }
                    SpinBox { id: cam2DurSpin; from: 1; to: 3600; value: backend.camera2Duration; onValueModified: backend.camera2Duration = value; width: 200
                        background: Rectangle { radius: 8; color: "#222"; border.color: "#555" }
                        contentItem: TextInput { text: cam2DurSpin.value; color: "white"; font.pixelSize: 20; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                    }
                }

                Rectangle { width: parent.width; height: 1; color: "#333" }
                Text { text: "\u041a\u0430\u043c\u0435\u0440\u0430 3"; color: "#aaa"; font.pixelSize: 20; font.bold: true }

                Row { spacing: 12; width: parent.width
                    CheckBox { id: rtspChk3; text: "\u041f\u043e\u043a\u0430\u0437\u044b\u0432\u0430\u0442\u044c \u0432\u0438\u0434\u0435\u043e \u0441 \u043a\u0430\u043c\u0435\u0440\u044b 3"; checked: backend.useRtsp3; onCheckedChanged: backend.useRtsp3 = checked
                        contentItem: Text { text: rtspChk3.text; color: "white"; font.pixelSize: 20; leftPadding: rtspChk3.indicator.width + 8; verticalAlignment: Text.AlignVCenter }
                        indicator: Rectangle { width: 24; height: 24; radius: 6; color: rtspChk3.checked ? "#5a5" : "#222"; border.color: "#888"; anchors.verticalCenter: parent.verticalCenter }
                    }
                }
                Row { spacing: 16; width: parent.width
                    Text { text: "URL:"; color: "white"; font.pixelSize: 20; width: 160; anchors.verticalCenter: parent.verticalCenter }
                    TextField { id: rtspField3; text: backend.rtspUrl3; onTextChanged: backend.rtspUrl3 = text; placeholderText: "rtsp://..."; width: parent.width - 176; color: "white"; font.pixelSize: 20; padding: 10; background: Rectangle { radius: 8; color: "#222"; border.color: rtspField3.activeFocus ? "#aaa" : "#555" } }
                }
                Row { spacing: 16; width: parent.width
                    Text { text: "\u0412\u0440\u0435\u043c\u044f \u043f\u043e\u043a\u0430\u0437\u0430 (\u0441\u0435\u043a):"; color: "white"; font.pixelSize: 20; width: 160; anchors.verticalCenter: parent.verticalCenter }
                    SpinBox { id: cam3DurSpin; from: 1; to: 3600; value: backend.camera3Duration; onValueModified: backend.camera3Duration = value; width: 200
                        background: Rectangle { radius: 8; color: "#222"; border.color: "#555" }
                        contentItem: TextInput { text: cam3DurSpin.value; color: "white"; font.pixelSize: 20; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                    }
                }

                }
            }

            // === TAB 3: SignalNet ===
            Flickable {
                contentHeight: snCol.height + 40; clip: true
                Column {
                    id: snCol; width: parent.width - 80; x: 40; y: 20; spacing: 0

                    Text { text: "SignalNet"; color: "#aaa"; font.pixelSize: 22; font.bold: true; bottomPadding: 8 }

                Row { spacing: 12; width: parent.width
                    CheckBox {
                        id: snChk
                        text: "\u041f\u043e\u0434\u043a\u043b\u044e\u0447\u0438\u0442\u044c SignalNet"
                        checked: backend.useSignalNet
                        onCheckedChanged: backend.useSignalNet = checked
                        contentItem: Text { text: snChk.text; color: "white"; font.pixelSize: 20; leftPadding: snChk.indicator.width + 8; verticalAlignment: Text.AlignVCenter }
                        indicator: Rectangle { width: 24; height: 24; radius: 6; color: snChk.checked ? "#5a5" : "#222"; border.color: "#888"; anchors.verticalCenter: parent.verticalCenter }
                    }
                }
                Row { spacing: 16; width: parent.width
                    Text { text: "\u0421\u0435\u0440\u0432\u0435\u0440:"; color: "white"; font.pixelSize: 20; width: 160; anchors.verticalCenter: parent.verticalCenter }
                    TextField { id: snSrvField; text: backend.signalNetServer; onTextChanged: backend.signalNetServer = text; placeholderText: "192.168.1.1"; width: parent.width - 176; color: "white"; font.pixelSize: 20; padding: 10; background: Rectangle { radius: 8; color: "#222"; border.color: snSrvField.activeFocus ? "#aaa" : "#555" } }
                }
                Row { spacing: 16; width: parent.width
                    Text { text: "\u041f\u043e\u0440\u0442:"; color: "white"; font.pixelSize: 20; width: 160; anchors.verticalCenter: parent.verticalCenter }
                    TextField { id: snPortField; text: backend.signalNetPort; onTextChanged: backend.signalNetPort = parseInt(text) || 8888; placeholderText: "8888"; width: parent.width - 176; color: "white"; font.pixelSize: 20; padding: 10; background: Rectangle { radius: 8; color: "#222"; border.color: snPortField.activeFocus ? "#aaa" : "#555" } }
                }
                Row { spacing: 16; width: parent.width
                    Text { text: "\u0422\u0440\u0430\u043d\u0441\u043f\u043e\u0440\u0442:"; color: "white"; font.pixelSize: 20; width: 160; anchors.verticalCenter: parent.verticalCenter }
                    ComboBox {
                        id: snTransportCombo
                        model: ["TCP", "UDP"]
                        currentIndex: backend.signalNetUseUdp ? 1 : 0
                        onActivated: backend.signalNetUseUdp = (index === 1)
                        width: 200
                        background: Rectangle { radius: 8; color: "#222"; border.color: "#555" }
                        contentItem: Text { text: snTransportCombo.displayText; color: "white"; font.pixelSize: 20; verticalAlignment: Text.AlignVCenter; leftPadding: 12 }
                        delegate: ItemDelegate {
                            width: snTransportCombo.width
                            contentItem: Text { text: modelData; color: "white"; font.pixelSize: 20; verticalAlignment: Text.AlignVCenter; leftPadding: 12 }
                            background: Rectangle { color: index === snTransportCombo.currentIndex ? "#444" : "#222" }
                        }
                    }
                }
                Row { spacing: 16; width: parent.width; visible: !backend.signalNetUseUdp
                    Text { text: "\u041b\u043e\u0433\u0438\u043d:"; color: "white"; font.pixelSize: 20; width: 160; anchors.verticalCenter: parent.verticalCenter }
                    TextField { id: snLoginField; text: backend.signalNetLogin; onTextChanged: backend.signalNetLogin = text; width: parent.width - 176; color: "white"; font.pixelSize: 20; padding: 10; background: Rectangle { radius: 8; color: "#222"; border.color: snLoginField.activeFocus ? "#aaa" : "#555" } }
                }
                Row { spacing: 16; width: parent.width; visible: !backend.signalNetUseUdp
                    Text { text: "\u041f\u0430\u0440\u043e\u043b\u044c:"; color: "white"; font.pixelSize: 20; width: 160; anchors.verticalCenter: parent.verticalCenter }
                    TextField { id: snPassField; text: backend.signalNetPass; onTextChanged: backend.signalNetPass = text; echoMode: TextInput.Password; width: parent.width - 176; color: "white"; font.pixelSize: 20; padding: 10; background: Rectangle { radius: 8; color: "#222"; border.color: snPassField.activeFocus ? "#aaa" : "#555" } }
                }
                Row { spacing: 16; width: parent.width; visible: backend.signalNetUseUdp
                    Text { text: "\u041b\u043e\u043a. \u043f\u043e\u0440\u0442:"; color: "white"; font.pixelSize: 20; width: 160; anchors.verticalCenter: parent.verticalCenter }
                    TextField { id: snUdpLocalPortField; text: backend.signalNetUdpLocalPort; onTextChanged: backend.signalNetUdpLocalPort = parseInt(text) || 29545; placeholderText: "29545"; width: parent.width - 176; color: "white"; font.pixelSize: 20; padding: 10; background: Rectangle { radius: 8; color: "#222"; border.color: snUdpLocalPortField.activeFocus ? "#aaa" : "#555" } }
                }
                Row { spacing: 16; width: parent.width; visible: backend.signalNetUseUdp
                    Text { text: "\u041a\u043b\u044e\u0447:"; color: "white"; font.pixelSize: 20; width: 160; anchors.verticalCenter: parent.verticalCenter }
                    TextField { id: snUdpKeyField; text: backend.signalNetUdpKey; onTextChanged: backend.signalNetUdpKey = text; placeholderText: "signalnet"; width: parent.width - 176; color: "white"; font.pixelSize: 20; padding: 10; background: Rectangle { radius: 8; color: "#222"; border.color: snUdpKeyField.activeFocus ? "#aaa" : "#555" } }
                }
                Row { spacing: 12; width: parent.width
                    Rectangle { width: 120; height: 40; radius: 8; color: snConnectMa.pressed ? "#555" : (backend.signalNetConnected ? "#5a5" : "#222"); border.color: "#888"; border.width: 1
                        Text { text: backend.signalNetConnected ? "\u041e\u0442\u043a\u043b\u044e\u0447\u0438\u0442\u044c" : "\u041f\u043e\u0434\u043a\u043b\u044e\u0447\u0438\u0442\u044c"; color: "white"; font.pixelSize: 16; anchors.centerIn: parent }
                        MouseArea { id: snConnectMa; anchors.fill: parent; onClicked: {
                            if (backend.signalNetConnected) backend.disconnectSignalNet()
                            else backend.connectSignalNet()
                        } }
                    }
                    Text { text: backend.signalNetConnected ? "\u041f\u043e\u0434\u043a\u043b\u044e\u0447\u0435\u043d\u043e" : "\u041e\u0442\u043a\u043b\u044e\u0447\u0435\u043d\u043e"; color: backend.signalNetConnected ? "#5a5" : "#f55"; font.pixelSize: 16; anchors.verticalCenter: parent.verticalCenter }
                }

                Text { text: "\u041a\u0430\u043c\u0435\u0440\u0430"; color: "#aaa"; font.pixelSize: 22; font.bold: true; bottomPadding: 8 }
                Row { spacing: 16; width: parent.width
                    Text { text: "\u0412\u0440\u0435\u043c\u044f \u043f\u043e\u043a\u0430\u0437\u0430 (\u0441\u0435\u043a):"; color: "white"; font.pixelSize: 20; width: 200; anchors.verticalCenter: parent.verticalCenter }
                    TextField { id: camDurField
                        text: backend.cameraDuration
                        onTextChanged: { var v = parseInt(text); if (v > 0) backend.cameraDuration = v }
                        placeholderText: "30"
                        width: 100; color: "white"; font.pixelSize: 20; padding: 10
                        validator: IntValidator { bottom: 1; top: 3600 }
                        background: Rectangle { radius: 8; color: "#222"; border.color: camDurField.activeFocus ? "#aaa" : "#555" }
                    }
                }
            }
        }
        } // StackLayout

        // Bottom buttons (always visible)
        Row { spacing: 20; anchors.bottom: parent.bottom; anchors.bottomMargin: 20; anchors.horizontalCenter: parent.horizontalCenter; z: 2
            Rectangle { width: 160; height: 56; radius: 14; color: exitMa.pressed ? "#666" : exitMa.containsMouse ? "#444" : "transparent"; border.color: "white"; border.width: 2
                Text { text: "\u0412\u044b\u0445\u043e\u0434"; color: "white"; font.pixelSize: 22; font.bold: true; anchors.centerIn: parent }
                MouseArea { id: exitMa; anchors.fill: parent; hoverEnabled: true; onClicked: backend.pageIndex = 0 }
            }
            Rectangle { width: 160; height: 56; radius: 14; color: saveMa.pressed ? "#666" : saveMa.containsMouse ? "#444" : "transparent"; border.color: "white"; border.width: 2
                Text { text: "\u0421\u043e\u0445\u0440\u0430\u043d\u0438\u0442\u044c"; color: "white"; font.pixelSize: 22; font.bold: true; anchors.centerIn: parent }
                MouseArea { id: saveMa; anchors.fill: parent; hoverEnabled: true; onClicked: backend.saveSettings() }
            }
        }
    }

    // TASKS OVERLAY
    Rectangle {
        anchors.fill: parent
        color: "#ee111111"
        visible: backend.pageIndex === 2
        z: 10
        ColumnLayout { anchors.fill: parent; anchors.margins: 50; spacing: 20
            Text { text: "\u0422\u0435\u043a\u0443\u0449\u0438\u0435 \u0437\u0430\u0434\u0430\u0447\u0438"; color: "white"; font.pixelSize: 36; font.bold: true }
            ListView { Layout.fillWidth: true; Layout.fillHeight: true; model: backend.tasks(); clip: true
                delegate: Rectangle { width: ListView.view.width; height: 50; color: index % 2 === 0 ? "#222" : "#2a2a2a"; border.color: "#333"; border.width: 1
                    Text { text: modelData; color: "white"; font.pixelSize: 20; anchors.verticalCenter: parent.verticalCenter; leftPadding: 16 }
                }
                Text { anchors.centerIn: parent; text: backend.tasks().length === 0 ? "\u041d\u0435\u0442 \u0437\u0430\u0434\u0430\u0447" : ""; color: "#666"; font.pixelSize: 24 }
            }
        }
    }

    // CALENDAR OVERLAY
    Rectangle {
        id: calOverlay
        anchors.fill: parent
        color: "#ee111111"
        visible: backend.pageIndex === 3
        z: 10

        property var today: new Date()
        property int dy: today.getFullYear()
        property int dm: today.getMonth()
        readonly property int dim: new Date(dy, dm + 1, 0).getDate()
        readonly property int fdow: new Date(dy, dm, 1).getDay()

        function mn(m) { return ["\u042f\u043d\u0432\u0430\u0440\u044c","\u0424\u0435\u0432\u0440\u0430\u043b\u044c","\u041c\u0430\u0440\u0442","\u0410\u043f\u0440\u0435\u043b\u044c","\u041c\u0430\u0439","\u0418\u044e\u043d\u044c","\u0418\u044e\u043b\u044c","\u0410\u0432\u0433\u0443\u0441\u0442","\u0421\u0435\u043d\u0442\u044f\u0431\u0440\u044c","\u041e\u043a\u0442\u044f\u0431\u0440\u044c","\u041d\u043e\u044f\u0431\u0440\u044c","\u0414\u0435\u043a\u0430\u0431\u0440\u044c"][m] }
        function dn(d) { return ["\u0412\u0441","\u041f\u043d","\u0412\u0442","\u0421\u0440","\u0427\u0442","\u041f\u0442","\u0421\u0431"][d] }

        ColumnLayout { anchors.fill: parent; anchors.margins: 50; spacing: 20
            Text { text: "\u041a\u0430\u043b\u0435\u043d\u0434\u0430\u0440\u044c"; color: "white"; font.pixelSize: 36; font.bold: true }
            RowLayout { width: parent.width; spacing: 20
                Rectangle { width: 56; height: 48; radius: 14; color: pma.pressed ? "#444" : "#222"; border.color: "#888"; border.width: 1
                    Text { text: "\u25c0"; color: "white"; font.pixelSize: 20; anchors.centerIn: parent }
                    MouseArea { id: pma; anchors.fill: parent; onClicked: { if (calOverlay.dm === 0) { calOverlay.dm = 11; calOverlay.dy-- } else calOverlay.dm-- } }
                }
                Text { text: calOverlay.mn(calOverlay.dm) + " " + calOverlay.dy; color: "white"; font.pixelSize: 24; font.bold: true; Layout.fillWidth: true; horizontalAlignment: Text.AlignHCenter }
                Rectangle { width: 56; height: 48; radius: 14; color: nma.pressed ? "#444" : "#222"; border.color: "#888"; border.width: 1
                    Text { text: "\u25b6"; color: "white"; font.pixelSize: 20; anchors.centerIn: parent }
                    MouseArea { id: nma; anchors.fill: parent; onClicked: { if (calOverlay.dm === 11) { calOverlay.dm = 0; calOverlay.dy++ } else calOverlay.dm++ } }
                }
            }
            GridLayout { Layout.fillWidth: true; Layout.fillHeight: true; columns: 7; columnSpacing: 2; rowSpacing: 2
                Repeater { model: 7; Rectangle { Layout.fillWidth: true; Layout.preferredHeight: 36; color: "#222"
                    Text { text: calOverlay.dn(index); color: "#aaa"; font.pixelSize: 16; font.bold: true; anchors.centerIn: parent } } }
                Repeater { model: calOverlay.fdow; Item { Layout.fillWidth: true; Layout.preferredHeight: 1 } }
                Repeater { model: calOverlay.dim
                    Rectangle { property int dayNum: index + 1; Layout.fillWidth: true; Layout.preferredHeight: 48
                        color: (calOverlay.today.getDate() === dayNum && calOverlay.today.getMonth() === calOverlay.dm && calOverlay.today.getFullYear() === calOverlay.dy) ? "#5af" : "#1a1a1a"
                        border.color: "#333"; border.width: 1; radius: 4
                        Text { text: dayNum; color: "white"; font.pixelSize: 18; anchors.centerIn: parent } } }
            }
        }
    }

    // CONTROL BAR
    Rectangle {
        anchors.left: parent.left; anchors.right: parent.right; anchors.bottom: parent.bottom
        height: 100; color: "transparent"; visible: !sleepOverlay.visible && backend.pageIndex !== 1; z: 20
        Row { anchors.centerIn: parent; anchors.bottom: parent.bottom; anchors.bottomMargin: 40; spacing: 16
            Rectangle { width: 84; height: 56; radius: 14; color: "#4d000000"; border.color: "white"; border.width: 1
                Text { text: backend.currentTime; color: "white"; font.pixelSize: 24; font.bold: true; font.family: "Consolas, monospace"; anchors.centerIn: parent } }
            Rectangle { width: 140; height: 56; radius: 14; color: backend.pageIndex === 3 ? "#4dffffff" : (dma.pressed ? "#555" : "#4d000000"); border.color: "white"; border.width: 1
                Text { text: backend.currentDate; color: "white"; font.pixelSize: 22; font.bold: true; font.family: "Consolas, monospace"; anchors.fill: parent; leftPadding: 10; rightPadding: 10; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                MouseArea { id: dma; anchors.fill: parent; onClicked: backend.pageIndex = backend.pageIndex === 3 ? 0 : 3 } }
            Rectangle { width: 56; height: 56; radius: 14; color: (backend.rtspState || 0) === 2 ? "#4dffffff" : (vma.pressed ? "#555" : "#4d000000"); border.color: "white"; border.width: 1
                Image { width: 28; height: 28; anchors.centerIn: parent; source: "data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 24 24' fill='white'%3E%3Cpath d='M17 10.5V7c0-.55-.45-1-1-1H4c-.55 0-1 .45-1 1v10c0 .55.45 1 1 1h12c.55 0 1-.45 1-1v-3.5l4 4v-11l-4 4z'/%3E%3C/svg%3E" }
                MouseArea { id: vma; anchors.fill: parent; onClicked: {
                    if ((backend.rtspState || 0) !== 0) { backend.stopRtsp() }
                    else { backend.pageIndex = 0; if (backend.useRtsp && backend.rtspUrl.length > 0) backend.reconnectRtsp() }
                } } }
            Rectangle { visible: backend.useRtsp2; width: 56; height: 56; radius: 14; color: (backend.rtsp2State || 0) === 2 ? "#4dffffff" : (v2ma.pressed ? "#555" : "#4d000000"); border.color: "white"; border.width: 1
                Image { width: 28; height: 28; anchors.centerIn: parent; source: "data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 24 24' fill='white'%3E%3Cpath d='M17 10.5V7c0-.55-.45-1-1-1H4c-.55 0-1 .45-1 1v10c0 .55.45 1 1 1h12c.55 0 1-.45 1-1v-3.5l4 4v-11l-4 4z'/%3E%3C/svg%3E" }
                Text { text: "2"; color: "#5af"; font.pixelSize: 10; font.bold: true; anchors.right: parent.right; anchors.top: parent.top; anchors.rightMargin: 2; anchors.topMargin: 1 }
                MouseArea { id: v2ma; anchors.fill: parent; onClicked: {
                    if ((backend.rtsp2State || 0) !== 0) { backend.stopRtsp2() }
                    else { backend.pageIndex = 0; if (backend.useRtsp2 && backend.rtspUrl2.length > 0) backend.reconnectRtsp2() }
                } } }
            Rectangle { visible: backend.useRtsp3; width: 56; height: 56; radius: 14; color: (backend.rtsp3State || 0) === 2 ? "#4dffffff" : (v3ma.pressed ? "#555" : "#4d000000"); border.color: "white"; border.width: 1
                Image { width: 28; height: 28; anchors.centerIn: parent; source: "data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 24 24' fill='white'%3E%3Cpath d='M17 10.5V7c0-.55-.45-1-1-1H4c-.55 0-1 .45-1 1v10c0 .55.45 1 1 1h12c.55 0 1-.45 1-1v-3.5l4 4v-11l-4 4z'/%3E%3C/svg%3E" }
                Text { text: "3"; color: "#5af"; font.pixelSize: 10; font.bold: true; anchors.right: parent.right; anchors.top: parent.top; anchors.rightMargin: 2; anchors.topMargin: 1 }
                MouseArea { id: v3ma; anchors.fill: parent; onClicked: {
                    if ((backend.rtsp3State || 0) !== 0) { backend.stopRtsp3() }
                    else { backend.pageIndex = 0; if (backend.useRtsp3 && backend.rtspUrl3.length > 0) backend.reconnectRtsp3() }
                } } }
            Rectangle { visible: backend.signalNetConnected; width: 56; height: 56; radius: 14; color: a1ma.pressed ? "#555" : "#4d000000"; border.color: "#fa0"; border.width: 1
                Text { text: "A1"; color: "#fa0"; font.pixelSize: 18; font.bold: true; anchors.centerIn: parent }
                MouseArea { id: a1ma; anchors.fill: parent; onClicked: backend.sendAction1() } }
            Rectangle { visible: backend.signalNetConnected; width: 56; height: 56; radius: 14; color: a2ma.pressed ? "#555" : "#4d000000"; border.color: "#fa0"; border.width: 1
                Text { text: "A2"; color: "#fa0"; font.pixelSize: 18; font.bold: true; anchors.centerIn: parent }
                MouseArea { id: a2ma; anchors.fill: parent; onClicked: backend.sendAction2() } }
            Rectangle { width: 56; height: 56; radius: 14; color: backend.pageIndex === 1 ? "#4dffffff" : (sma.pressed ? "#555" : "#4d000000"); border.color: "white"; border.width: 1
                Text { text: "\u2261"; color: "white"; font.pixelSize: 28; font.bold: true; anchors.centerIn: parent }
                MouseArea { id: sma; anchors.fill: parent; onClicked: backend.pageIndex = backend.pageIndex === 1 ? 0 : 1 } }
            Rectangle { width: 56; height: 56; radius: 14; color: backend.pageIndex === 2 ? "#4dffffff" : (tma.pressed ? "#555" : "#4d000000"); border.color: "white"; border.width: 1
                Text { text: "\u2713"; color: "white"; font.pixelSize: 24; font.bold: true; anchors.centerIn: parent }
                MouseArea { id: tma; anchors.fill: parent; onClicked: backend.pageIndex = backend.pageIndex === 2 ? 0 : 2 } }
        }
    }

    Rectangle {
        id: loadingOverlay
        anchors.centerIn: parent
        width: 300
        height: 160
        radius: 20
        color: "#aa111111"
        border.color: "#555"
        border.width: 1
        visible: loading && (backend.rtspState || 0) !== 2 && backend.pageIndex === 0
        z: 5
        Column {
            anchors.centerIn: parent
            spacing: 16
            Text {
                text: "\u23f3"
                color: "#aaa"
                font.pixelSize: 48
                anchors.horizontalCenter: parent.horizontalCenter
            }
            Text {
                text: loadingText
                color: "white"
                font.pixelSize: 20
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }
    }

    Rectangle { id: sleepOverlay; anchors.fill: parent; color: "black"; visible: false; z: 50 }

    // Indoor temperature — top right
    Text {
        visible: backend.useSignalNet && backend.signalNetConnected && backend.signalNetTemperatureValid
        anchors.top: parent.top; anchors.right: parent.right
        anchors.topMargin: 20; anchors.rightMargin: 24
        z: 20
        property bool tempWarn: backend.signalNetTemperature < 18 || backend.signalNetTemperature > 24
        text: (tempWarn ? "\u26a0 " : "") + backend.signalNetTemperature.toFixed(1) + "\u00b0C"
        color: tempWarn ? "#f55" : "white"
        font.pixelSize: 48; font.bold: true
        font.family: "Consolas, monospace"
        style: Text.Outline; styleColor: "#44000000"
    }

    // Outdoor temperature
    Text {
        visible: backend.useSignalNet && backend.signalNetConnected && backend.signalNetTemperatureOutValid
        anchors.top: parent.top; anchors.right: parent.right
        anchors.topMargin: 80; anchors.rightMargin: 24
        z: 20
        text: backend.signalNetTemperatureOut.toFixed(1) + "\u00b0C"
        color: "#aaf"
        font.pixelSize: 64; font.bold: true
        font.family: "Consolas, monospace"
        style: Text.Outline; styleColor: "#44000000"
    }

    // Sensor indicators
    Column {
        visible: backend.useSignalNet && backend.signalNetConnected
        anchors.top: parent.top; anchors.right: parent.right
        anchors.topMargin: 160; anchors.rightMargin: 24
        z: 20; spacing: 6
        Text {
            visible: backend.signalNetHumidityValid
            property bool warn: backend.signalNetHumidity < 30 || backend.signalNetHumidity > 60
            text: (warn ? "\u26a0 " : "") + backend.signalNetHumidity.toFixed(0) + "%"
            color: warn ? "#f55" : "#5cf"
            font.pixelSize: 36; font.bold: true; font.family: "Consolas, monospace"
            style: Text.Outline; styleColor: "#44000000"
            horizontalAlignment: Text.AlignRight; width: parent.width
        }
        Text {
            visible: backend.signalNetCo2Valid
            property bool warn: backend.signalNetCo2 >= 1000
            text: (warn ? "\u26a0 " : "") + backend.signalNetCo2 + " ppm"
            color: warn ? "#f55" : "#fa3"
            font.pixelSize: 36; font.bold: true; font.family: "Consolas, monospace"
            style: Text.Outline; styleColor: "#44000000"
            horizontalAlignment: Text.AlignRight; width: parent.width
        }
        Text {
            visible: backend.signalNetDustValid
            property bool warn: backend.signalNetDust >= 25
            text: (warn ? "\u26a0 " : "") + backend.signalNetDust + " \u03bcg/m\u00b3"
            color: warn ? "#f55" : "#f77"
            font.pixelSize: 36; font.bold: true; font.family: "Consolas, monospace"
            style: Text.Outline; styleColor: "#44000000"
            horizontalAlignment: Text.AlignRight; width: parent.width
        }
        Text {
            visible: backend.signalNetVarValid
            text: backend.signalNetVar.toFixed(1)
            color: "#aaa"
            font.pixelSize: 36; font.bold: true; font.family: "Consolas, monospace"
            style: Text.Outline; styleColor: "#44000000"
            horizontalAlignment: Text.AlignRight; width: parent.width
        }
    }

    // SignalNet Alert Banner
    Rectangle {
        id: snAlertBanner
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top; anchors.topMargin: 20
        width: Math.min(parent.width - 60, 500); height: 72; radius: 36
        visible: false; z: 55
        property int alertSeverity: backend.signalNetAlertSeverity
        property color accentColor: alertSeverity >= 2 ? "#e53935" : (alertSeverity === 1 ? "#fb8c00" : "#42a5f5")
        color: Qt.rgba(0.1, 0.1, 0.1, 0.92)
        border.color: accentColor; border.width: 2
        opacity: 0; y: -80
        onVisibleChanged: { if (visible) { slideIn.start(); autoHide.restart() } }
        NumberAnimation on y { id: slideIn; from: -80; to: 20; duration: 300; easing.type: Easing.OutCubic
            onRunningChanged: { if (!running && snAlertBanner.visible) snAlertBanner.opacity = 1.0 } }
        NumberAnimation on y { id: slideOut; from: 20; to: -80; duration: 250; easing.type: Easing.InCubic
            onRunningChanged: { if (!running) snAlertBanner.visible = false } }
        NumberAnimation on opacity { id: fadeIn; from: 0; to: 1; duration: 300 }
        NumberAnimation on opacity { id: fadeOut; from: 1; to: 0; duration: 250 }
        Row {
            anchors.centerIn: parent; spacing: 14
            Rectangle {
                width: 40; height: 40; radius: 20
                color: snAlertBanner.accentColor; opacity: 0.9
                anchors.verticalCenter: parent.verticalCenter
                SequentialAnimation on opacity {
                    running: snAlertBanner.visible && snAlertBanner.alertSeverity > 0
                    loops: Animation.Infinite
                    NumberAnimation { from: 0.9; to: 0.4; duration: 600; easing.type: Easing.InOutQuad }
                    NumberAnimation { from: 0.4; to: 0.9; duration: 600; easing.type: Easing.InOutQuad }
                }
                Text { text: snAlertBanner.alertSeverity >= 2 ? "\u26a0" : (snAlertBanner.alertSeverity === 1 ? "\u26a0" : "\u2139"); color: "white"; font.pixelSize: 22; anchors.centerIn: parent }
            }
            Text { text: backend.signalNetAlert; color: "white"; font.pixelSize: 22; font.bold: true; anchors.verticalCenter: parent.verticalCenter }
            Text { text: "\u00d7"; color: "#aaa"; font.pixelSize: 24; anchors.verticalCenter: parent.verticalCenter
                MouseArea { anchors.fill: parent; onClicked: { fadeOut.start(); slideOut.start(); backend.clearSignalNetAlert() } }
            }
        }
        MouseArea { anchors.fill: parent; onClicked: { fadeOut.start(); slideOut.start(); backend.clearSignalNetAlert() } }
        Timer { id: autoHide; interval: 8000; running: snAlertBanner.visible
            onTriggered: { fadeOut.start(); slideOut.start(); backend.clearSignalNetAlert() } }
    }

    MouseArea { anchors.fill: parent; z: 1; visible: backend.pageIndex === 0 && (backend.rtspState || 0) !== 2; onClicked: backend.nextSlide(); propagateComposedEvents: true }

    Item { anchors.fill: parent; focus: true; z: -1
        Keys.onPressed: {
            if (event.key === Qt.Key_Right) backend.nextSlide()
            else if (event.key === Qt.Key_Left) backend.prevSlide()
            else if (event.key === Qt.Key_Space) backend.toggleSlideshow()
            else if (event.key === Qt.Key_S || event.key === Qt.Key_Escape) backend.pageIndex = backend.pageIndex === 1 ? 0 : 1
            else if (event.key === Qt.Key_V) {
                if ((backend.rtspState || 0) !== 0) backend.stopRtsp()
                else { backend.pageIndex = 0; if (backend.useRtsp && backend.rtspUrl.length > 0) backend.reconnectRtsp() }
            }
            else if (event.key === Qt.Key_T) backend.pageIndex = backend.pageIndex === 2 ? 0 : 2
            else if (event.key === Qt.Key_C) backend.pageIndex = backend.pageIndex === 3 ? 0 : 3
            else if (event.key === Qt.Key_Q) Qt.quit()
        }
    }
}
