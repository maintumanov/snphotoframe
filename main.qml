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

    Connections {
        target: backend
        onImageChanged: {
            if (showingA) {
                imageB.source = backend.currentImagePath
                animB.start()
            } else {
                imageA.source = backend.currentImagePath
                animA.start()
            }
            showingA = !showingA
        }
        onSleepChanged: sleepOverlay.visible = sleeping
        onRtspStarted: {
            rtspPlayer.source = url
            rtspPlayer.play()
        }
        onRtspStopped: {
            rtspPlayer.stop()
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        StackLayout {
            id: pageStack
            currentIndex: backend.pageIndex
            Layout.fillWidth: true
            Layout.fillHeight: true

            // PAGE 0: Photo
            Item {
                id: photoPage

                Image {
                    id: imageA
                    anchors.fill: parent
                    fillMode: Image.PreserveAspectFit
                    smooth: true
                    opacity: 1.0
                    Behavior on opacity {
                        NumberAnimation {
                            id: animA
                            duration: 1500
                            from: 0.0
                            to: 1.0
                            easing.type: Easing.InOutQuad
                        }
                    }
                }

                Image {
                    id: imageB
                    anchors.fill: parent
                    fillMode: Image.PreserveAspectFit
                    smooth: true
                    opacity: 0.0
                    Behavior on opacity {
                        NumberAnimation {
                            id: animB
                            duration: 1500
                            from: 0.0
                            to: 1.0
                            easing.type: Easing.InOutQuad
                        }
                    }
                }

                MediaPlayer {
                    id: rtspPlayer
                    autoPlay: false
                }
                VideoOutput {
                    id: rtspOutput
                    anchors.fill: parent
                    source: rtspPlayer
                    visible: rtspPlayer.playbackState === MediaPlayer.PlayingState
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: backend.nextSlide()
                }

                Rectangle {
                    id: sleepOverlay
                    anchors.fill: parent
                    color: "black"
                    visible: false
                    z: 100
                }
            }

            // PAGE 1: Settings
            Item {
                id: settingsPage

                Flickable {
                    anchors.fill: parent
                    contentHeight: settingsColumn.height + 40
                    clip: true

                    Column {
                        id: settingsColumn
                        width: parent.width - 100
                        x: 50
                        y: 20
                        spacing: 0

                        Text {
                            text: "\u041d\u0430\u0441\u0442\u0440\u043e\u0439\u043a\u0438"
                            color: "white"
                            font.pixelSize: 36
                            font.bold: true
                            bottomPadding: 20
                        }

                        Text {
                            text: "\u0421\u0435\u0442\u044c"
                            color: "#aaa"
                            font.pixelSize: 22
                            font.bold: true
                            topPadding: 10
                            bottomPadding: 8
                        }

                        Row {
                            spacing: 16
                            width: parent.width
                            Text {
                                text: "\u0421\u0435\u0440\u0432\u0435\u0440:"
                                color: "white"
                                font.pixelSize: 20
                                width: 160
                                anchors.verticalCenter: parent.verticalCenter
                            }
                            TextField {
                                id: srvField
                                text: backend.server
                                onTextChanged: backend.server = text
                                width: parent.width - 176
                                color: "white"
                                font.pixelSize: 20
                                padding: 10
                                background: Rectangle {
                                    radius: 8
                                    color: "#222"
                                    border.color: srvField.activeFocus ? "#aaa" : "#555"
                                }
                            }
                        }

                        Row {
                            spacing: 16
                            width: parent.width
                            topPadding: 8
                            Text {
                                text: "\u041f\u0430\u043f\u043a\u0430:"
                                color: "white"
                                font.pixelSize: 20
                                width: 160
                                anchors.verticalCenter: parent.verticalCenter
                            }
                            TextField {
                                id: shrField
                                text: backend.share
                                onTextChanged: backend.share = text
                                width: parent.width - 176
                                color: "white"
                                font.pixelSize: 20
                                padding: 10
                                background: Rectangle {
                                    radius: 8
                                    color: "#222"
                                    border.color: shrField.activeFocus ? "#aaa" : "#555"
                                }
                            }
                        }

                        Row {
                            spacing: 16
                            width: parent.width
                            topPadding: 8
                            Text {
                                text: "\u041b\u043e\u0433\u0438\u043d:"
                                color: "white"
                                font.pixelSize: 20
                                width: 160
                                anchors.verticalCenter: parent.verticalCenter
                            }
                            TextField {
                                id: usrField
                                text: backend.user
                                onTextChanged: backend.user = text
                                width: parent.width - 176
                                color: "white"
                                font.pixelSize: 20
                                padding: 10
                                background: Rectangle {
                                    radius: 8
                                    color: "#222"
                                    border.color: usrField.activeFocus ? "#aaa" : "#555"
                                }
                            }
                        }

                        Row {
                            spacing: 16
                            width: parent.width
                            topPadding: 8
                            Text {
                                text: "\u041f\u0430\u0440\u043e\u043b\u044c:"
                                color: "white"
                                font.pixelSize: 20
                                width: 160
                                anchors.verticalCenter: parent.verticalCenter
                            }
                            TextField {
                                id: pswField
                                text: backend.pass
                                onTextChanged: backend.pass = text
                                echoMode: TextInput.Password
                                width: parent.width - 176
                                color: "white"
                                font.pixelSize: 20
                                padding: 10
                                background: Rectangle {
                                    radius: 8
                                    color: "#222"
                                    border.color: pswField.activeFocus ? "#aaa" : "#555"
                                }
                            }
                        }

                        Row {
                            spacing: 16
                            width: parent.width
                            topPadding: 8
                            Text {
                                text: "\u0418\u043d\u0442\u0435\u0440\u0432\u0430\u043b (\u0441\u0435\u043a):"
                                color: "white"
                                font.pixelSize: 20
                                width: 160
                                anchors.verticalCenter: parent.verticalCenter
                            }
                            SpinBox {
                                id: intervalSpin
                                from: 2
                                to: 3600
                                value: backend.interval
                                onValueModified: backend.interval = value
                                width: 200
                                background: Rectangle {
                                    radius: 8
                                    color: "#222"
                                    border.color: "#555"
                                }
                                contentItem: TextInput {
                                    text: intervalSpin.value
                                    color: "white"
                                    font.pixelSize: 20
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                }
                            }
                        }

                        Row {
                            spacing: 12
                            width: parent.width
                            topPadding: 8
                            CheckBox {
                                id: shuffleChk
                                text: "\u0421\u043b\u0443\u0447\u0430\u0439\u043d\u044b\u0439 \u043f\u043e\u0440\u044f\u0434\u043e\u043a"
                                checked: backend.shuffle
                                onCheckedChanged: backend.shuffle = checked
                                contentItem: Text {
                                    text: shuffleChk.text
                                    color: "white"
                                    font.pixelSize: 20
                                    leftPadding: shuffleChk.indicator.width + 8
                                    verticalAlignment: Text.AlignVCenter
                                }
                                indicator: Rectangle {
                                    width: 24
                                    height: 24
                                    radius: 6
                                    color: shuffleChk.checked ? "#5a5" : "#222"
                                    border.color: "#888"
                                    anchors.verticalCenter: parent.verticalCenter
                                }
                            }
                        }

                        Text {
                            text: "\u0420\u0430\u0441\u043f\u0438\u0441\u0430\u043d\u0438\u0435"
                            color: "#aaa"
                            font.pixelSize: 22
                            font.bold: true
                            topPadding: 20
                            bottomPadding: 8
                        }

                        Row {
                            spacing: 12
                            width: parent.width
                            CheckBox {
                                id: schedChk
                                text: "\u0412\u043a\u043b\u044e\u0447\u0438\u0442\u044c \u0441\u043f\u044f\u0449\u0438\u0439 \u0440\u0435\u0436\u0438\u043c"
                                checked: backend.useSchedule
                                onCheckedChanged: backend.useSchedule = checked
                                contentItem: Text {
                                    text: schedChk.text
                                    color: "white"
                                    font.pixelSize: 20
                                    leftPadding: schedChk.indicator.width + 8
                                    verticalAlignment: Text.AlignVCenter
                                }
                                indicator: Rectangle {
                                    width: 24
                                    height: 24
                                    radius: 6
                                    color: schedChk.checked ? "#5a5" : "#222"
                                    border.color: "#888"
                                    anchors.verticalCenter: parent.verticalCenter
                                }
                            }
                        }

                        Row {
                            spacing: 16
                            width: parent.width
                            topPadding: 8
                            Text {
                                text: "\u041f\u0440\u043e\u0431\u0443\u0436\u0434\u0435\u043d\u0438\u0435:"
                                color: "white"
                                font.pixelSize: 20
                                width: 160
                                anchors.verticalCenter: parent.verticalCenter
                            }
                            TextField {
                                id: wakeField
                                text: backend.wakeTime
                                onEditingFinished: backend.wakeTime = text
                                width: 120
                                color: "white"
                                font.pixelSize: 20
                                padding: 10
                                inputMethodHints: Qt.ImhTime
                                background: Rectangle {
                                    radius: 8
                                    color: "#222"
                                    border.color: "#555"
                                }
                            }
                        }

                        Row {
                            spacing: 16
                            width: parent.width
                            topPadding: 8
                            Text {
                                text: "\u0421\u043d:"
                                color: "white"
                                font.pixelSize: 20
                                width: 160
                                anchors.verticalCenter: parent.verticalCenter
                            }
                            TextField {
                                id: sleepField
                                text: backend.sleepTime
                                onEditingFinished: backend.sleepTime = text
                                width: 120
                                color: "white"
                                font.pixelSize: 20
                                padding: 10
                                inputMethodHints: Qt.ImhTime
                                background: Rectangle {
                                    radius: 8
                                    color: "#222"
                                    border.color: "#555"
                                }
                            }
                        }

                        Text {
                            text: "RTSP"
                            color: "#aaa"
                            font.pixelSize: 22
                            font.bold: true
                            topPadding: 20
                            bottomPadding: 8
                        }

                        Row {
                            spacing: 12
                            width: parent.width
                            CheckBox {
                                id: rtspChk
                                text: "\u041f\u043e\u043a\u0430\u0437\u044b\u0432\u0430\u0442\u044c \u0432\u0438\u0434\u0435\u043e \u0441 RTSP \u043a\u0430\u043c\u0435\u0440\u044b"
                                checked: backend.useRtsp
                                onCheckedChanged: backend.useRtsp = checked
                                contentItem: Text {
                                    text: rtspChk.text
                                    color: "white"
                                    font.pixelSize: 20
                                    leftPadding: rtspChk.indicator.width + 8
                                    verticalAlignment: Text.AlignVCenter
                                }
                                indicator: Rectangle {
                                    width: 24
                                    height: 24
                                    radius: 6
                                    color: rtspChk.checked ? "#5a5" : "#222"
                                    border.color: "#888"
                                    anchors.verticalCenter: parent.verticalCenter
                                }
                            }
                        }

                        Row {
                            spacing: 16
                            width: parent.width
                            topPadding: 8
                            Text {
                                text: "URL:"
                                color: "white"
                                font.pixelSize: 20
                                width: 160
                                anchors.verticalCenter: parent.verticalCenter
                            }
                            TextField {
                                id: rtspField
                                text: backend.rtspUrl
                                onTextChanged: backend.rtspUrl = text
                                placeholderText: "rtsp://user:pass@ip:port/stream"
                                width: parent.width - 176
                                color: "white"
                                font.pixelSize: 20
                                padding: 10
                                background: Rectangle {
                                    radius: 8
                                    color: "#222"
                                    border.color: rtspField.activeFocus ? "#aaa" : "#555"
                                }
                            }
                        }

                        Item {
                            width: 1
                            height: 30
                        }

                        Row {
                            spacing: 20
                            anchors.horizontalCenter: parent.horizontalCenter
                            bottomPadding: 30

                            Rectangle {
                                width: 160
                                height: 56
                                radius: 14
                                color: exitMa.pressed ? "#666" : exitMa.containsMouse ? "#444" : "transparent"
                                border.color: "white"
                                border.width: 2
                                Text {
                                    text: "\u0412\u044b\u0445\u043e\u0434"
                                    color: "white"
                                    font.pixelSize: 22
                                    font.bold: true
                                    anchors.centerIn: parent
                                }
                                MouseArea {
                                    id: exitMa
                                    anchors.fill: parent
                                    hoverEnabled: true
                                    onClicked: backend.pageIndex = 0
                                }
                            }

                            Rectangle {
                                width: 160
                                height: 56
                                radius: 14
                                color: saveMa.pressed ? "#666" : saveMa.containsMouse ? "#444" : "transparent"
                                border.color: "white"
                                border.width: 2
                                Text {
                                    text: "\u0421\u043e\u0445\u0440\u0430\u043d\u0438\u0442\u044c"
                                    color: "white"
                                    font.pixelSize: 22
                                    font.bold: true
                                    anchors.centerIn: parent
                                }
                                MouseArea {
                                    id: saveMa
                                    anchors.fill: parent
                                    hoverEnabled: true
                                    onClicked: backend.saveSettings()
                                }
                            }
                        }
                    }
                }
            }

            // PAGE 2: Tasks
            Item {
                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 50
                    spacing: 20

                    Text {
                        text: "\u0422\u0435\u043a\u0443\u0449\u0438\u0435 \u0437\u0430\u0434\u0430\u0447\u0438"
                        color: "white"
                        font.pixelSize: 36
                        font.bold: true
                    }

                    ListView {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        model: backend.tasks()
                        clip: true
                        delegate: Rectangle {
                            width: ListView.view.width
                            height: 50
                            color: index % 2 === 0 ? "#111" : "#1a1a1a"
                            border.color: "#222"
                            border.width: 1
                            Text {
                                text: modelData
                                color: "white"
                                font.pixelSize: 20
                                anchors.verticalCenter: parent.verticalCenter
                                leftPadding: 16
                            }
                        }
                        Text {
                            anchors.centerIn: parent
                            text: backend.tasks().length === 0 ? "\u041d\u0435\u0442 \u0437\u0430\u0434\u0430\u0447" : ""
                            color: "#666"
                            font.pixelSize: 24
                        }
                    }
                }
            }

            // PAGE 3: Calendar
            Item {
                id: calendarPage

                property var today: new Date()
                property int displayYear: today.getFullYear()
                property int displayMonth: today.getMonth()
                property int daysInMonth: new Date(displayYear, displayMonth + 1, 0).getDate()
                property int firstDayOfWeek: new Date(displayYear, displayMonth, 1).getDay()

                function monthName(m) {
                    var names = ["\u042f\u043d\u0432\u0430\u0440\u044c","\u0424\u0435\u0432\u0440\u0430\u043b\u044c","\u041c\u0430\u0440\u0442","\u0410\u043f\u0440\u0435\u043b\u044c","\u041c\u0430\u0439","\u0418\u044e\u043d\u044c","\u0418\u044e\u043b\u044c","\u0410\u0432\u0433\u0443\u0441\u0442","\u0421\u0435\u043d\u0442\u044f\u0431\u0440\u044c","\u041e\u043a\u0442\u044f\u0431\u0440\u044c","\u041d\u043e\u044f\u0431\u0440\u044c","\u0414\u0435\u043a\u0430\u0431\u0440\u044c"]
                    return names[m]
                }

                function dayName(d) {
                    var names = ["\u0412\u0441","\u041f\u043d","\u0412\u0442","\u0421\u0440","\u0427\u0442","\u041f\u0442","\u0421\u0431"]
                    return names[d]
                }

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 50
                    spacing: 20

                    Text {
                        text: "\u041a\u0430\u043b\u0435\u043d\u0434\u0430\u0440\u044c"
                        color: "white"
                        font.pixelSize: 36
                        font.bold: true
                    }

                    RowLayout {
                        width: parent.width
                        spacing: 20

                        Rectangle {
                            width: 56; height: 48; radius: 14
                            color: prevMa.pressed ? "#444" : "#222"
                            border.color: "#888"; border.width: 1
                            Text { text: "\u25c0"; color: "white"; font.pixelSize: 20; anchors.centerIn: parent }
                            MouseArea { id: prevMa; anchors.fill: parent; onClicked: {
                                if (calendarPage.displayMonth === 0) {
                                    calendarPage.displayMonth = 11
                                    calendarPage.displayYear--
                                } else {
                                    calendarPage.displayMonth--
                                }
                            }}
                        }

                        Text {
                            text: calendarPage.monthName(calendarPage.displayMonth) + " " + calendarPage.displayYear
                            color: "white"
                            font.pixelSize: 24
                            font.bold: true
                            Layout.fillWidth: true
                            horizontalAlignment: Text.AlignHCenter
                        }

                        Rectangle {
                            width: 56; height: 48; radius: 14
                            color: nextMa.pressed ? "#444" : "#222"
                            border.color: "#888"; border.width: 1
                            Text { text: "\u25b6"; color: "white"; font.pixelSize: 20; anchors.centerIn: parent }
                            MouseArea { id: nextMa; anchors.fill: parent; onClicked: {
                                if (calendarPage.displayMonth === 11) {
                                    calendarPage.displayMonth = 0
                                    calendarPage.displayYear++
                                } else {
                                    calendarPage.displayMonth++
                                }
                            }}
                        }
                    }

                    GridLayout {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        columns: 7
                        columnSpacing: 2
                        rowSpacing: 2

                        Repeater {
                            model: 7
                            Rectangle {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 36
                                color: "#222"
                                Text {
                                    text: calendarPage.dayName(index)
                                    color: "#aaa"
                                    font.pixelSize: 16
                                    font.bold: true
                                    anchors.centerIn: parent
                                }
                            }
                        }

                        Repeater {
                            model: calendarPage.firstDayOfWeek
                            Item { Layout.fillWidth: true; Layout.preferredHeight: 1 }
                        }

                        Repeater {
                            model: calendarPage.daysInMonth
                            Rectangle {
                                property int dayNum: index + 1
                                Layout.fillWidth: true
                                Layout.preferredHeight: 48
                                color: (calendarPage.today.getDate() === dayNum && calendarPage.today.getMonth() === calendarPage.displayMonth && calendarPage.today.getFullYear() === calendarPage.displayYear) ? "#5af" : "#1a1a1a"
                                border.color: "#333"
                                border.width: 1
                                radius: 4
                                Text {
                                    text: dayNum
                                    color: "white"
                                    font.pixelSize: 18
                                    anchors.centerIn: parent
                                }
                            }
                        }
                    }
                }
            }
        }

        // ---- CONTROL BAR ----
        Rectangle {
            id: controlBar
            Layout.fillWidth: true
            Layout.preferredHeight: 100
            color: "transparent"
            visible: !sleepOverlay.visible

            Row {
                anchors.centerIn: parent
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 40
                spacing: 16

                Rectangle {
                    width: 84
                    height: 56
                    radius: 14
                    color: "#4d000000"
                    border.color: "white"
                    border.width: 2
                    Text {
                        text: backend.currentTime
                        color: "white"
                        font.pixelSize: 24
                        font.bold: true
                        font.family: "Consolas, monospace"
                        anchors.centerIn: parent
                    }
                }

                Rectangle {
                    width: 84
                    height: 56
                    radius: 14
                    color: "#4d000000"
                    border.color: "white"
                    border.width: 2
                    Text {
                        text: backend.currentDate
                        color: "white"
                        font.pixelSize: 18
                        font.bold: true
                        font.family: "Consolas, monospace"
                        anchors.centerIn: parent
                    }
                }

                Rectangle {
                    width: 56
                    height: 56
                    radius: 14
                    color: backend.pageIndex === 0 ? "#4dffffff" : (videoMa.pressed ? "#555" : "#4d000000")
                    border.color: "white"
                    border.width: 2
                    Text {
                        text: "\u25b6"
                        color: "white"
                        font.pixelSize: 24
                        anchors.centerIn: parent
                    }
                    MouseArea {
                        id: videoMa
                        anchors.fill: parent
                        onClicked: backend.pageIndex = 0
                    }
                }

                Rectangle {
                    width: 56
                    height: 56
                    radius: 14
                    color: backend.pageIndex === 1 ? "#4dffffff" : (settingsMa.pressed ? "#555" : "#4d000000")
                    border.color: "white"
                    border.width: 2
                    Text {
                        text: "\u2261"
                        color: "white"
                        font.pixelSize: 28
                        font.bold: true
                        anchors.centerIn: parent
                    }
                    MouseArea {
                        id: settingsMa
                        anchors.fill: parent
                        onClicked: backend.pageIndex = backend.pageIndex === 1 ? 0 : 1
                    }
                }

                Rectangle {
                    width: 56
                    height: 56
                    radius: 14
                    color: backend.pageIndex === 2 ? "#4dffffff" : (tasksMa.pressed ? "#555" : "#4d000000")
                    border.color: "white"
                    border.width: 2
                    Text {
                        text: "\u2713"
                        color: "white"
                        font.pixelSize: 24
                        font.bold: true
                        anchors.centerIn: parent
                    }
                    MouseArea {
                        id: tasksMa
                        anchors.fill: parent
                        onClicked: backend.pageIndex = backend.pageIndex === 2 ? 0 : 2
                    }
                }

                Rectangle {
                    width: 56
                    height: 56
                    radius: 14
                    color: backend.pageIndex === 3 ? "#4dffffff" : (calMa.pressed ? "#555" : "#4d000000")
                    border.color: "white"
                    border.width: 2
                    Text {
                        text: "\u2605"
                        color: "white"
                        font.pixelSize: 22
                        anchors.centerIn: parent
                    }
                    MouseArea {
                        id: calMa
                        anchors.fill: parent
                        onClicked: backend.pageIndex = backend.pageIndex === 3 ? 0 : 3
                    }
                }
            }
        }
    }

    Item {
        anchors.fill: parent
        focus: true
        Keys.onPressed: {
            if (event.key === Qt.Key_Right) backend.nextSlide()
            else if (event.key === Qt.Key_Left) backend.prevSlide()
            else if (event.key === Qt.Key_Space) backend.toggleSlideshow()
            else if (event.key === Qt.Key_S || event.key === Qt.Key_Escape) backend.pageIndex = backend.pageIndex === 1 ? 0 : 1
            else if (event.key === Qt.Key_V) backend.pageIndex = 0
            else if (event.key === Qt.Key_T) backend.pageIndex = backend.pageIndex === 2 ? 0 : 2
            else if (event.key === Qt.Key_C) backend.pageIndex = backend.pageIndex === 3 ? 0 : 3
            else if (event.key === Qt.Key_Q) Qt.quit()
        }
    }
}
