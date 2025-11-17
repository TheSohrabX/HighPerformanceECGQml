import QtQuick
import QtGraphs

Item {
    GraphsView {
        id: graphView
        anchors.fill: parent
        theme: GraphsTheme { colorScheme: GraphsTheme.ColorScheme.Dark }

        axisX: ValueAxis {
            id: xAxis
            max: 100
        }
        axisY: ValueAxis {
            id: yAxis
            max: 5
        }

        Component.onCompleted: graphView.seriesList.push(eggSeries)
    }
}
