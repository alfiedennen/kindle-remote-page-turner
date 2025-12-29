# Kindle Remote Page Turner

A WiFi-based remote page turner for Kindle that works with the **stock Kindle reader** (Amazon DRM books). No need to use KOReader for reading - this works with the native Kindle app.

## What You Get

- **Hardware remote** using M5StickC PLUS2 (~$20)
- **Single button control**: tap for next page, double-tap for previous
- **Works with DRM books** from Amazon
- **No modifications** to how you read - just adds remote control

## How It Works

```
[M5StickC PLUS2] --WiFi--> [Kindle HTTP Server] --Touch Injection--> [Page Turn]
```

1. Kindle runs a small HTTP server that listens for page turn commands
2. M5StickC PLUS2 sends HTTP requests when you press the button
3. Server injects touch events to turn pages in the stock Kindle reader

## Requirements

### Kindle
- Kindle with firmware **5.18.0 or earlier** (check: Settings > Device Info)
- WiFi connection

### Hardware Remote
- **M5StickC PLUS2** - [The Pi Hut UK](https://thepihut.com/products/m5stickc-plus2-esp32-mini-iot-development-kit) (~£24) or [M5Stack Store](https://shop.m5stack.com/products/m5stickc-plus2-esp32-mini-iot-development-kit)
- USB-C cable for initial programming

### For Setup
- Windows/Mac/Linux PC
- USB cable to connect Kindle

## Quick Start

### 1. Jailbreak Your Kindle
Follow the [Jailbreak Guide](docs/JAILBREAK.md) using WinterBreak.

### 2. Set Up Kindle Server
Follow the [Kindle Setup Guide](docs/KINDLE-SETUP.md) to install KOReader and the page turner server.

### 3. Program the Remote
Follow the [Hardware Remote Guide](docs/HARDWARE-REMOTE.md) to flash the M5StickC PLUS2.

### 4. Use It!

**Starting the server** (each time you want to use the remote):
1. Open KOReader on Kindle
2. Enable SSH (Tools > SSH server > ON)
3. SSH in and run:
```bash
ssh -p 2222 root@<KINDLE_IP>
cd /mnt/us/koreader && ./luajit /mnt/us/pageserver.lua &
iptables -I INPUT 1 -p tcp --dport 8888 -j ACCEPT
```

**Using the remote**:
- **Single tap** (front button) = Next page
- **Double tap** (front button) = Previous page

## Project Structure

```
kindle-remote-page-turner/
├── kindle/                  # Scripts for Kindle
│   ├── pageserver.lua      # HTTP server
│   └── tap.lua             # Touch injection helper
├── m5stick-plus2/          # M5StickC PLUS2 firmware
│   └── KindlePageTurner/
│       └── KindlePageTurner.ino
└── docs/                   # Setup guides
    ├── JAILBREAK.md
    ├── KINDLE-SETUP.md
    └── HARDWARE-REMOTE.md
```

## Tested On

- Kindle Paperwhite (firmware 5.16.2.1.1)
- M5StickC PLUS2

## Troubleshooting

See the [troubleshooting section](docs/KINDLE-SETUP.md#troubleshooting) for common issues.

## Contributing

Contributions welcome! Please test on your Kindle model and submit PRs with any fixes or improvements.

## License

MIT License - see [LICENSE](LICENSE)

## Acknowledgments

- [WinterBreak](https://kindlemodding.org/jailbreaking/WinterBreak/) jailbreak
- [KOReader](https://koreader.rocks/) for Lua runtime and SSH
- [MobileRead Forums](https://www.mobileread.com/forums/) community
