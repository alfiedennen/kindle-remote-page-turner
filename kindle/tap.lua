-- Kindle Touch Injection Helper
-- Usage: ./luajit tap.lua [x] [y]
-- Example: ./luajit tap.lua 900 800

local ffi = require("ffi")

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
    local fd = ffi.C.open("/dev/input/event9", 1) -- O_WRONLY
    if fd < 0 then
        print("Failed to open /dev/input/event9")
        print("Check your touch device with: cat /proc/bus/input/devices")
        return
    end

    send_event(fd, EV_ABS, ABS_MT_TRACKING_ID, 0)
    send_event(fd, EV_ABS, ABS_MT_POSITION_X, x)
    send_event(fd, EV_ABS, ABS_MT_POSITION_Y, y)
    send_event(fd, EV_KEY, BTN_TOUCH, 1)
    send_event(fd, EV_SYN, SYN_REPORT, 0)
    send_event(fd, EV_KEY, BTN_TOUCH, 0)
    send_event(fd, EV_ABS, ABS_MT_TRACKING_ID, -1)
    send_event(fd, EV_SYN, SYN_REPORT, 0)

    ffi.C.close(fd)
    print("Tap sent at " .. x .. "," .. y)
end

-- Default coordinates (right side of screen for next page)
local x = tonumber(arg[1]) or 900
local y = tonumber(arg[2]) or 800

tap(x, y)
