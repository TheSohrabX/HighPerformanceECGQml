import QtQuick

Item {
    id: page
    property Component contentItem: null

    Loader{
        id: loader
        active: false
        asynchronous: true
        anchors.fill: parent
        sourceComponent: page.contentItem
    }

    Component.onCompleted: loader.active = true
}
