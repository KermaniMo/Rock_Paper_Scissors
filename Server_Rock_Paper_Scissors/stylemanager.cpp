#include "stylemanager.h"

QString StyleManager::darkTheme()
{
    return R"(
        /* Main Window & Central Widget */
        QMainWindow, QWidget#centralwidget {
            background-color: #2B2B2B;
        }

        /* Labels */
        QLabel {
            color: white;
            font-family: 'Segoe UI', Arial;
            font-size: 14px;
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

        /* QPushButton */
        QPushButton {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #4A4D53, stop:1 #2F3136);
            border: 1px solid #1E1E1E;
            border-radius: 6px;
            color: white;
            padding: 8px 20px;
            font-size: 14px;
            font-weight: bold;
        }
        QPushButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #5A5D63, stop:1 #3F4146);
        }
        QPushButton:pressed {
            background: #202225;
        }

        /* QDockWidget */
        QDockWidget {
            color: white;
            border: 1px solid #3A3A3A;
        }
        QDockWidget::title {
            background: #1A1A1A;
            text-align: left;
            padding: 8px;
            border-radius: 4px;
        }
        QDockWidget::close-button, QDockWidget::float-button {
            background: transparent;
            padding: 0px;
        }
        QDockWidget::close-button:hover, QDockWidget::float-button:hover {
            background: #3A3A3A;
            border-radius: 3px;
        }

        /* QListWidget */
        QListWidget {
            background-color: #1E1E1E;
            color: #E0E0E0;
            border: 1px solid #3A3A3A;
            border-radius: 4px;
            padding: 5px;
            font-size: 14px;
        }
        QListWidget::item {
            padding: 5px;
            border-bottom: 1px solid #2A2A2A;
        }
        QListWidget::item:selected {
            background-color: #7B68EE;
            color: white;
            border-radius: 3px;
        }
        QListWidget::item:hover {
            background-color: #3A3A3A;
        }

        /* Scrollbars */
        QScrollBar:vertical {
            border: none;
            background: #2B2B2B;
            width: 10px;
            margin: 0px 0px 0px 0px;
        }
        QScrollBar::handle:vertical {
            background: #555;
            min-height: 20px;
            border-radius: 5px;
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0px;
        }

        /* StatusBar */
        QStatusBar {
            background-color: #1A1A1A;
            color: #AAAAAA;
        }
    )";
}
