-- Kindle Page Turner HTTP Server
-- Runs on Kindle with KOReader installed
-- Listens on port 8888 for /next and /prev requests

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

-- Screen coordinates for page turns
-- Adjust these if your Kindle model has different screen dimensions
local NEXT_X, NEXT_Y = 1100, 800  -- Right side tap for next page
local PREV_X, PREV_Y = 100, 800   -- Left side tap for previous page

local server = assert(socket.bind("0.0.0.0", 8888))
print("Page turner server running on port 8888")
print("Next page: GET /next")
print("Previous page: GET /prev")

while true do
    local client = server:accept()
    if client then
        client:settimeout(1)
        local request = client:receive()
        local response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nAccess-Control-Allow-Origin: *\r\n\r\n"

        if request and request:match("GET /next") then
            tap(NEXT_X, NEXT_Y)
            response = response .. "Next page"
            print("-> Next page")
        elseif request and request:match("GET /prev") then
            tap(PREV_X, PREV_Y)
            response = response .. "Previous page"
            print("-> Previous page")
        else
            response = response .. "Use /next or /prev"
        end

        client:send(response)
        client:close()
    end
end
