# 🎮 AI Rock Paper Scissors (Multiplayer)

A real-time multiplayer **Rock Paper Scissors** game powered by **Computer Vision** and **Artificial Intelligence**.
This project uses **OpenCV** and a Deep Neural Network (MediaPipe ONNX) to detect hand gestures via webcam and plays against an opponent over the network.

![C++](https://img.shields.io/badge/C++-17-blue.svg)
![Qt](https://img.shields.io/badge/Qt-6.0%2B-green.svg)
![OpenCV](https://img.shields.io/badge/OpenCV-4.x-red.svg)
![License](https://img.shields.io/badge/License-MIT-yellow.svg)

## ✨ Features

- **🤖 AI Hand Detection:** Uses a pre-trained ONNX model to recognize "Rock", "Paper", or "Scissors" gestures in real-time.
- **🌐 Multiplayer:** Client-Server architecture allows two players to connect and play remotely.
- **⚡ Real-time Performance:** Optimized with C++ and OpenCV for smooth video processing.
- **🖥️ Modern UI:** Built with Qt Widgets for a clean and responsive interface.

## 🚀 Download & Play (Easy Way)

Don't want to build from source? Download the latest ready-to-play version for Windows:

1.  Go to the [**Releases Page**](../../releases).
2.  Download the `.zip` file (e.g., `RockPaperScissors_v1.0.zip`).
3.  Extract the zip file.
4.  Run `Server_Rock_Paper_Scissors.exe` (to start the host).
5.  Run `Client_Rock_Paper_Scissors.exe` (to start the player).

---

## 🛠️ Build from Source (For Developers)

If you want to modify the code or build it yourself, follow these steps.

### Prerequisites
* **C++ Compiler** (MSVC 2019+ recommended for Windows)
* **Qt 6** (Widgets & Network modules)
* **OpenCV 4** (Pre-built binaries recommended)
* **CMake**

### Installation Steps

1.  **Clone the repository:**
    ```bash
    git clone [https://github.com/KermaniMo/Rock_Paper_Scissors.git](https://github.com/KermaniMo/Rock_Paper_Scissors.git)
    cd Rock_Paper_Scissors
    ```

2.  **Build the Project:**
    You can open the project in **Qt Creator** (open `CMakeLists.txt`) or use the command line.

    **Note:** You need to set the `OPENCV_DIR` in `CMakeLists.txt` or as an environment variable if CMake cannot find your OpenCV installation.

    ```bash
    mkdir build
    cd build
    cmake ..
    cmake --build . --config Release
    ```

3.  **Run:**
    Ensure the `hand_landmark_sparse_Nx3x224x224.onnx` file is in the same directory as the Client executable (CMake should copy it automatically).

## 📂 Project Structure

* `Client_Rock_Paper_Scissors/`: Contains the game client, UI, and AI logic.
* `Server_Rock_Paper_Scissors/`: Contains the server logic for handling connections and game state.

## 🤝 Contributing

Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

## 📜 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---
*Created by [KermaniMo](https://github.com/KermaniMo)*
