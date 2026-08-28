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
                Row { spacing: 16; width: parent.width; topPadding: 8
                    Text { text: "\u041f\u0430\u043f\u043a\u0430:"; color: "white"; font.pixelSize: 20; width: 160; anchors.verticalCenter: parent.verticalCenter }
                    TextField { id: shrField; text: backend.share; onTextChanged: backend.share = text; width: parent.width - 176; color: "white"; font.pixelSize: 20; padding: 10; background: Rectangle { radius: 8; color: "#222"; border.color: shrField.activeFocus ? "#aaa" : "#555" } }
                }
                Row { spacing: 16; width: parent.width; topPadding: 8
                    Text { text: "\u041b\u043e\u0433\u0438\u043d:"; color: "white"; font.pixelSize: 20; width: 160; anchors.verticalCenter: parent.verticalCenter }
                    TextField { id: usrField; text: backend.user; onTextChanged: backend.user = text; width: parent.width - 176; color: "white"; font.pixelSize: 20; padding: 10; background: Rectangle { radius: 8; color: "#222"; border.color: usrField.activeFocus ? "#aaa" : "#555" } }
                }
                Row { spacing: 16; width: parent.width; topPadding: 8
                    Text { text: "\u041f\u0430\u0440\u043e\u043b\u044c:"; color: "white"; font.pixelSize: 20; width: 160; anchors.verticalCenter: parent.verticalCenter }
                    TextField { id: pswField; text: backend.pass; onTextChanged: backend.pass = text; echoMode: TextInput.Password; enabled: !guestChk.checked; width: parent.width - 176; color: "white"; font.pixelSize: 20; padding: 10; background: Rectangle { radius: 8; color: guestChk.checked ? "#111" : "#222"; border.color: pswField.activeFocus ? "#aaa" : "#555" } }
                }
                Row { spacing: 12; width: parent.width; topPadding: 8
                    CheckBox {
                        id: guestChk
                        text: "\u0413\u043e\u0441\u0442\u0435\u0432\u043e\u0439 \u0434\u043e\u0441\u0442\u0443\u043f"
                        checked: backend.useGuest ? true : false
                        onCheckedChanged: backend.useGuest = checked
                        contentItem: Text { text: guestChk.text; color: "white"; font.pixelSize: 20; leftPadding: guestChk.indicator.width + 8; verticalAlignment: Text.AlignVCenter }
                        indicator: Rectangle { width: 24; height: 24; radius: 6; color: guestChk.checked ? "#5a5" : "#222"; border.color: "#888"; anchors.verticalCenter: parent.verticalCenter }
                    }
                }
                Row { spacing: 16; width: parent.width; topPadding: 8
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
                Row { spacing: 16; width: parent.width; topPadding: 8
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
                Row { spacing: 12; width: parent.width; topPadding: 8
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
                Row { spacing: 16; width: parent.width; topPadding: 8
                    Text { text: "\u041f\u0440\u043e\u0431\u0443\u0436\u0434\u0435\u043d\u0438\u0435:"; color: "white"; font.pixelSize: 20; width: 160; anchors.verticalCenter: parent.verticalCenter }
                    TextField { id: wakeField; text: backend.wakeTime; onEditingFinished: backend.wakeTime = text; width: 120; color: "white"; font.pixelSize: 20; padding: 10; inputMethodHints: Qt.ImhTime; background: Rectangle { radius: 8; color: "#222"; border.color: "#555" } }
                }
                Row { spacing: 16; width: parent.width; topPadding: 8
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
                Row { spacing: 16; width: parent.width; topPadding: 8
                    Text { text: "URL:"; color: "white"; font.pixelSize: 20; width: 160; anchors.verticalCenter: parent.verticalCenter }
                    TextField { id: rtspField; text: backend.rtspUrl; onTextChanged: backend.rtspUrl = text; placeholderText: "rtsp://user:pass@ip:port/stream"; width: parent.width - 176; color: "white"; font.pixelSize: 20; padding: 10; background: Rectangle { radius: 8; color: "#222"; border.color: rtspField.activeFocus ? "#aaa" : "#555" } }
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
                Row { spacing: 16; width: parent.width; topPadding: 8
                    Text { text: "\u0421\u0435\u0440\u0432\u0435\u0440:"; color: "white"; font.pixelSize: 20; width: 160; anchors.verticalCenter: parent.verticalCenter }
                    TextField { id: snSrvField; text: backend.signalNetServer; onTextChanged: backend.signalNetServer = text; placeholderText: "192.168.1.1"; width: parent.width - 176; color: "white"; font.pixelSize: 20; padding: 10; background: Rectangle { radius: 8; color: "#222"; border.color: snSrvField.activeFocus ? "#aaa" : "#555" } }
                }
                Row { spacing: 16; width: parent.width; topPadding: 8
                    Text { text: "\u041f\u043e\u0440\u0442:"; color: "white"; font.pixelSize: 20; width: 160; anchors.verticalCenter: parent.verticalCenter }
                    TextField { id: snPortField; text: backend.signalNetPort; onTextChanged: backend.signalNetPort = parseInt(text) || 8888; placeholderText: "8888"; width: parent.width - 176; color: "white"; font.pixelSize: 20; padding: 10; background: Rectangle { radius: 8; color: "#222"; border.color: snPortField.activeFocus ? "#aaa" : "#555" } }
                }
                Row { spacing: 16; width: parent.width; topPadding: 8
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
                Row { spacing: 16; width: parent.width; topPadding: 8; visible: !backend.signalNetUseUdp
                    Text { text: "\u041b\u043e\u0433\u0438\u043d:"; color: "white"; font.pixelSize: 20; width: 160; anchors.verticalCenter: parent.verticalCenter }
                    TextField { id: snLoginField; text: backend.signalNetLogin; onTextChanged: backend.signalNetLogin = text; width: parent.width - 176; color: "white"; font.pixelSize: 20; padding: 10; background: Rectangle { radius: 8; color: "#222"; border.color: snLoginField.activeFocus ? "#aaa" : "#555" } }
                }
                Row { spacing: 16; width: parent.width; topPadding: 8; visible: !backend.signalNetUseUdp
                    Text { text: "\u041f\u0430\u0440\u043e\u043b\u044c:"; color: "white"; font.pixelSize: 20; width: 160; anchors.verticalCenter: parent.verticalCenter }
                    TextField { id: snPassField; text: backend.signalNetPass; onTextChanged: backend.signalNetPass = text; echoMode: TextInput.Password; width: parent.width - 176; color: "white"; font.pixelSize: 20; padding: 10; background: Rectangle { radius: 8; color: "#222"; border.color: snPassField.activeFocus ? "#aaa" : "#555" } }
                }
                Row { spacing: 16; width: parent.width; topPadding: 8; visible: backend.signalNetUseUdp
                    Text { text: "\u041b\u043e\u043a. \u043f\u043e\u0440\u0442:"; color: "white"; font.pixelSize: 20; width: 160; anchors.verticalCenter: parent.verticalCenter }
                    TextField { id: snUdpLocalPortField; text: backend.signalNetUdpLocalPort; onTextChanged: backend.signalNetUdpLocalPort = parseInt(text) || 29545; placeholderText: "29545"; width: parent.width - 176; color: "white"; font.pixelSize: 20; padding: 10; background: Rectangle { radius: 8; color: "#222"; border.color: snUdpLocalPortField.activeFocus ? "#aaa" : "#555" } }
                }
                Row { spacing: 16; width: parent.width; topPadding: 8; visible: backend.signalNetUseUdp
                    Text { text: "\u041a\u043b\u044e\u0447:"; color: "white"; font.pixelSize: 20; width: 160; anchors.verticalCenter: parent.verticalCenter }
                    TextField { id: snUdpKeyField; text: backend.signalNetUdpKey; onTextChanged: backend.signalNetUdpKey = text; placeholderText: "signalnet"; width: parent.width - 176; color: "white"; font.pixelSize: 20; padding: 10; background: Rectangle { radius: 8; color: "#222"; border.color: snUdpKeyField.activeFocus ? "#aaa" : "#555" } }
                }
                Row { spacing: 12; width: parent.width; topPadding: 8
                    Rectangle { width: 120; height: 40; radius: 8; color: snConnectMa.pressed ? "#555" : (backend.signalNetConnected ? "#5a5" : "#222"); border.color: "#888"; border.width: 1
                        Text { text: backend.signalNetConnected ? "\u041e\u0442\u043a\u043b\u044e\u0447\u0438\u0442\u044c" : "\u041f\u043e\u0434\u043a\u043b\u044e\u0447\u0438\u0442\u044c"; color: "white"; font.pixelSize: 16; anchors.centerIn: parent }
                        MouseArea { id: snConnectMa; anchors.fill: parent; onClicked: {
                            if (backend.signalNetConnected) backend.disconnectSignalNet()
                            else backend.connectSignalNet()
                        } }
                    }
                    Text { text: backend.signalNetConnected ? "\u041f\u043e\u0434\u043a\u043b\u044e\u0447\u0435\u043d\u043e" : "\u041e\u0442\u043a\u043b\u044e\u0447\u0435\u043d\u043e"; color: backend.signalNetConnected ? "#5a5" : "#f55"; font.pixelSize: 16; anchors.verticalCenter: parent.verticalCenter }
                }

                Text { text: "\u041a\u0430\u043c\u0435\u0440\u0430"; color: "#aaa"; font.pixelSize: 22; font.bold: true; topPadding: 20; bottomPadding: 8 }
                Row { spacing: 16; width: parent.width; topPadding: 8
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
                Text { text: "\u25b6"; color: "white"; font.pixelSize: 24; anchors.centerIn: parent }
                MouseArea { id: vma; anchors.fill: parent; onClicked: {
                    if ((backend.rtspState || 0) !== 0) { backend.stopRtsp() }
                    else { backend.pageIndex = 0; if (backend.useRtsp && backend.rtspUrl.length > 0) backend.reconnectRtsp() }
                } } }
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

    // Temperature — top right corner
    Text {
        visible: backend.useSignalNet && backend.signalNetConnected && backend.signalNetTemperatureValid
        anchors.top: parent.top; anchors.right: parent.right
        anchors.topMargin: 20; anchors.rightMargin: 24
        z: 20
        text: backend.signalNetTemperature.toFixed(1) + "\u00b0C"
        color: "white"
        font.pixelSize: 48
        font.bold: true
        font.family: "Consolas, monospace"
        style: Text.Outline
        styleColor: "#44000000"
    }

    // SignalNet Alert Overlay
    Rectangle {
        id: snAlertOverlay
        anchors.fill: parent
        visible: backend.signalNetAlert.length > 0
        z: 55
        property int alertSeverity: backend.signalNetAlertSeverity
        property color bgColor: alertSeverity >= 2 ? "#ee000000" : (alertSeverity === 1 ? "#ee330000" : "#ee000033")
        color: bgColor

        // Flashing border
        Rectangle {
            anchors.fill: parent
            color: "transparent"
            border.color: snAlertOverlay.alertSeverity >= 2 ? "#f00" : (snAlertOverlay.alertSeverity === 1 ? "#fa0" : "#5af")
            border.width: 4
            SequentialAnimation on border.width {
                running: snAlertOverlay.visible
                loops: Animation.Infinite
                NumberAnimation { from: 2; to: 8; duration: 400; easing.type: Easing.InOutQuad }
                NumberAnimation { from: 8; to: 2; duration: 400; easing.type: Easing.InOutQuad }
            }
        }

        Column {
            anchors.centerIn: parent
            spacing: 20
            // Pulsing icon
            Text {
                id: alertIcon
                text: snAlertOverlay.alertSeverity >= 2 ? "\u26a0" : (snAlertOverlay.alertSeverity === 1 ? "\u26a0" : "\u2139")
                color: snAlertOverlay.alertSeverity >= 2 ? "#f00" : (snAlertOverlay.alertSeverity === 1 ? "#fa0" : "#5af")
                font.pixelSize: 80
                anchors.horizontalCenter: parent.horizontalCenter
                SequentialAnimation on opacity {
                    running: snAlertOverlay.visible
                    loops: Animation.Infinite
                    NumberAnimation { from: 1.0; to: 0.3; duration: 500; easing.type: Easing.InOutQuad }
                    NumberAnimation { from: 0.3; to: 1.0; duration: 500; easing.type: Easing.InOutQuad }
                }
            }
            Text {
                text: backend.signalNetAlert
                color: "white"
                font.pixelSize: 32
                font.bold: true
                anchors.horizontalCenter: parent.horizontalCenter
            }
            Text {
                text: "SignalNet"
                color: "#aaa"
                font.pixelSize: 16
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }
        MouseArea {
            anchors.fill: parent
            onClicked: backend.clearSignalNetAlert()
        }
        Timer {
            interval: 10000
            running: snAlertOverlay.visible
            onTriggered: backend.clearSignalNetAlert()
        }
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
