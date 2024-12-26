#!/usr/bin/env python3
import serial
import time
import re
import os.path
import os
import readline
import atexit
import esptool
import serial.serialutil

MAX_RETRIES = 5
MAX_READ_RETRIES = 1
PORT_NAME = "/dev/ttyACM"
BAUD_RATE = 115200

histfile = os.path.join(os.path.expanduser("~"), ".pepit_fm_history")
try:
    readline.read_history_file(histfile)
    # default history len is -1 (infinite), which may grow unruly
    readline.set_history_length(1000)
except FileNotFoundError:
    pass

atexit.register(readline.write_history_file, histfile)

for i in range(10):
    try:
        ser = serial.Serial(f"{PORT_NAME}{i}", 115200, timeout=10)
        print(f"Port {PORT_NAME}{i} connected")
        break
    except serial.serialutil.SerialException:
        print(f"Port {PORT_NAME}{i} not found")

def reset():
    global ser
    print("Resetting...")
    ser.close()
    esp32 = esptool.get_default_connected_device(["/dev/ttyACM0", "/dev/ttyACM1", "/dev/ttyACM2"], None, 1, BAUD_RATE)
    esp32.hard_reset()
    time.sleep(3)
    ser = serial.Serial(PORT_NAME, 115200, timeout=20)

def writeFile(path, data):
    if isinstance(path,str):
        path = path.encode("utf-8")
    if isinstance(data,str):
        data = data.encode("utf-8")
    ser.flush()
    ser.reset_input_buffer()
    ser.reset_output_buffer()
    print(b" ul "+path)
    print(str(len(data)).encode("utf-8"))
    print(len(data))
    ser.write(b" ul "+path+b"\n")
    ser.write(str(len(data)).encode("utf-8")+b"\n")
    for i in range(1+len(data)):
        ser.write(data[i:i+1])
        ser.flush()
    res = b""
    time.sleep(5)
    while ser.in_waiting > 0:
        res += ser.read(ser.in_waiting)
        time.sleep(0.01)
    print(res.decode("utf-8", errors="ignore"))

def readFile(path, retry=0):
    if retry>=MAX_RETRIES:
        print(f"Giving up downloading {path}")
        raise RuntimeError(f"Giving up downloading {path}")
    if isinstance(path,str):
        path = path.encode("utf-8")
    ser.flush()
    ser.reset_input_buffer()
    ser.reset_output_buffer()
    ser.write(b" catl "+path+b"\n")
    res = b""
    readRetry = 0
    while not res.endswith(b"Read from file: \r\n"):
        read = ser.read(1)
        if len(read) <= 0:
            readRetry += 1
            print("Nothing here, retrying")
        else:
            readRetry = 0
        if readRetry > MAX_READ_RETRIES:
            print(f"Failed to download {path} due to nothing to read. Retrying.")
            reset()
            return readFile(path, retry + 1)
        res += read
    lines = res.strip().splitlines()
    try:
        fileLen = int(lines[-2])
    except:
        print(f"Failed to download {path} due to wrong fileLen. Retrying.")
        reset()
        time.sleep(20)
        return readFile(path, retry + 1)
    print(f"File len: {fileLen}")
    out = ser.read(fileLen)
    readRetry = 0
    for i in range(3):
        if len(out)<fileLen:
            read = ser.read(fileLen - len(out))
            out += read
            if len(read) <= 0:
                readRetry += 1
                print("Nothing here, retrying")
            else:
                readRetry = 0
            if readRetry > MAX_READ_RETRIES:
                print(f"Failed to download {path} due to nothing to read. Retrying.")
                reset()
                return readFile(path, retry + 1)
    print(f"Bytes read: {len(out)}")
    if len(out) != fileLen:
        print(f"Failed to download {path}, retrying.")
        reset()
        return readFile(path, retry + 1)
    return out

def ls(path, absolute=False):
    if isinstance(path,str):
        path = path.encode("utf-8")
    ser.flush()
    ser.reset_input_buffer()
    ser.reset_output_buffer()
    ser.write(b" ls "+path+b"\n")
    res = b""
    while not res.endswith(b"\r\n$ "):
        res += ser.read(1)
    prefix = path if absolute else b""
    dirs  = [ os.path.join(prefix, x.strip()[6:]) for x in res.splitlines() if x.strip().startswith(b"DIR : ") ]
    files = [ tuple(x.strip()[6:].split(b" SIZE: ")) for x in res.splitlines() if x.strip().startswith(b"FILE: ") ]
    files = [ (os.path.join(prefix, x[0]), int(x[1])) for x in files ]
    return {"dirs":dirs, "files":files}

