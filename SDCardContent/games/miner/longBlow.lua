if (isTouchInZone(0, 0, 100, 240)) then
  if (playerX > 0 and not isCurrentlyTouching and stepsLeft > 0) then
    isCurrentlyTouching = true
    playerX = playerX - 1
    mineTile(playerX, 2)
    stepsLeft = stepsLeft - 1
  end
elseif (isTouchInZone(220, 0, 100, 240)) then
  if (playerX < 7 and not isCurrentlyTouching and stepsLeft > 0) then
    isCurrentlyTouching = true
    playerX = playerX + 1
    mineTile(playerX, 2)
    stepsLeft = stepsLeft - 1
  end
else
  isCurrentlyTouching = false
end

if (lastBlowStatus == 1 and ms-blowEndMs > 0 and ms-blowEndMs < 1000) then
  yDigOffset = (ms - blowEndMs) / 22
  mineTile(playerX, 3)
elseif yDigOffset > 0 then
  for x = 0, 7 do
    for y = 0, 4 do
      worldMapArray[x][y] = worldMapArray[x][y+1]
    end
    v = math.random(0, 100)
    genTile(x, 5)
  end
  yDigOffset = 0
  stepsLeft = 2
else
  yDigOffset = 0
end

for x = 0, 7 do
  for y = 0, 4 do
    drawTile(x, y, yDigOffset)
  end
end
if (yDigOffset > 0) then
  for x = 0, 7 do
    drawTile(x, 5, yDigOffset)
  end
  fillRect(0, 176, 320, 24+40 - yDigOffset, 0x0)
else
  fillRect(0, 176, 320, 24, 0x0)
end

if (currentlyBlowing) then
  drawSprite(sRobotDig, playerX*40+3 + playerXOffset, 2*40 + math.fmod(ms/100, 2) * 4)
else
  drawSprite(sRobotIdle, playerX*40+3 + playerXOffset, 2*40)
end

if (playerXOffset > 0) then
  playerXOffset = playerXOffset - msDelta * 0.08
elseif (playerXOffset < 0) then
  playerXOffset = playerXOffset + msDelta * 0.08
end
if (math.abs(playerXOffset) < 0.1) then
  playerXOffset = 0
end

setTextSize(2)

if (earnTime > ms) then
  drawString("+" .. earnValue, playerX*40 + 20, 40 + (earnTime - ms) * 0.01)
end

drawString("$" .. money, 190, 188)
drawString("Schritte: " .. stepsLeft, 190, 213)
setTextSize(1)

--[[drawString("EggBmp " .. eggBmp, 100, 10)
drawString("Blow count " .. blowCount, 100, 20)

drawAnimSprite(eggBmp, 10, 85, (ms / 500) % 6)
prefsSetInt("hatched", 1)
]]