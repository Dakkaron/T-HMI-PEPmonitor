print(string.format("Lua version is %s", _VERSION))

LED_BUILTIN = 13

pinMode(LED_BUILTIN, OUTPUT)

for i=1,5,1
do
   print(i)
   print("LED ON")
   digitalWrite(LED_BUILTIN, HIGH)
   delay(1000)
   print("LED OFF")
   digitalWrite(LED_BUILTIN, LOW)
   delay(1000)
end
