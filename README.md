# meta-http-server

A custom Yocto Project layer that provides a lightweight C++ HTTP server for controlling hardware LEDs on **STM32MP157D-DK1**.

This project demonstrates how to integrate a custom CMake-based C++ application into a Yocto Linux image, interact with the Linux Sysfs interface for GPIO/LED control, and serve a modern web interface.

---

## 🚀 Features

* **Lightweight Server:** Written in pure C++ using standard Linux sockets (no heavy external web frameworks).
* **Hardware Control:** Toggles board LEDs via the Linux Sysfs interface (`/sys/class/leds`).
* **Web Interface:** Serves a static HTML/JS dashboard with a modern dark mode UI.
* **REST API:** Simple GET endpoints to control hardware state remotely.
* **Yocto Integration:** Fully compatible with OpenEmbedded/Yocto build systems.

## 📂 Project Structure

```text
meta-http-server/
├── conf/
│   └── layer.conf            # Layer configuration
├── recipes-http-server/
│   └── http-server/
│       ├── http-server_1.0.bb  # BitBake recipe
│       └── code/               # C++ Source Code
│           ├── main.cpp
│           ├── server.cpp
│           ├── server.h
│           ├── CMakeLists.txt
│           └── static/         # HTML/CSS/JS assets
```
### Demo
!(recipes-http-server/http-server/code/static/demo.gif)
