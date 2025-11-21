import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {

    RowLayout {
        id: buttons
        anchors.fill: parent
        anchors.margins: 10
        spacing: 10

        Button {
            id: eggMonitorBtn
            text: "ECG Monitor"
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.preferredWidth: buttons.width / 2
            onClicked: stackView.push(eggMonitorComponent)
        }

        Button {
            id: voiceVisualizerBtn
            text: "Audio Visualizer"
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.preferredWidth: buttons.width / 2
            onClicked: stackView.push(voiceVisualizerComponent)
        }
    }

}
