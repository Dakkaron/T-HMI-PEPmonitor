if (isTouchInZone(0, 0, 100, 240)) then
  if (playerX > 0 and not isCurrentlyTouching and stepsLeft > 0) then
    isCurrentlyTouching = true
    playerX = playerX - 1
    mineTile(playerX, 1)
    stepsLeft = stepsLeft - 1
  end
elseif (isTouchInZone(220, 0, 100, 240)) then
  if (playerX < 7 and not isCurrentlyTouching and stepsLeft > 0) then
    isCurrentlyTouching = true
    playerX = playerX + 1
    mineTile(playerX, 1)
    stepsLeft = stepsLeft - 1
  end
elseif (isTouchInZone(110, 0, 40, 53)) then
  if (money >= 2 and not isCurrentlyTouching) then
    isCurrentlyTouching = true
    money = money - 2
    earnTime = ms + 2000
    earnValue = -2
    fullVisibilityTime = ms + 2000
  end
else
  isCurrentlyTouching = false
end

if (lastBlowStatus == 1 and ms-blowEndMs > 0 and ms-blowEndMs < 1000) then
  yDigOffset = (ms - blowEndMs) / 22
  mineTile(playerX, 2)
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
    drawTile(x, y, yDigOffset, fullVisibilityTime>ms)
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
  drawSprite(sRobotDig, playerX*40+3 + playerXOffset, 1*40 + math.fmod(ms/100, 2) * 4)
else
  drawSprite(sRobotIdle, playerX*40+3 + playerXOffset, 1*40)
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
  if (earnValue > 0) then
    drawString("+" .. earnValue, playerX*40 + 20, 40 + (earnTime - ms) * 0.01)
  else
    drawString("" .. earnValue, playerX*40 + 20, 40 + (earnTime - ms) * 0.01)
  end
end

drawString("$" .. money, 190, 188)
drawString("Schritte: " .. stepsLeft, 190, 213)

drawSprite(sItemLantern, 120, 10)
drawString("$2", 130, 10)
setTextSize(1)
