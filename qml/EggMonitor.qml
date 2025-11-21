import QtQuick
import QtGraphs
import Egg

Item {

    EggModel{
        id: model
        sensor: EggSensor {
            mode: EggSensor.HeartAttack
            sampleRate: 1000
        }
    }

    GraphsView {
        id: graphView
        anchors.fill: parent

        theme: GraphsTheme { colorScheme: GraphsTheme.ColorScheme.Dark }

        axisX: ValueAxis {
            id: xAxis
            max: 2000
            min: 0
        }

        axisY: ValueAxis {
            id: yAxis
            min: -1.6
            max: 1.6
        }

        LineSeries {
            id: lineSeries
        }

        Component.onCompleted: {
            model.axisX = xAxis;
            model.axisY = yAxis;
            model.series = lineSeries
            model.start();
        }
    }
}
