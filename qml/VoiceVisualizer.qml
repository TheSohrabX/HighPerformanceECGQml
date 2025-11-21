import QtQuick
import QtGraphs
import AudioViz
import QtQuick.Controls

LazyPage {
    contentItem: Component {
        GraphsView {
            anchors.fill: parent
            theme: GraphsTheme {
                colorScheme: GraphsTheme.ColorScheme.Dark
                seriesColors: ["Red"]
            }

            axisX: ValueAxis  {
                id: xAxis
                labelsVisible: false
            }

            axisY: ValueAxis {
                id: yAxis
                min: 0
                max: 50
            }

            BarSeries {
                id: bars
                barWidth: 0.8
            }

            Component.onCompleted: {
                audioViz.barSeries = bars
                audioViz.axisX = xAxis
                audioViz.axisY = yAxis
            }
        }
    }

    Button {
        text: "Back"
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.margins: 10
        onClicked: {
            audioViz.stop()
            stackView.pop()
        }
    }

    Button {
        property bool running: false

        text: running ? "Stop" : "Start"
        anchors.centerIn: parent
        onClicked: {
            running ? audioViz.stop() : audioViz.start();
            running = !running;
        }
    }

    AudioVisualizer {
        id: audioViz
    }
}
