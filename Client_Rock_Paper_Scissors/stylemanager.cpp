#include "stylemanager.h"

QString StyleManager::darkTheme()
{
    return R"(
        QMainWindow {
            background-color: #2B2B2B;
        }
        QWidget#centralwidget {
            background-color: #2B2B2B;
        }
        /* LoginForm Window */
        QWidget#LoginForm {
            background-color: #2B2B2B;
        }
        QLabel {
            color: white;
            font-family: 'Segoe UI', Arial;
            font-size: 16px;
        }
        #label_title {
            font-size: 24px;
            font-weight: bold;
            color: #E0E0E0;
            padding: 10px;
        }
        #label_player_title {
            font-size: 16px;
            font-weight: bold;
            color: #CCCCCC;
        }
        #label_camera {
            border: 2px solid #4CAF50;
            border-radius: 8px;
            background-color: #1A1A1A;
        }
        #label_result {
            font-size: 32px;
            font-weight: bold;
            color: #FFD700;
            padding: 5px;
        }
        #label_fingers {
            font-size: 16px;
            color: #AAAAAA;
        }
        #label_debug {
            border: 2px solid #00BCD4;
            border-radius: 6px;
            background-color: #1A1A1A;
        }
        QPushButton {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #4A4D53, stop:1 #2F3136);
            border: 1px solid #1E1E1E;
            border-radius: 8px;
            color: white;
            padding: 10px 25px;
            font-size: 14px;
            font-weight: bold;
            min-width: 120px;
        }
        QPushButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #5A5D63, stop:1 #3F4146);
        }
        QPushButton:pressed {
            background: #202225;
        }
        #btn_play {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #7B68EE, stop:1 #5A4FCF);
            border: 1px solid #4A3FBF;
        }
        #btn_play:hover {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #8B78FE, stop:1 #6A5FDF);
        }

        /* QMessageBox Fix */
        QMessageBox {
            background-color: #2B2B2B;
        }
        QMessageBox QLabel {
            color: white;
        }
        QMessageBox QPushButton {
            background-color: #4A4D53;
            color: white;
            border: 1px solid #1E1E1E;
            border-radius: 4px;
            padding: 6px 20px;
        }
        QMessageBox QPushButton:pressed {
            background-color: #3F4146;
        }

        /* QLineEdit (Inputs) */
        QLineEdit {
            background-color: #1A1A1A;
            color: white;
            border: 1px solid #4A4D53;
            border-radius: 4px;
            padding: 6px;
            font-size: 14px;
        }
        QLineEdit:focus {
            border: 1px solid #7B68EE;
        }
    )";
}
