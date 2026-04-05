import QtQuick 1.0
import Dee 3.0

ListView {
    width: 200
    height: 200
    delegate: Text {
        x: 66
        y: 93
        text: column_4
    }
    model: DeeListModel {
        name: "com.canonical.Unity.Lens.applications.T1313498309.Results"
    }
}
