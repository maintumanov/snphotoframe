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
            console.log("RTSP1: QML play", url)
            rtspPlayer.stop()
            rtspPlayer.source = ""
            rtspResetTimer.url = url
            rtspResetTimer.start()
        }
        onRtspStopPlayer: {
            console.log("RTSP1: QML stopPlayer")
            rtspResetTimer.stop()
            rtspPlayer.stop()
            rtspPlayer.source = ""
        }
        onRtspShowOverlay: {
            connectingOverlay.visible = true
        }
        onRtspHideOverlay: {
            connectingOverlay.visible = false
        }
        onRtsp2Play: {
            console.log("RTSP2: QML play", url)
            rtspPlayer2.stop()
            rtspPlayer2.source = ""
            rtspResetTimer2.url = url
            rtspResetTimer2.start()
        }
        onRtsp2StopPlayer: {
            console.log("RTSP2: QML stopPlayer")
            rtspResetTimer2.stop()
            rtspPlayer2.stop()
            rtspPlayer2.source = ""
        }
        onRtsp2ShowOverlay: { connectingOverlay2.visible = true }
        onRtsp2HideOverlay: { connectingOverlay2.visible = false }
        onRtsp3Play: {
            console.log("RTSP3: QML play", url)
            rtspPlayer3.stop()
            rtspPlayer3.source = ""
            rtspResetTimer3.url = url
            rtspResetTimer3.start()
        }
        onRtsp3StopPlayer: {
            console.log("RTSP3: QML stopPlayer")
            rtspResetTimer3.stop()
            rtspPlayer3.stop()
            rtspPlayer3.source = ""
        }
        onRtsp3ShowOverlay: { connectingOverlay3.visible = true }
        onRtsp3HideOverlay: { connectingOverlay3.visible = false }
        onSignalNetAlertChanged: {
            if (backend.signalNetAlert.length > 0)
                snAlertBanner.show()
            else
                snAlertBanner.hide()
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
            visible: (backend.rtspState || 0) !== 2 && (backend.rtsp2State || 0) !== 2 && (backend.rtsp3State || 0) !== 2
        }

        Image {
            id: imageB
            anchors.fill: parent
            fillMode: Image.PreserveAspectFit
            smooth: true
            opacity: 0.0
            visible: (backend.rtspState || 0) !== 2 && (backend.rtsp2State || 0) !== 2 && (backend.rtsp3State || 0) !== 2
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
                console.log("RTSP1: errorChanged error=", error, errorString)
                if (source.toString() !== "" && error !== MediaPlayer.NoError)
                    backend.onRtspError(errorString)
            }
            onPlaybackStateChanged: {
                console.log("RTSP1: playbackState=", playbackState)
                if (playbackState === MediaPlayer.PlayingState)
                    backend.onRtspPlaying()
            }
        }

        Timer {
            id: rtspResetTimer
            interval: 150
            property string url: ""
            onTriggered: {
                rtspPlayer.stop()
                rtspPlayer.source = url
                rtspPlayer.play()
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
                    text: backend.rtspErrorMsg
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
            onErrorChanged: { console.log("RTSP2: errorChanged error=", error, errorString); if (source.toString() !== "" && error !== MediaPlayer.NoError) backend.onRtsp2Error(errorString) }
            onPlaybackStateChanged: { console.log("RTSP2: playbackState=", playbackState); if (playbackState === MediaPlayer.PlayingState) backend.onRtsp2Playing() }
        }
        Timer {
            id: rtspResetTimer2
            interval: 150
            property string url: ""
            onTriggered: {
                rtspPlayer2.stop()
                rtspPlayer2.source = url
                rtspPlayer2.play()
            }
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
            onErrorChanged: { console.log("RTSP3: errorChanged error=", error, errorString); if (source.toString() !== "" && error !== MediaPlayer.NoError) backend.onRtsp3Error(errorString) }
            onPlaybackStateChanged: { console.log("RTSP3: playbackState=", playbackState); if (playbackState === MediaPlayer.PlayingState) backend.onRtsp3Playing() }
        }
        Timer {
            id: rtspResetTimer3
            interval: 150
            property string url: ""
            onTriggered: {
                rtspPlayer3.stop()
                rtspPlayer3.source = url
                rtspPlayer3.play()
            }
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
            height: 64; z: 2
            Repeater {
                model: ["\u0421\u0435\u0442\u044c", "\u0420\u0430\u0441\u043f\u0438\u0441\u0430\u043d\u0438\u0435", "RTSP", "SignalNet", "\u0414\u0438\u0441\u043f\u043b\u0435\u0439"]
                Rectangle {
                    width: tabBar.width / 5; height: 64
                    color: settingsTab === index ? "#333" : "#1a1a1a"
                    border.color: settingsTab === index ? "#5af" : "#333"
                    border.width: settingsTab === index ? 2 : 1
                    Text {
                        text: modelData; color: settingsTab === index ? "#5af" : "#aaa"
                        font.pixelSize: 18; font.bold: settingsTab === index
                        anchors.centerIn: parent
                    }
                    MouseArea { anchors.fill: parent; onClicked: settingsTab = index }
                }
            }
        }

        StackLayout {
            anchors.top: tabBar.bottom; anchors.left: parent.left; anchors.right: parent.right; anchors.bottom: parent.bottom; anchors.bottomMargin: 80
            currentIndex: settingsTab

            // === TAB 0: Network ===
            Flickable {
                contentHeight: netCol.height + 40; clip: true
                Column {
                    id: netCol; width: parent.width - 60; x: 30; y: 20; spacing: 0

                    Text { text: "\u0421\u0435\u0442\u044c / SMB"; color: "#aaa"; font.pixelSize: 20; font.bold: true; bottomPadding: 10 }

                    Column { width: parent.width; spacing: 10
                        Row { spacing: 12; width: parent.width; height: 48
                            Text { text: "\u0421\u0435\u0440\u0432\u0435\u0440"; color: "#ccc"; font.pixelSize: 16; width: 120; verticalAlignment: Text.AlignVCenter }
                            TextField { id: srvField; text: backend.server; onTextChanged: backend.server = text; width: parent.width - 132; color: "white"; font.pixelSize: 16; padding: 12; background: Rectangle { radius: 8; color: "#222"; border.color: srvField.activeFocus ? "#5af" : "#444" } } }
                        Row { spacing: 12; width: parent.width; height: 48
                            Text { text: "\u041f\u0430\u043f\u043a\u0430"; color: "#ccc"; font.pixelSize: 16; width: 120; verticalAlignment: Text.AlignVCenter }
                            TextField { id: shrField; text: backend.share; onTextChanged: backend.share = text; width: parent.width - 132; color: "white"; font.pixelSize: 16; padding: 12; background: Rectangle { radius: 8; color: "#222"; border.color: shrField.activeFocus ? "#5af" : "#444" } } }
                        Row { spacing: 12; width: parent.width; height: 48
                            Text { text: "\u041b\u043e\u0433\u0438\u043d"; color: "#ccc"; font.pixelSize: 16; width: 120; verticalAlignment: Text.AlignVCenter }
                            TextField { id: usrField; text: backend.user; onTextChanged: backend.user = text; width: parent.width - 132; color: "white"; font.pixelSize: 16; padding: 12; background: Rectangle { radius: 8; color: "#222"; border.color: usrField.activeFocus ? "#5af" : "#444" } } }
                        Row { spacing: 12; width: parent.width; height: 48
                            Text { text: "\u041f\u0430\u0440\u043e\u043b\u044c"; color: "#ccc"; font.pixelSize: 16; width: 120; verticalAlignment: Text.AlignVCenter }
                            TextField { id: pswField; text: backend.pass; onTextChanged: backend.pass = text; echoMode: TextInput.Password; enabled: !guestChk.checked; width: parent.width - 132; color: "white"; font.pixelSize: 16; padding: 12; background: Rectangle { radius: 8; color: guestChk.checked ? "#111" : "#222"; border.color: pswField.activeFocus ? "#5af" : "#444" } } }
                        Row { spacing: 12; width: parent.width; height: 48
                            CheckBox {
                                id: guestChk; checked: backend.useGuest ? true : false; onCheckedChanged: backend.useGuest = checked
                                contentItem: Text { text: "\u0413\u043e\u0441\u0442\u0435\u0432\u043e\u0439 \u0434\u043e\u0441\u0442\u0443\u043f"; color: "white"; font.pixelSize: 16; leftPadding: 32; verticalAlignment: Text.AlignVCenter }
                                indicator: Rectangle { width: 22; height: 22; radius: 6; color: guestChk.checked ? "#5a5" : "#222"; border.color: "#666"; anchors.verticalCenter: parent.verticalCenter } } }
                        Row { spacing: 12; width: parent.width; height: 48
                            Text { text: "SMB"; color: "#ccc"; font.pixelSize: 16; width: 120; verticalAlignment: Text.AlignVCenter }
                            ComboBox {
                                id: smbCombo; model: ["1.0", "2.0", "2.1", "3.0", "3.1.1"]; currentIndex: model.indexOf(backend.smbVers); onActivated: backend.smbVers = currentText; width: 160; height: 40
                                background: Rectangle { radius: 8; color: "#222"; border.color: "#444" }
                                contentItem: Text { text: smbCombo.displayText; color: "white"; font.pixelSize: 16; verticalAlignment: Text.AlignVCenter; leftPadding: 12 }
                                delegate: ItemDelegate { width: smbCombo.width; contentItem: Text { text: modelData; color: "white"; font.pixelSize: 16; verticalAlignment: Text.AlignVCenter; leftPadding: 12 }
                                    background: Rectangle { color: index === smbCombo.currentIndex ? "#444" : "#222" } } } }
                    }

                    Item { width: parent.width; height: 21
                        Rectangle { anchors.centerIn: parent; width: parent.width; height: 1; color: "#333" } }
                    Text { text: "\u0421\u043b\u0430\u0439\u0434\u0448\u043e\u0443"; color: "#aaa"; font.pixelSize: 20; font.bold: true; bottomPadding: 10 }

                    Column { width: parent.width; spacing: 10
                        Row { spacing: 12; width: parent.width; height: 48
                            Text { text: "\u0418\u043d\u0442\u0435\u0440\u0432\u0430\u043b (\u0441\u0435\u043a)"; color: "#ccc"; font.pixelSize: 16; width: 160; verticalAlignment: Text.AlignVCenter }
                            SpinBox {
                                id: intervalSpin; from: 2; to: 3600; value: backend.interval; onValueModified: backend.interval = value; width: 160; height: 40
                                background: Rectangle { radius: 8; color: "#222"; border.color: "#444" }
                                contentItem: TextInput { text: intervalSpin.value; color: "white"; font.pixelSize: 16; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter } } }
                        Row { spacing: 12; width: parent.width; height: 48
                            CheckBox {
                                id: shuffleChk; checked: backend.shuffle; onCheckedChanged: backend.shuffle = checked
                                contentItem: Text { text: "\u0421\u043b\u0443\u0447\u0430\u0439\u043d\u044b\u0439 \u043f\u043e\u0440\u044f\u0434\u043e\u043a"; color: "white"; font.pixelSize: 16; leftPadding: 32; verticalAlignment: Text.AlignVCenter }
                                indicator: Rectangle { width: 22; height: 22; radius: 6; color: shuffleChk.checked ? "#5a5" : "#222"; border.color: "#666"; anchors.verticalCenter: parent.verticalCenter } } }
                    }
                }
            }

            // === TAB 1: Schedule ===
            Flickable {
                contentHeight: schedCol.height + 40; clip: true
                Column {
                    id: schedCol; width: parent.width - 60; x: 30; y: 20; spacing: 0

                    Text { text: "\u0420\u0430\u0441\u043f\u0438\u0441\u0430\u043d\u0438\u0435"; color: "#aaa"; font.pixelSize: 20; font.bold: true; bottomPadding: 10 }

                    Column { width: parent.width; spacing: 10
                        Row { spacing: 12; width: parent.width; height: 48
                            CheckBox {
                                id: schedChk; checked: backend.useSchedule; onCheckedChanged: backend.useSchedule = checked
                                contentItem: Text { text: "\u0412\u043a\u043b\u044e\u0447\u0438\u0442\u044c \u0441\u043f\u044f\u0449\u0438\u0439 \u0440\u0435\u0436\u0438\u043c"; color: "white"; font.pixelSize: 16; leftPadding: 32; verticalAlignment: Text.AlignVCenter }
                                indicator: Rectangle { width: 22; height: 22; radius: 6; color: schedChk.checked ? "#5a5" : "#222"; border.color: "#666"; anchors.verticalCenter: parent.verticalCenter } } }
                        Row { spacing: 12; width: parent.width; height: 48
                            Text { text: "\u041f\u0440\u043e\u0431\u0443\u0436\u0434\u0435\u043d\u0438\u0435"; color: "#ccc"; font.pixelSize: 16; width: 120; verticalAlignment: Text.AlignVCenter }
                            TextField { id: wakeField; text: backend.wakeTime; onEditingFinished: backend.wakeTime = text; width: 120; color: "white"; font.pixelSize: 16; padding: 12; inputMethodHints: Qt.ImhTime; background: Rectangle { radius: 8; color: "#222"; border.color: wakeField.activeFocus ? "#5af" : "#444" } } }
                        Row { spacing: 12; width: parent.width; height: 48
                            Text { text: "\u0421\u043d"; color: "#ccc"; font.pixelSize: 16; width: 120; verticalAlignment: Text.AlignVCenter }
                            TextField { id: sleepField; text: backend.sleepTime; onEditingFinished: backend.sleepTime = text; width: 120; color: "white"; font.pixelSize: 16; padding: 12; inputMethodHints: Qt.ImhTime; background: Rectangle { radius: 8; color: "#222"; border.color: sleepField.activeFocus ? "#5af" : "#444" } } }
                    }
                }
            }

            // === TAB 2: RTSP ===
            Flickable {
                contentHeight: rtspCol.height + 40; clip: true
                Column {
                    id: rtspCol; width: parent.width - 60; x: 30; y: 20; spacing: 0

                    // Camera 1
                    Text { text: "\U0001f4f7 \u041a\u0430\u043c\u0435\u0440\u0430 1"; color: "#aaa"; font.pixelSize: 20; font.bold: true; bottomPadding: 10 }
                    Column { width: parent.width; spacing: 10
                        Row { spacing: 12; width: parent.width; height: 48
                            CheckBox { id: rtspChk; checked: backend.useRtsp; onCheckedChanged: backend.useRtsp = checked
                                contentItem: Text { text: "\u041f\u043e\u043a\u0430\u0437\u044b\u0432\u0430\u0442\u044c"; color: "white"; font.pixelSize: 16; leftPadding: 32; verticalAlignment: Text.AlignVCenter }
                                indicator: Rectangle { width: 22; height: 22; radius: 6; color: rtspChk.checked ? "#5a5" : "#222"; border.color: "#666"; anchors.verticalCenter: parent.verticalCenter } } }
                        Row { spacing: 12; width: parent.width; height: 48
                            Text { text: "URL"; color: "#ccc"; font.pixelSize: 16; width: 60; verticalAlignment: Text.AlignVCenter }
                            TextField { id: rtspField; text: backend.rtspUrl; onTextChanged: backend.rtspUrl = text; placeholderText: "rtsp://..."; width: parent.width - 72; color: "white"; font.pixelSize: 14; padding: 12; background: Rectangle { radius: 8; color: "#222"; border.color: rtspField.activeFocus ? "#5af" : "#444" } } }
                        Row { spacing: 12; width: parent.width; height: 48
                            Text { text: "\u0412\u0440\u0435\u043c\u044f (\u0441\u0435\u043a)"; color: "#ccc"; font.pixelSize: 16; width: 120; verticalAlignment: Text.AlignVCenter }
                            SpinBox { id: camDurSpin; from: 1; to: 3600; value: backend.cameraDuration; onValueModified: backend.cameraDuration = value; width: 120; height: 40
                                background: Rectangle { radius: 8; color: "#222"; border.color: "#444" }
                                contentItem: TextInput { text: camDurSpin.value; color: "white"; font.pixelSize: 16; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter } } }
                    }

                    Item { width: parent.width; height: 17
                        Rectangle { anchors.centerIn: parent; width: parent.width; height: 1; color: "#333" } }

                    // Camera 2
                    Text { text: "\U0001f4f7 \u041a\u0430\u043c\u0435\u0440\u0430 2"; color: "#aaa"; font.pixelSize: 20; font.bold: true; bottomPadding: 10 }
                    Column { width: parent.width; spacing: 10
                        Row { spacing: 12; width: parent.width; height: 48
                            CheckBox { id: rtspChk2; checked: backend.useRtsp2; onCheckedChanged: backend.useRtsp2 = checked
                                contentItem: Text { text: "\u041f\u043e\u043a\u0430\u0437\u044b\u0432\u0430\u0442\u044c"; color: "white"; font.pixelSize: 16; leftPadding: 32; verticalAlignment: Text.AlignVCenter }
                                indicator: Rectangle { width: 22; height: 22; radius: 6; color: rtspChk2.checked ? "#5a5" : "#222"; border.color: "#666"; anchors.verticalCenter: parent.verticalCenter } } }
                        Row { spacing: 12; width: parent.width; height: 48
                            Text { text: "URL"; color: "#ccc"; font.pixelSize: 16; width: 60; verticalAlignment: Text.AlignVCenter }
                            TextField { id: rtspField2; text: backend.rtspUrl2; onTextChanged: backend.rtspUrl2 = text; placeholderText: "rtsp://..."; width: parent.width - 72; color: "white"; font.pixelSize: 14; padding: 12; background: Rectangle { radius: 8; color: "#222"; border.color: rtspField2.activeFocus ? "#5af" : "#444" } } }
                        Row { spacing: 12; width: parent.width; height: 48
                            Text { text: "\u0412\u0440\u0435\u043c\u044f (\u0441\u0435\u043a)"; color: "#ccc"; font.pixelSize: 16; width: 120; verticalAlignment: Text.AlignVCenter }
                            SpinBox { id: cam2DurSpin; from: 1; to: 3600; value: backend.camera2Duration; onValueModified: backend.camera2Duration = value; width: 120; height: 40
                                background: Rectangle { radius: 8; color: "#222"; border.color: "#444" }
                                contentItem: TextInput { text: cam2DurSpin.value; color: "white"; font.pixelSize: 16; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter } } }
                    }

                    Item { width: parent.width; height: 17
                        Rectangle { anchors.centerIn: parent; width: parent.width; height: 1; color: "#333" } }

                    // Camera 3
                    Text { text: "\U0001f4f7 \u041a\u0430\u043c\u0435\u0440\u0430 3"; color: "#aaa"; font.pixelSize: 20; font.bold: true; bottomPadding: 10 }
                    Column { width: parent.width; spacing: 10
                        Row { spacing: 12; width: parent.width; height: 48
                            CheckBox { id: rtspChk3; checked: backend.useRtsp3; onCheckedChanged: backend.useRtsp3 = checked
                                contentItem: Text { text: "\u041f\u043e\u043a\u0430\u0437\u044b\u0432\u0430\u0442\u044c"; color: "white"; font.pixelSize: 16; leftPadding: 32; verticalAlignment: Text.AlignVCenter }
                                indicator: Rectangle { width: 22; height: 22; radius: 6; color: rtspChk3.checked ? "#5a5" : "#222"; border.color: "#666"; anchors.verticalCenter: parent.verticalCenter } } }
                        Row { spacing: 12; width: parent.width; height: 48
                            Text { text: "URL"; color: "#ccc"; font.pixelSize: 16; width: 60; verticalAlignment: Text.AlignVCenter }
                            TextField { id: rtspField3; text: backend.rtspUrl3; onTextChanged: backend.rtspUrl3 = text; placeholderText: "rtsp://..."; width: parent.width - 72; color: "white"; font.pixelSize: 14; padding: 12; background: Rectangle { radius: 8; color: "#222"; border.color: rtspField3.activeFocus ? "#5af" : "#444" } } }
                        Row { spacing: 12; width: parent.width; height: 48
                            Text { text: "\u0412\u0440\u0435\u043c\u044f (\u0441\u0435\u043a)"; color: "#ccc"; font.pixelSize: 16; width: 120; verticalAlignment: Text.AlignVCenter }
                            SpinBox { id: cam3DurSpin; from: 1; to: 3600; value: backend.camera3Duration; onValueModified: backend.camera3Duration = value; width: 120; height: 40
                                background: Rectangle { radius: 8; color: "#222"; border.color: "#444" }
                                contentItem: TextInput { text: cam3DurSpin.value; color: "white"; font.pixelSize: 16; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter } } }
                    }
                }
            }

            // === TAB 3: SignalNet ===
            Flickable {
                contentHeight: snCol.height + 40; clip: true
                Column {
                    id: snCol; width: parent.width - 60; x: 30; y: 20; spacing: 0

                    Text { text: "SignalNet"; color: "#aaa"; font.pixelSize: 20; font.bold: true; bottomPadding: 10 }

                    Column { width: parent.width; spacing: 10
                        Row { spacing: 12; width: parent.width; height: 48
                            CheckBox { id: snChk; checked: backend.useSignalNet; onCheckedChanged: backend.useSignalNet = checked
                                contentItem: Text { text: "\u041f\u043e\u0434\u043a\u043b\u044e\u0447\u0438\u0442\u044c SignalNet"; color: "white"; font.pixelSize: 16; leftPadding: 32; verticalAlignment: Text.AlignVCenter }
                                indicator: Rectangle { width: 22; height: 22; radius: 6; color: snChk.checked ? "#5a5" : "#222"; border.color: "#666"; anchors.verticalCenter: parent.verticalCenter } } }
                        Row { spacing: 12; width: parent.width; height: 48
                            CheckBox { id: actBtnChk; checked: backend.useActionButtons; onCheckedChanged: backend.useActionButtons = checked
                                contentItem: Text { text: "\u041a\u043d\u043e\u043f\u043a\u0438 A1/A2"; color: "white"; font.pixelSize: 16; leftPadding: 32; verticalAlignment: Text.AlignVCenter }
                                indicator: Rectangle { width: 22; height: 22; radius: 6; color: actBtnChk.checked ? "#5a5" : "#222"; border.color: "#666"; anchors.verticalCenter: parent.verticalCenter } } }
                        Row { spacing: 12; width: parent.width; height: 48
                            Text { text: "\u0421\u0435\u0440\u0432\u0435\u0440"; color: "#ccc"; font.pixelSize: 16; width: 120; verticalAlignment: Text.AlignVCenter }
                            TextField { id: snSrvField; text: backend.signalNetServer; onTextChanged: backend.signalNetServer = text; placeholderText: "192.168.1.1"; width: parent.width - 132; color: "white"; font.pixelSize: 16; padding: 12; background: Rectangle { radius: 8; color: "#222"; border.color: snSrvField.activeFocus ? "#5af" : "#444" } } }
                        Row { spacing: 12; width: parent.width; height: 48
                            Text { text: "\u041f\u043e\u0440\u0442"; color: "#ccc"; font.pixelSize: 16; width: 120; verticalAlignment: Text.AlignVCenter }
                            TextField { id: snPortField; text: backend.signalNetPort; onTextChanged: backend.signalNetPort = parseInt(text) || 8888; placeholderText: "8888"; width: parent.width - 132; color: "white"; font.pixelSize: 16; padding: 12; background: Rectangle { radius: 8; color: "#222"; border.color: snPortField.activeFocus ? "#5af" : "#444" } } }
                        Row { spacing: 12; width: parent.width; height: 48
                            Text { text: "\u0422\u0440\u0430\u043d\u0441\u043f\u043e\u0440\u0442"; color: "#ccc"; font.pixelSize: 16; width: 120; verticalAlignment: Text.AlignVCenter }
                            ComboBox {
                                id: snTransportCombo; model: ["TCP", "UDP"]; currentIndex: backend.signalNetUseUdp ? 1 : 0; onActivated: backend.signalNetUseUdp = (index === 1); width: 160; height: 40
                                background: Rectangle { radius: 8; color: "#222"; border.color: "#444" }
                                contentItem: Text { text: snTransportCombo.displayText; color: "white"; font.pixelSize: 16; verticalAlignment: Text.AlignVCenter; leftPadding: 12 }
                                delegate: ItemDelegate { width: snTransportCombo.width; contentItem: Text { text: modelData; color: "white"; font.pixelSize: 16; verticalAlignment: Text.AlignVCenter; leftPadding: 12 }
                                    background: Rectangle { color: index === snTransportCombo.currentIndex ? "#444" : "#222" } } } }
                    }

                    // TCP fields
                    Column { width: parent.width; spacing: 10; visible: !backend.signalNetUseUdp
                        Row { spacing: 12; width: parent.width; height: 48
                            Text { text: "\u041b\u043e\u0433\u0438\u043d"; color: "#ccc"; font.pixelSize: 16; width: 120; verticalAlignment: Text.AlignVCenter }
                            TextField { id: snLoginField; text: backend.signalNetLogin; onTextChanged: backend.signalNetLogin = text; width: parent.width - 132; color: "white"; font.pixelSize: 16; padding: 12; background: Rectangle { radius: 8; color: "#222"; border.color: snLoginField.activeFocus ? "#5af" : "#444" } } }
                        Row { spacing: 12; width: parent.width; height: 48
                            Text { text: "\u041f\u0430\u0440\u043e\u043b\u044c"; color: "#ccc"; font.pixelSize: 16; width: 120; verticalAlignment: Text.AlignVCenter }
                            TextField { id: snPassField; text: backend.signalNetPass; onTextChanged: backend.signalNetPass = text; echoMode: TextInput.Password; width: parent.width - 132; color: "white"; font.pixelSize: 16; padding: 12; background: Rectangle { radius: 8; color: "#222"; border.color: snPassField.activeFocus ? "#5af" : "#444" } } }
                    }

                    // UDP fields
                    Column { width: parent.width; spacing: 10; visible: backend.signalNetUseUdp
                        Row { spacing: 12; width: parent.width; height: 48
                            Text { text: "\u041b\u043e\u043a. \u043f\u043e\u0440\u0442"; color: "#ccc"; font.pixelSize: 16; width: 120; verticalAlignment: Text.AlignVCenter }
                            TextField { id: snUdpLocalPortField; text: backend.signalNetUdpLocalPort; onTextChanged: backend.signalNetUdpLocalPort = parseInt(text) || 29545; placeholderText: "29545"; width: parent.width - 132; color: "white"; font.pixelSize: 16; padding: 12; background: Rectangle { radius: 8; color: "#222"; border.color: snUdpLocalPortField.activeFocus ? "#5af" : "#444" } } }
                        Row { spacing: 12; width: parent.width; height: 48
                            Text { text: "\u041a\u043b\u044e\u0447"; color: "#ccc"; font.pixelSize: 16; width: 120; verticalAlignment: Text.AlignVCenter }
                            TextField { id: snUdpKeyField; text: backend.signalNetUdpKey; onTextChanged: backend.signalNetUdpKey = text; placeholderText: "signalnet"; width: parent.width - 132; color: "white"; font.pixelSize: 16; padding: 12; background: Rectangle { radius: 8; color: "#222"; border.color: snUdpKeyField.activeFocus ? "#5af" : "#444" } } }
                    }

                    // Connect button
                    Row { spacing: 12; width: parent.width; height: 48; topPadding: 10
                        Rectangle { width: 140; height: 44; radius: 10; color: snConnectMa.pressed ? "#555" : (backend.signalNetConnected ? "#5a5" : "#444"); border.color: "#666"; border.width: 1
                            Text { text: backend.signalNetConnected ? "\u041e\u0442\u043a\u043b\u044e\u0447\u0438\u0442\u044c" : "\u041f\u043e\u0434\u043a\u043b\u044e\u0447\u0438\u0442\u044c"; color: "white"; font.pixelSize: 16; font.bold: true; anchors.centerIn: parent }
                            MouseArea { id: snConnectMa; anchors.fill: parent; onClicked: {
                                if (backend.signalNetConnected) backend.disconnectSignalNet()
                                else backend.connectSignalNet() } } }
                        Text { text: backend.signalNetConnected ? "\u2705 \u041f\u043e\u0434\u043a\u043b\u044e\u0447\u0435\u043d\u043e" : "\u274c \u041e\u0442\u043a\u043b\u044e\u0447\u0435\u043d\u043e"; color: backend.signalNetConnected ? "#5a5" : "#f55"; font.pixelSize: 16; anchors.verticalCenter: parent.verticalCenter } }
                }
            }

            // === TAB 4: Display ===
            Flickable {
                contentHeight: dispCol.height + 40; clip: true
                Column {
                    id: dispCol; width: parent.width - 60; x: 30; y: 20; spacing: 0

                    Text { text: "\u0414\u0438\u0441\u043f\u043b\u0435\u0439"; color: "#aaa"; font.pixelSize: 20; font.bold: true; bottomPadding: 10 }

                    Column { width: parent.width; spacing: 10
                        Text { text: "DSI-\u043f\u043e\u0434\u0441\u0432\u0435\u0442\u043a\u0430 \u043d\u0435 \u043e\u0431\u043d\u0430\u0440\u0443\u0436\u0435\u043d\u0430"; color: "#888"; font.pixelSize: 14; visible: !backend.backlightAvailable }
                        Row { spacing: 12; width: parent.width; height: 48
                            Text { text: "\u042f\u0440\u043a\u043e\u0441\u0442\u044c"; color: "#ccc"; font.pixelSize: 16; width: 120; verticalAlignment: Text.AlignVCenter }
                            Slider {
                                id: brightSlider
                                from: 0; to: 100; stepSize: 1
                                value: backend.brightness
                                onMoved: backend.brightness = value
                                enabled: backend.backlightAvailable
                                opacity: backend.backlightAvailable ? 1.0 : 0.4
                                width: parent.width - 210
                                anchors.verticalCenter: parent.verticalCenter
                            }
                            Text { text: Math.round(backend.brightness) + " %"; color: "white"; font.pixelSize: 16; width: 60; verticalAlignment: Text.AlignVCenter }
                        }
                        Text { text: "\u0412 \u0441\u043f\u044f\u0449\u0435\u043c \u0440\u0435\u0436\u0438\u043c\u0435 \u044f\u0440\u043a\u043e\u0441\u0442\u044c \u0441\u043d\u0438\u0436\u0430\u0435\u0442\u0441\u044f \u0430\u0432\u0442\u043e\u043c\u0430\u0442\u0438\u0447\u0435\u0441\u043a\u0438"; color: "#888"; font.pixelSize: 14; visible: backend.backlightAvailable }
                    }
                }
            }
        } // StackLayout

        // Bottom buttons
        Row { spacing: 20; anchors.bottom: parent.bottom; anchors.bottomMargin: 20; anchors.horizontalCenter: parent.horizontalCenter; z: 2
            Rectangle { width: 160; height: 56; radius: 14; color: exitMa.pressed ? "#666" : exitMa.containsMouse ? "#444" : "transparent"; border.color: "white"; border.width: 2
                Text { text: "\u0412\u044b\u0445\u043e\u0434"; color: "white"; font.pixelSize: 22; font.bold: true; anchors.centerIn: parent }
                MouseArea { id: exitMa; anchors.fill: parent; hoverEnabled: true; onClicked: backend.pageIndex = 0 } }
            Rectangle { width: 160; height: 56; radius: 14; color: saveMa.pressed ? "#666" : saveMa.containsMouse ? "#444" : "transparent"; border.color: "white"; border.width: 2
                Text { text: "\u0421\u043e\u0445\u0440\u0430\u043d\u0438\u0442\u044c"; color: "white"; font.pixelSize: 22; font.bold: true; anchors.centerIn: parent }
                MouseArea { id: saveMa; anchors.fill: parent; hoverEnabled: true; onClicked: backend.saveSettings() } }
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
            Rectangle { width: 56; height: 56; radius: 14; color: firstMa.pressed ? "#555" : "#4d000000"; border.color: "white"; border.width: 1
                Image { width: 28; height: 28; anchors.centerIn: parent; source: "data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 24 24' fill='white'%3E%3Cpath d='M6 6h2v12H6zm3.5 6l8.5 6V6z'/%3E%3C/svg%3E" }
                MouseArea { id: firstMa; anchors.fill: parent; onClicked: backend.firstSlide() } }
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
            Rectangle { visible: backend.useSignalNet && backend.signalNetConnected && backend.useActionButtons; width: 56; height: 56; radius: 14; color: a1BarMa.pressed ? "#555" : "#4d000000"; border.color: "#fa0"; border.width: 1
                Image { width: 28; height: 28; anchors.centerIn: parent; source: "data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 24 24' fill='%23fa0'%3E%3Cpath d='M8 5v14l11-7z'/%3E%3C/svg%3E" }
                MouseArea { id: a1BarMa; anchors.fill: parent; onClicked: backend.sendAction1() } }
            Rectangle { visible: backend.useSignalNet && backend.signalNetConnected && backend.useActionButtons; width: 56; height: 56; radius: 14; color: a2BarMa.pressed ? "#555" : "#4d000000"; border.color: "#fa0"; border.width: 1
                Image { width: 28; height: 28; anchors.centerIn: parent; source: "data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 24 24' fill='%23fa0'%3E%3Cpath d='M6 18l8.5-6L6 6v12zM16 6v12h2V6h-2z'/%3E%3C/svg%3E" }
                MouseArea { id: a2BarMa; anchors.fill: parent; onClicked: backend.sendAction2() } }
            Rectangle { width: 56; height: 56; radius: 14; color: backend.pageIndex === 1 ? "#4dffffff" : (sma.pressed ? "#555" : "#4d000000"); border.color: "white"; border.width: 1
                Text { text: "\u2261"; color: "white"; font.pixelSize: 28; font.bold: true; anchors.centerIn: parent }
                MouseArea { id: sma; anchors.fill: parent; onClicked: backend.pageIndex = backend.pageIndex === 1 ? 0 : 1 } }
            Rectangle { width: 56; height: 56; radius: 14; color: lastMa.pressed ? "#555" : "#4d000000"; border.color: "white"; border.width: 1
                Image { width: 28; height: 28; anchors.centerIn: parent; source: "data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 24 24' fill='white'%3E%3Cpath d='M6 18l8.5-6L6 6v12zM16 6v12h2V6h-2z'/%3E%3C/svg%3E" }
                MouseArea { id: lastMa; anchors.fill: parent; onClicked: backend.lastSlide() } }
            Rectangle { visible: false; width: 56; height: 56; radius: 14; color: backend.pageIndex === 2 ? "#4dffffff" : (tma.pressed ? "#555" : "#4d000000"); border.color: "white"; border.width: 1
                Text { text: "\u2713"; color: "white"; font.pixelSize: 24; font.bold: true; anchors.centerIn: parent }
                MouseArea { id: tma; anchors.fill: parent; onClicked: backend.pageIndex = backend.pageIndex === 2 ? 0 : 2 } }
        }
    }

    Rectangle {
        id: loadingOverlay
        anchors.centerIn: parent
        width: 220; height: 220; radius: 24
        color: "#cc0a0a14"
        border.color: loadingRing.active ? "#5af" : "#333"
        border.width: 2
        visible: loading && (backend.rtspState || 0) !== 2 && backend.pageIndex === 0
        z: 5; opacity: 0
        Behavior on opacity { NumberAnimation { duration: 400; easing.type: Easing.InOutQuad } }
        onVisibleChanged: { if (visible) opacity = 1; else opacity = 0 }

        // Spinning ring
        Rectangle {
            id: loadingRing
            property bool active: loadingOverlay.visible
            width: 80; height: 80; radius: 40
            color: "transparent"
            border.color: "#5af"; border.width: 3
            anchors.centerIn: parent; anchors.verticalCenterOffset: -20
            visible: false
            SequentialAnimation on rotation {
                id: loadingSpin; running: loadingOverlay.visible; loops: Animation.Infinite
                NumberAnimation { from: 0; to: 360; duration: 2000; easing.type: Easing.Linear } }
            SequentialAnimation on opacity {
                running: loadingOverlay.visible; loops: Animation.Infinite
                NumberAnimation { from: 0.3; to: 1.0; duration: 800; easing.type: Easing.InOutQuad }
                NumberAnimation { from: 1.0; to: 0.3; duration: 800; easing.type: Easing.InOutQuad } }
        }

        // Photo icon (pulsing)
        Image {
            width: 40; height: 40; anchors.centerIn: parent; anchors.verticalCenterOffset: -20
            source: "data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 24 24' fill='%235af'%3E%3Cpath d='M21 19V5c0-1.1-.9-2-2-2H5c-1.1 0-2 .9-2 2v14c0 1.1.9 2 2 2h14c1.1 0 2-.9 2-2zM8.5 13.5l2.5 3.01L14.5 12l4.5 6H5l3.5-4.5z'/%3E%3C/svg%3E"
            SequentialAnimation on scale {
                running: loadingOverlay.visible; loops: Animation.Infinite
                NumberAnimation { from: 0.9; to: 1.1; duration: 1000; easing.type: Easing.InOutQuad }
                NumberAnimation { from: 1.1; to: 0.9; duration: 1000; easing.type: Easing.InOutQuad } }
        }

        // Loading text
        Text {
            text: loadingText
            color: "white"
            font.pixelSize: 16; font.bold: true
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom; anchors.bottomMargin: 40
            SequentialAnimation on opacity {
                running: loadingOverlay.visible; loops: Animation.Infinite
                NumberAnimation { from: 1.0; to: 0.4; duration: 1200; easing.type: Easing.InOutQuad }
                NumberAnimation { from: 0.4; to: 1.0; duration: 1200; easing.type: Easing.InOutQuad } }
        }

        // Animated dots
        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom; anchors.bottomMargin: 18
            spacing: 10
            Repeater {
                model: 3
                Rectangle {
                    width: 8; height: 8; radius: 4
                    color: "#5af"
                    property int idx: index
                    SequentialAnimation on opacity {
                        running: loadingOverlay.visible; loops: Animation.Infinite
                        PauseAnimation { duration: idx * 250 }
                        NumberAnimation { from: 0.2; to: 1.0; duration: 350 }
                        PauseAnimation { duration: 150 }
                        NumberAnimation { from: 1.0; to: 0.2; duration: 350 }
                        PauseAnimation { duration: 500 - idx * 250 } }
                }
            }
        }
    }

    Rectangle { id: sleepOverlay; anchors.fill: parent; color: "#000000"; visible: false; z: 50 }

    // Time & Date + Action Buttons — top left corner over photo
    Column {
        visible: !sleepOverlay.visible && backend.pageIndex !== 1
        anchors.top: parent.top; anchors.left: parent.left
        anchors.topMargin: 16; anchors.leftMargin: 16
        z: 20; spacing: 4
        Row { spacing: 6
            Rectangle { width: timeText.implicitWidth + 24; height: 52; radius: 12; color: "#88000000"; border.color: "#55ffffff"; border.width: 1
                Text { id: timeText; text: backend.currentTime; color: "white"; font.pixelSize: 32; font.bold: true; font.family: "Consolas, monospace"; anchors.centerIn: parent } }
            Rectangle { width: dateText.implicitWidth + 20; height: 52; radius: 12; color: backend.pageIndex === 3 ? "#4dffffff" : "#88000000"; border.color: "#55ffffff"; border.width: 1
                Text { id: dateText; text: backend.currentDate; color: "white"; font.pixelSize: 20; font.bold: true; font.family: "Consolas, monospace"; anchors.centerIn: parent }
                MouseArea { anchors.fill: parent; onClicked: backend.pageIndex = backend.pageIndex === 3 ? 0 : 3 } } }
    }

    // === INDICATOR PANEL (over photo, styled like control bar buttons) ===
    Column {
        visible: backend.useSignalNet && backend.signalNetConnected && backend.pageIndex !== 1
        anchors.top: parent.top; anchors.right: parent.right
        anchors.topMargin: 16; anchors.rightMargin: 16
        z: 20; spacing: 6

        // System status icons row
        Row {
            spacing: 6; width: parent.width; layoutDirection: Qt.RightToLeft

            // Photo file name
            Rectangle { visible: backend.currentFileDate.length > 0; width: Math.min(fileInfoText.implicitWidth + 20, 240); height: 34; radius: 8; color: "#88000000"; border.color: "#55ffffff"; border.width: 1
                Text { id: fileInfoText; text: backend.currentFileDate; color: "#aaa"; font.pixelSize: 13; font.family: "Consolas, monospace"; anchors.centerIn: parent } }

            // SignalNet connection indicator
            Rectangle {
                width: 34; height: 34; radius: 10
                color: "#66000000"; border.color: backend.signalNetConnected ? "#5a5" : "#555"; border.width: 1
                Rectangle { width: 10; height: 10; radius: 5; color: backend.signalNetConnected ? "#5a5" : "#f55"; anchors.centerIn: parent; opacity: 0.9 }
            }

            // Camera 1 indicator
            Rectangle {
                visible: backend.useRtsp
                width: 34; height: 34; radius: 10
                color: (backend.rtspState || 0) === 2 ? "#4d5af5" : "#66000000"
                border.color: (backend.rtspState || 0) === 2 ? "#5af" : ((backend.rtspState || 0) === 3 ? "#f55" : "#555")
                border.width: 1
                Image { width: 18; height: 18; anchors.centerIn: parent
                    source: "data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 24 24' fill='white'%3E%3Cpath d='M17 10.5V7c0-.55-.45-1-1-1H4c-.55 0-1 .45-1 1v10c0 .55.45 1 1 1h12c.55 0 1-.45 1-1v-3.5l4 4v-11l-4 4z'/%3E%3C/svg%3E" } }

            // Camera 2 indicator
            Rectangle {
                visible: backend.useRtsp2
                width: 34; height: 34; radius: 10
                color: (backend.rtsp2State || 0) === 2 ? "#4d5af5" : "#66000000"
                border.color: (backend.rtsp2State || 0) === 2 ? "#5af" : ((backend.rtsp2State || 0) === 3 ? "#f55" : "#555")
                border.width: 1
                Image { width: 16; height: 16; anchors.centerIn: parent
                    source: "data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 24 24' fill='white'%3E%3Cpath d='M17 10.5V7c0-.55-.45-1-1-1H4c-.55 0-1 .45-1 1v10c0 .55.45 1 1 1h12c.55 0 1-.45 1-1v-3.5l4 4v-11l-4 4z'/%3E%3C/svg%3E" }
                Text { text: "2"; color: "#5af"; font.pixelSize: 8; font.bold: true; anchors.right: parent.right; anchors.top: parent.top; anchors.rightMargin: 1; anchors.topMargin: 1 } }

            // Camera 3 indicator
            Rectangle {
                visible: backend.useRtsp3
                width: 34; height: 34; radius: 10
                color: (backend.rtsp3State || 0) === 2 ? "#4d5af5" : "#66000000"
                border.color: (backend.rtsp3State || 0) === 2 ? "#5af" : ((backend.rtsp3State || 0) === 3 ? "#f55" : "#555")
                border.width: 1
                Image { width: 16; height: 16; anchors.centerIn: parent
                    source: "data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 24 24' fill='white'%3E%3Cpath d='M17 10.5V7c0-.55-.45-1-1-1H4c-.55 0-1 .45-1 1v10c0 .55.45 1 1 1h12c.55 0 1-.45 1-1v-3.5l4 4v-11l-4 4z'/%3E%3C/svg%3E" }
                Text { text: "3"; color: "#5af"; font.pixelSize: 8; font.bold: true; anchors.right: parent.right; anchors.top: parent.top; anchors.rightMargin: 1; anchors.topMargin: 1 } }
        }

        // Outdoor temperature — prominent card
        Rectangle {
            visible: backend.signalNetTemperatureOutValid
            anchors.right: parent.right
            width: 180; height: 64; radius: 16
            color: "#aa0a1a3a"; border.color: "#5af"; border.width: 2
            Row { anchors.centerIn: parent; spacing: 10
                Image { width: 28; height: 28; anchors.verticalCenter: parent.verticalCenter
                    source: "data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 24 24' fill='%235af'%3E%3Cpath d='M6.76 4.84l-1.8-1.79-1.41 1.41 1.79 1.79 1.42-1.41zM4 10.5H1v2h3v-2zm9-9.95h-2V3.5h2V.55zm7.45 3.91l-1.41-1.41-1.79 1.79 1.41 1.41 1.79-1.79zm-3.21 13.7l1.79 1.8 1.41-1.41-1.8-1.79-1.4 1.4zM20 10.5v2h3v-2h-3zm-8-5c-3.31 0-6 2.69-6 6s2.69 6 6 6 6-2.69 6-6-2.69-6-6-6zm-1 16.95h2V19.5h-2v2.95zm-7.45-3.91l1.41 1.41 1.79-1.8-1.41-1.41-1.79 1.8z'/%3E%3C/svg%3E" }
                Text { text: backend.signalNetTemperatureOut.toFixed(1) + "\u00b0C"
                    color: "#5cf"
                    font.pixelSize: 32; font.bold: true; font.family: "Consolas, monospace"
                    anchors.verticalCenter: parent.verticalCenter } }
        }

        // Indoor temperature card
        Rectangle {
            visible: backend.signalNetTemperatureValid
            anchors.right: parent.right
            width: 160; height: 44; radius: 12
            property bool tempWarn: backend.signalNetTemperature < 18 || backend.signalNetTemperature > 24
            color: "#88000000"; border.color: tempWarn ? "#f55" : "#55ffffff"; border.width: 1
            Row { anchors.centerIn: parent; spacing: 8
                Image { width: 20; height: 20; anchors.verticalCenter: parent.verticalCenter
                    source: "data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 24 24' fill='%23e88'%3E%3Cpath d='M15 13V5c0-1.66-1.34-3-3-3S9 3.34 9 5v8c-1.21.91-2 2.37-2 4 0 2.76 2.24 5 5 5s5-2.24 5-5c0-1.63-.79-3.09-2-4zm-4-8c0-.55.45-1 1-1s1 .45 1 1h-1v1h1v2h-1v1h1v2h-2V5z'/%3E%3C/svg%3E" }
                Text { text: backend.signalNetTemperature.toFixed(1) + "\u00b0C"
                    color: parent.parent.tempWarn ? "#f55" : "white"
                    font.pixelSize: 20; font.bold: true; font.family: "Consolas, monospace"
                    anchors.verticalCenter: parent.verticalCenter } }
        }

        // Humidity card
        Rectangle {
            visible: backend.signalNetHumidityValid
            anchors.right: parent.right
            width: 160; height: 44; radius: 12
            property bool humWarn: backend.signalNetHumidity < 30 || backend.signalNetHumidity > 60
            color: "#88000000"; border.color: humWarn ? "#f55" : "#55ffffff"; border.width: 1
            Row { anchors.centerIn: parent; spacing: 8
                Image { width: 20; height: 20; anchors.verticalCenter: parent.verticalCenter
                    source: "data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 24 24' fill='%235cf'%3E%3Cpath d='M12 2c-5.33 4.55-8 8.48-8 11.8 0 4.98 3.8 8.2 8 8.2s8-3.22 8-8.2C20 10.48 17.33 6.55 12 2z'/%3E%3C/svg%3E" }
                Text { text: backend.signalNetHumidity.toFixed(0) + "%"
                    color: parent.parent.humWarn ? "#f55" : "white"
                    font.pixelSize: 20; font.bold: true; font.family: "Consolas, monospace"
                    anchors.verticalCenter: parent.verticalCenter } }
        }

        // CO2 card
        Rectangle {
            visible: backend.signalNetCo2Valid
            anchors.right: parent.right
            width: 160; height: 44; radius: 12
            property bool co2Warn: backend.signalNetCo2 >= 1000
            color: "#88000000"; border.color: co2Warn ? "#f55" : "#55ffffff"; border.width: 1
            Row { anchors.centerIn: parent; spacing: 8
                Image { width: 20; height: 20; anchors.verticalCenter: parent.verticalCenter
                    source: "data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 24 24' fill='%23fa3'%3E%3Cpath d='M19.35 10.04C18.67 6.59 15.64 4 12 4 9.11 4 6.6 5.64 5.35 8.04 2.34 8.36 0 10.91 0 14c0 3.31 2.69 6 6 6h13c2.76 0 5-2.24 5-5 0-2.64-2.05-4.78-4.65-4.96z'/%3E%3C/svg%3E" }
                Text { text: backend.signalNetCo2 + " ppm"
                    color: parent.parent.co2Warn ? "#f55" : "white"
                    font.pixelSize: 20; font.bold: true; font.family: "Consolas, monospace"
                    anchors.verticalCenter: parent.verticalCenter } }
        }

        // Dust card
        Rectangle {
            visible: backend.signalNetDustValid
            anchors.right: parent.right
            width: 160; height: 44; radius: 12
            property bool dustWarn: backend.signalNetDust >= 25
            color: "#88000000"; border.color: dustWarn ? "#f55" : "#55ffffff"; border.width: 1
            Row { anchors.centerIn: parent; spacing: 8
                Image { width: 20; height: 20; anchors.verticalCenter: parent.verticalCenter
                    source: "data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 24 24' fill='%23f77'%3E%3Cpath d='M14.5 17c0 1.65-1.35 3-3 3s-3-1.35-3-3h2c0 .55.45 1 1 1s1-.45 1-1-.45-1-1-1H2v-2h9.5c1.65 0 3 1.35 3 3zM19 6.5C19 4.57 17.43 3 15.5 3S12 4.57 12 6.5h2c0-.83.67-1.5 1.5-1.5s1.5.67 1.5 1.5S16.33 8 15.5 8H2v2h13.5c1.93 0 3.5-1.57 3.5-3.5zm-.5 4.5H2v2h16.5c.83 0 1.5.67 1.5 1.5s-.67 1.5-1.5 1.5S17 15.33 17 14.5h-2c0 1.93 1.57 3.5 3.5 3.5s3.5-1.57 3.5-3.5-1.57-3.5-3.5-3.5z'/%3E%3C/svg%3E" }
                Text { text: backend.signalNetDust + " \u03bcg/m\u00b3"
                    color: parent.parent.dustWarn ? "#f55" : "white"
                    font.pixelSize: 20; font.bold: true; font.family: "Consolas, monospace"
                    anchors.verticalCenter: parent.verticalCenter } }
        }

        // VAR card
        Rectangle {
            visible: backend.signalNetVarValid
            anchors.right: parent.right
            width: 160; height: 44; radius: 12
            color: "#88000000"; border.color: "#55ffffff"; border.width: 1
            Row { anchors.centerIn: parent; spacing: 8
                Image { width: 20; height: 20; anchors.verticalCenter: parent.verticalCenter
                    source: "data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 24 24' fill='%23aaa'%3E%3Cpath d='M11.99 2C6.47 2 2 6.48 2 12s4.47 10 9.99 10C17.52 22 22 17.52 22 12S17.52 2 11.99 2zM12 20c-4.42 0-8-3.58-8-8s3.58-8 8-8 8 3.58 8 8-3.58 8-8 8zm.99-13h-2v6l5.25 3.15.75-1.23-4-2.37V7z'/%3E%3C/svg%3E" }
                Text { text: backend.signalNetVar.toFixed(1)
                    color: "white"
                    font.pixelSize: 20; font.bold: true; font.family: "Consolas, monospace"
                    anchors.verticalCenter: parent.verticalCenter } }
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

        function show() {
            slideOut.stop()
            fadeOut.stop()
            autoHide.stop()
            visible = true
            opacity = 1
            slideIn.restart()
            autoHide.start()
        }
        function hide() {
            if (visible && !slideOut.running) {
                autoHide.stop()
                slideOut.start()
                fadeOut.start()
            }
            backend.clearSignalNetAlert()
        }
        onVisibleChanged: { if (visible) slideIn.start() }
        NumberAnimation on y { id: slideIn; from: -80; to: 20; duration: 300; easing.type: Easing.OutCubic
            onRunningChanged: { if (!running && snAlertBanner.visible) snAlertBanner.opacity = 1.0 } }
        NumberAnimation on y { id: slideOut; from: 20; to: -80; duration: 250; easing.type: Easing.InCubic
            onFinished: snAlertBanner.visible = false }
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
                MouseArea { anchors.fill: parent; onClicked: snAlertBanner.hide() }
            }
        }
        MouseArea { anchors.fill: parent; onClicked: snAlertBanner.hide() }
        Timer { id: autoHide; interval: 8000
            onTriggered: snAlertBanner.hide() }
    }

    MouseArea { anchors.fill: parent; z: 1; visible: backend.pageIndex === 0 && (backend.rtspState || 0) !== 2; onClicked: backend.nextSlide(); propagateComposedEvents: true }

    Item { anchors.fill: parent; focus: true; z: -1
        Keys.onPressed: {
            if (event.key === Qt.Key_Right) backend.nextSlide()
            else if (event.key === Qt.Key_Left) backend.prevSlide()
            else if (event.key === Qt.Key_Home) backend.firstSlide()
            else if (event.key === Qt.Key_End) backend.lastSlide()
            else if (event.key === Qt.Key_Space) backend.toggleSlideshow()
            else if (event.key === Qt.Key_S || event.key === Qt.Key_Escape) backend.pageIndex = backend.pageIndex === 1 ? 0 : 1
            else if (event.key === Qt.Key_V) {
                if ((backend.rtspState || 0) !== 0) backend.stopRtsp()
                else { backend.pageIndex = 0; if (backend.useRtsp && backend.rtspUrl.length > 0) backend.reconnectRtsp() }
            }
            // else if (event.key === Qt.Key_T) backend.pageIndex = backend.pageIndex === 2 ? 0 : 2  // Tasks hidden
            else if (event.key === Qt.Key_C) backend.pageIndex = backend.pageIndex === 3 ? 0 : 3
            else if (event.key === Qt.Key_Q) Qt.quit()
        }
    }
}
