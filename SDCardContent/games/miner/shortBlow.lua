if isNewTask then
  rockImageLoaded = -1
end

targetRockImageNumber = (10*blowNumber) // totalBlowNumber
if rockImageLoaded < targetRockImageNumber then
  if rockImageLoaded >= 0 then
    freeSprite(sRock)
  end
  rockImageLoaded = targetRockImageNumber
  if rockImageLoaded < 10 then
    sRock = loadSprite("gfx/rock_" .. rockImageLoaded .. ".bmp", 0, 0x0000)
  end
  if rockImageLoaded > 0 then
    earnTime = ms + 1000
    earnValue = 1
    money = money + earnValue
  end
end

if rockImageLoaded >= 0 and rockImageLoaded<10 then
  drawSprite(sRock, 210, 160 - spriteHeight(sRock))
end

drawSprite(sRobotFunnel, 20, 120)

if (currentlyBlowing) then
  rayWidth = 200 * math.min((ms-blowStartMs) / targetDurationMs)
  px = 54
  py = 137
  drawFastHLine(px, py, rayWidth, 0x0000FF)
  drawFastHLine(px, py+1, rayWidth, 0x9999FF)
  drawFastHLine(px, py+2, rayWidth, 0xDDDDFF)
  drawFastHLine(px, py+3, rayWidth, 0xDDDDFF)
  drawFastHLine(px, py+4, rayWidth, 0xDDDDFF)
  drawFastHLine(px, py+5, rayWidth, 0x9999FF)
  drawFastHLine(px, py+6, rayWidth, 0x0000FF)
end

setTextSize(2)
if (earnTime > ms) then
  drawAnimSprite(sExplosionAnim, 225, 100, 10 - ((earnTime - ms) // 100))
  drawString("+" .. earnValue, 265, 40 + (earnTime - ms) * 0.01)
end

drawString("$" .. money, 190, 188)
setTextSize(1)

--drawString("Free RAM" .. getFreeRAM(), 10, 100);