import QtQuick
import QtQuick.Controls

Item {
    StackView {
        id: stackView
        anchors.fill: parent
        initialItem: toolSelectorComponent
    }

    Component {
        id: eggMonitorComponent
        EggMonitor {}
    }

    Component {
        id: voiceVisualizerComponent
        VoiceVisualizer{}
    }

    Component{
        id: toolSelectorComponent
        ToolSelector{}
    }

}
