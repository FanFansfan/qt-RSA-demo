import QtQuick 2.11
import QtQuick.Window 2.11
import QtQuick.Controls 2.3
import QtQuick.Dialogs.qml 1.0

Window {
    visible: true
    width: 830
    height: 480
    title: qsTr("RSA Demo")

    Button {
        id: genKey
        x: 36
        y: 399
        text: qsTr(" Gen Key")
        autoExclusive: false
        onClicked: rsa.genKey()
    }

    Text {
        id: textP
        x: 36
        y: 34
        width: 609
        height: 15
        text: rsa.p == "0" ? "" : rsa.p.toLocaleUpperCase()
        elide: Text.ElideNone
        wrapMode: Text.WrapAnywhere
        renderType: Text.QtRendering
        font.pixelSize: 12
    }

    Label {
        id: labelP
        x: 10
        y: 32
        text: qsTr("p:")
    }

    Label {
        id: labelQ
        x: 10
        y: 69
        text: qsTr("q:")
    }

    Text {
        id: textQ
        x: 36
        y: 71
        width: 609
        height: 15
        text: rsa.q == "0" ? "" : rsa.q.toLocaleUpperCase()
        wrapMode: Text.WrapAnywhere
        elide: Text.ElideNone
        font.pixelSize: 12
    }

    Label {
        id: labelCip
        x: 11
        y: 230
        width: 81
        height: 20
        text: qsTr("Encrypt:")
    }

    Label {
        id: labelEncode
        x: 11
        y: 183
        width: 75
        height: 25
        text: qsTr("Encode:")
    }

    Label {
        id: label
        x: 12
        y: 318
        text: qsTr("Decode:")
    }

    Label {
        id: label2
        x: 11
        y: 136
        text: qsTr("Plaintext:")
    }

    Label {
        id: label3
        x: 11
        y: 275
        width: 58
        height: 19
        text: qsTr("Decrypt:")
    }

    Button {
        id: buttonEncry
        x: 363
        y: 399
        text: qsTr("Encry")
        onClicked: {
            textEncode.text = rsa.encode(textEditPlain.text)
            textEncrypt.text = rsa.encrypt(textEncode.text)
        }
    }

    Button {
        id: buttonDecry
        x: 690
        y: 399
        text: qsTr("Decry")
        onClicked: {
            textDecrypt.text = rsa.decrypt(textEncrypt.text)
            textDecode.text = rsa.decode(textDecrypt.text)
        }
    }

    ScrollView {
        id: plainTextSV
        x: 111
        y: 136
        width: 550
        height: 28
        background: Rectangle {
            anchors.fill: parent
            border.color: "gray"
        }

        TextArea {
            id: textEditPlain
            text: qsTr("Hello world!")
            wrapMode: Text.WrapAnywhere
            selectByMouse:  true
            //autoScroll: true
            topPadding: 1
            leftPadding: 5
            font.pixelSize: 12
        }
    }

    ScrollView {
        x: 111
        y: 179
        width: 550
        height: 30

        TextArea {
            id: textEncode
            text: qsTr("")
            wrapMode: Text.WrapAnywhere
            font.pixelSize: 12
        }
    }

    ScrollView {
        id: textEncryptSV
        x: 111
        y: 225
        width: 550
        height: 30

        TextArea {
            id: textEncrypt
            text: qsTr("")
            wrapMode: Text.WrapAnywhere
            font.pixelSize: 12
        }
    }

    ScrollView {
        id: textDecryptSV
        x: 111
        y: 271
        width: 550
        height: 30

        TextArea {
            id: textDecrypt

            text: qsTr("")
            wrapMode: Text.WrapAnywhere
            font.pixelSize: 12
        }

    }

    ScrollView {
        id: textDecodeSV
        x: 111
        y: 316
        width: 550
        height: 30

        TextArea {
            id: textDecode

            text: qsTr("")
            wrapMode: Text.WrapAnywhere
            font.pixelSize: 12
        }
    }
}