def leftPad(s, lm):
    l = lm - len(s)
    if (l<=0):
        return s
    return " "*l + s

def rightPad(s, lm):
    l = lm - len(s)
    if (l<=0):
        return s
    return s + " "*l

def printLs(lsData):
    print("DIRs:")
    print("\n".join([ "          "+x.decode("utf-8") for x in lsData["dirs"]]))

    print("Files:")
    print("\n".join([ leftPad(str(x[1]),8)+"  "+x[0].decode("utf-8") for x in lsData["files"]]))

def ul(src, target):
    with open(src.strip(), "rb") as f:
        data = f.read()
    writeFile(target.strip(), data)

def dl(src, target):
    data = readFile(src.strip())
    with open(target.strip(), "wb") as f:
        f.write(data)

def issueCommand(command, param):
    if isinstance(command,str):
        command = command.encode("utf-8")
    if isinstance(param,str):
        param = param.encode("utf-8")
    ser.flush()
    ser.reset_input_buffer()
    ser.reset_output_buffer()
    ser.write(b" "+command+b" "+param+b"\n")
    res = b""
    time.sleep(0.1)
    while ser.in_waiting > 0:
        res += ser.read(ser.in_waiting)
    return res

def mkdir(path):
    res = issueCommand("mkdir", path)
    if res:
        print(res.decode("utf-8"))

def rmdir(path):
    res = issueCommand("rmdir", path)
    if res:
        print(res.decode("utf-8"))

def rm(path):
    res = issueCommand("rm", path)
    if res:
        print(res.decode("utf-8"))

def rmdirr(path):
    lsres = ls(path)
    for file in lsres["files"]:
        rm(os.path.join(path, file[0]))
    for dir in lsres["dirs"]:
        rmdirr(os.path.join(path, dir))
        rmdir(os.path.join(path, dir))
    rmdir(path)

def ulr(src, target):
    print(f"ULR {src} {target}")
    paths = os.listdir(src)
    mkdir(target)
    for path in paths:
        srcPath = os.path.join(src, path)
        targetPath = os.path.join(target, path)
        if os.path.isdir(srcPath):
            ulr(srcPath, targetPath)
        elif os.path.isfile(srcPath):
            print(f"UL {srcPath} {targetPath}")
            ul(srcPath, targetPath)

def dlr(src, target):
    start = time.time()
    os.makedirs(target, exist_ok=True)
    print(f"DLR {src} {target}")
    lsres = ls(src)
    for file in lsres["files"]:
        filePath = file[0].decode("utf-8")
        print(f"DL {os.path.join(src, filePath)} {os.path.join(target, filePath)}")
        dl(os.path.join(src, filePath), os.path.join(target, filePath))
    for dir in lsres["dirs"]:
        dirPath = dir.decode("utf-8")
        dlr(os.path.join(src, dirPath), os.path.join(target, dirPath))
    print(f"Took {time.time()-start}s")

if __name__ == "__main__":
    while True:
        inp = input("$ ").strip()
        cmd = inp.split(" ")[0]
        param = inp[len(cmd):].strip()
        if cmd == "lsa":
            printLs(ls(param, True))
        elif cmd == "ls":
            printLs(ls(param, False))
        elif cmd == "mkdir":
            mkdir(param)
        elif cmd == "rmdir":
            rmdir(param)
        elif cmd == "rm":
            rm(param)
        elif cmd == "cat":
            res = readFile(param)
            res = res.decode("utf-8")
            print("File content:")
            print(res)
        elif cmd == "ul":
            path = param.split(" ")
            ul(path[0], path[1])
        elif cmd == "ulr":
            path = param.split(" ")
            print(path)
            ulr(path[0], path[1])
        elif cmd == "dl":
            path = param.split(" ")
            dl(path[0], path[1])
        elif cmd == "dlr":
            path = param.split(" ")
            print(path)
            dlr(path[0], path[1])
        elif inp == "exit":
            exit()
        elif inp == "reset":
            reset()
            exit()
        else:
            print(f"Unknown command: {inp}")
