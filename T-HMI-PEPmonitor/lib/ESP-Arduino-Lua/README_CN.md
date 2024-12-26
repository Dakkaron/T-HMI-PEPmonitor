* [English Version](README.md)

# Arduino Lua

本Arduino扩展库为支持的设备提供 [lua](https://www.lua.org/) 5.3.6 ([正式版](https://www.lua.org/ftp/lua-5.3.6.tar.gz)) 的脚本运行引擎。 使用该扩展库，可以在Arduino上动态运行Lua代码，而不必重新编译和烧录固件。

支持的设备：
* ESP8266
* ESP32/ESP32S2/ESP32C3/ESP32S3
* Arduino Uno R4 Minima/WiFi
* Raspberry Pi Pico/Pico W

除了 Lua 5.3.6 核心功能以外，还包含了如下的 Lua 标准库：

- base (print(), tostring(), tonumber(), etc.)
- math
- table (insert(), sort(), remove(), ...)
- string (len(), match(), ...)

##  示例: ExecuteScriptFromSerial.ino

安装本扩展后，在 *File* 菜单中的 *Examples* 下的 *Arduino-Lua* 中 会提供部分示例。示例中包含 **ExecuteScriptFromSerial** ，可以通过串口输入 Lua 脚本并执行。例如，如下的标准 Arduino 函数，可以被绑定从而在 lua 脚本中使用：

- pinMode()
- digitalWrite()
- delay()
- millis()
- print() *(only builtin binding)*

```
# Enter the lua script and press Control-D when finished:
# 输入如下的 lua脚本，然后按 Ctrl+D 运行:
print("My first test!")
# Executing script:
# 运行脚本:
My first test!


# Enter the lua script and press Control-D when finished-
print("Current uptime: " .. millis())
# Executing script:
Current uptime: 159926.0


# Enter the lua script and press Control-D when finished:
print("Hello world!")
# Executing script:
Hello world!


# Enter the lua script and press Control-D when finished:
pinLED = 2
period = 500
pinMode(pinLED, OUTPUT)
while(true)
do
  print("LED on")
  digitalWrite(pinLED, LOW)
  delay(period)
  print("LED off")
  digitalWrite(pinLED, HIGH)
  delay(period)
end
# Executing script:
LED on
LED off
```
## 资源使用 (ExecuteScriptFromSerial.ino)

**ESP8266:**
Sketch uses 327776 bytes (31%) of program storage space. Maximum is 1044464 bytes.
Global variables use 31276 bytes (38%) of dynamic memory, leaving 50644 bytes for local variables. Maximum is 81920 bytes.

**ESP32:**
Sketch uses 310749 bytes (23%) of program storage space. Maximum is 1310720 bytes.
Global variables use 15388 bytes (4%) of dynamic memory, leaving 312292 bytes for local variables. Maximum is 327680 bytes.

**Arduino Uno R4:**
Sketch uses 133756 bytes (51%) of program storage space. Maximum is 262144 bytes.
Global variables use 2836 bytes (8%) of dynamic memory, leaving 29932 bytes for local variables. Maximum is 32768 bytes.

**Raspberry Pi Pico:**
Sketch uses 124736 bytes (5%) of program storage space. Maximum is 2093056 bytes.
Global variables use 10352 bytes (3%) of dynamic memory, leaving 251792 bytes for local variables. Maximum is 262144 bytes.

## 示例: HelloWorld.ino
```
#include <LuaWrapper.h>

LuaWrapper lua;

void setup() {
  Serial.begin(115200);
  String script = String("print('Hello world!')");
  Serial.println(lua.Lua_dostring(&script));
}

void loop() {

}
```
## 资源使用 (HelloWorld.ino)

**ESP8266:**
Sketch uses 365276 bytes (34%) of program storage space. Maximum is 1044464 bytes.
Global variables use 34712 bytes (42%) of dynamic memory, leaving 47208 bytes for local variables. Maximum is 81920 bytes.

**ESP32:**
Sketch uses 309913 bytes (23%) of program storage space. Maximum is 1310720 bytes.
Global variables use 15388 bytes (4%) of dynamic memory, leaving 312292 bytes for local variables. Maximum is 327680 bytes.

## Lua 语言:
[Lua 5.3 参考手册](https://www.lua.org/manual/5.3/)

