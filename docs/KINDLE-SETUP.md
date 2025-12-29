# Kindle Setup Guide

This guide covers installing KOReader and the page turner server on your jailbroken Kindle.

## Prerequisites

- Jailbroken Kindle (see [Jailbreak Guide](JAILBREAK.md))
- PC connected to same WiFi as Kindle

## Step 1: Install KUAL (Kindle Unified Application Launcher)

KUAL is a launcher for running homebrew applications on your Kindle.

### Download Required Files

1. **KUAL**: https://www.mobileread.com/forums/showthread.php?t=203326
2. **MRPI (MobileRead Package Installer)**: https://www.mobileread.com/forums/showthread.php?t=225030

### Install KUAL

1. Connect Kindle to PC via USB
2. Extract and copy to Kindle root:
   - `extensions/` folder (contains MRInstaller)
   - `mrpackages/` folder
3. Copy `Update_KUALBooklet_*_install.bin` to Kindle root
4. Safely eject Kindle
5. On Kindle: Open your library and find the **"Run Hotfix"** book - open it first
6. Search in library for: `;log mrpi`
7. KUAL will install and appear in your library

## Step 2: Install KOReader

KOReader provides the Lua runtime and SSH server we need.

### Option A: Via KUAL (Recommended)

1. Open KUAL on your Kindle
2. Go to **Helper > Install MR Packages**
3. Follow prompts to install KOReader

### Option B: Via Web Installer

1. Visit: https://scriptlets.notmarek.com/installer.html
2. Follow the on-screen instructions
3. Select KOReader and install

After installation, KOReader will appear in your library.

## Step 3: Enable SSH Access

1. Open **KOReader** on your Kindle
2. Tap the top menu bar to show settings
3. Go to **Tools** (gear icon) > **More tools** > **SSH server**
4. Enable **"Login without password"** (for initial setup)
5. Turn SSH server **ON**
6. Note the IP address shown (e.g., 192.168.1.100)

## Step 4: Deploy Page Turner Scripts

### Connect via SSH

From your PC terminal:

```bash
ssh -p 2222 root@<KINDLE_IP>
```

Replace `<KINDLE_IP>` with your Kindle's IP address.

### Create the Page Server Script

```bash
cat > /mnt/us/pageserver.lua << 'EOF'
-- Set up KOReader paths
package.path = "/mnt/us/koreader/common/?.lua;/mnt/us/koreader/?.lua;" .. package.path
package.cpath = "/mnt/us/koreader/common/?.so;/mnt/us/koreader/libs/?.so;" .. package.cpath

local ffi = require("ffi")
local socket = require("socket")

ffi.cdef[[
    struct input_event {
        long tv_sec;
        long tv_usec;
        unsigned short type;
        unsigned short code;
        int value;
    };
    int open(const char *pathname, int flags);
    int write(int fd, const void *buf, int count);
    int close(int fd);
]]

local EV_SYN, EV_KEY, EV_ABS = 0, 1, 3
local SYN_REPORT = 0
local BTN_TOUCH = 0x14a
local ABS_MT_TRACKING_ID = 0x39
local ABS_MT_POSITION_X = 0x35
local ABS_MT_POSITION_Y = 0x36

local function send_event(fd, type, code, value)
    local ev = ffi.new("struct input_event")
    ev.tv_sec = 0
    ev.tv_usec = 0
    ev.type = type
    ev.code = code
    ev.value = value
    ffi.C.write(fd, ev, ffi.sizeof(ev))
end

local function tap(x, y)
    local fd = ffi.C.open("/dev/input/event9", 1)
    if fd < 0 then return false end
    send_event(fd, EV_ABS, ABS_MT_TRACKING_ID, 0)
    send_event(fd, EV_ABS, ABS_MT_POSITION_X, x)
    send_event(fd, EV_ABS, ABS_MT_POSITION_Y, y)
    send_event(fd, EV_KEY, BTN_TOUCH, 1)
    send_event(fd, EV_SYN, SYN_REPORT, 0)
    send_event(fd, EV_KEY, BTN_TOUCH, 0)
    send_event(fd, EV_ABS, ABS_MT_TRACKING_ID, -1)
    send_event(fd, EV_SYN, SYN_REPORT, 0)
    ffi.C.close(fd)
    return true
end

local NEXT_X, NEXT_Y = 1100, 800
local PREV_X, PREV_Y = 100, 800

local server = assert(socket.bind("0.0.0.0", 8888))
print("Page turner server running on port 8888")

while true do
    local client = server:accept()
    if client then
        client:settimeout(1)
        local request = client:receive()
        local response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nAccess-Control-Allow-Origin: *\r\n\r\n"

        if request and request:match("GET /next") then
            tap(NEXT_X, NEXT_Y)
            response = response .. "Next page"
        elseif request and request:match("GET /prev") then
            tap(PREV_X, PREV_Y)
            response = response .. "Previous page"
        else
            response = response .. "Use /next or /prev"
        end

        client:send(response)
        client:close()
    end
end
EOF
```

## Step 5: Start the Server

Each time you want to use the remote:

```bash
# SSH into Kindle
ssh -p 2222 root@<KINDLE_IP>

# Start the server
cd /mnt/us/koreader && ./luajit /mnt/us/pageserver.lua &

# Open firewall port
iptables -I INPUT 1 -p tcp --dport 8888 -j ACCEPT
```

## Step 6: Test It

From your PC, test the server:

```bash
# Next page
curl http://<KINDLE_IP>:8888/next

# Previous page
curl http://<KINDLE_IP>:8888/prev
```

You should see the page turn on your Kindle!

## Troubleshooting

### SSH Connection Refused
- Make sure KOReader is open
- Check SSH is enabled in KOReader settings
- Verify the IP address

### Connection Refused on Port 8888
Add firewall rule:
```bash
iptables -I INPUT 1 -p tcp --dport 8888 -j ACCEPT
```

### Page Turns Not Working
Your Kindle may use a different touch device. Find yours:
```bash
cat /proc/bus/input/devices | grep -A5 touch
```

Then update the device path in `pageserver.lua` (change `/dev/input/event9` to your device).

### Server Stops When Closing KOReader
The server uses KOReader's Lua libraries. Start the server while KOReader is running, then you can minimize/close KOReader and use the stock reader.

## Next Steps

Continue to [Hardware Remote](HARDWARE-REMOTE.md) to set up the M5StickC PLUS2.
