if (isTouchInZone(0, 85, 85, 101, 0xF800)) then
  touchX = getTouchX() - 35
  touchY = getTouchY() - 135
  length = math.sqrt(touchX * touchX + touchY * touchY)
  if (length > 0) then
    inhalationVectorX = touchX / length
    inhalationVectorY = touchY / length
  else
    touchX = 0
    touchY = 0
  end
  inhalationCrosshairX = 50*inhalationVectorX + 35
  inhalationCrosshairY = 50*inhalationVectorY + 135
end

if (currentlyBlowing) then
  baseEffect = (ms-blowStartMs) / 2000.0
  if (not wasBlowing) then
    wasBlowing = true
    targetVector = {}
    for i = 1, 3 do
      targetEffectiveness[i] = 100.0 / math.max(1,getDistancePointLine(35, 135, inhalationCrosshairX, inhalationCrosshairY, targetPositions[i][1], targetPositions[i][2]))
      targetVector[i] = {targetPositions[i][1] - 35, targetPositions[i][2] - 135}
      length = math.sqrt(targetVector[i][1] * targetVector[i][1] + targetVector[i][2] * targetVector[i][2])
      targetVector[i][1] = targetVector[i][1] / length
      targetVector[i][2] = targetVector[i][2] / length
    end
  end
  for i = 1, 3 do
    targetPositions[i][1] = targetPositions[i][1] - targetVector[i][1] * baseEffect * targetEffectiveness[i]
    targetPositions[i][2] = targetPositions[i][2] - targetVector[i][2] * baseEffect * targetEffectiveness[i]
    if (targetPositions[i][1] <= 35) then
      targetPositions[i][1] = 100000
      r = getEarnings(targetType[i])
      earnValue = earnValue + r
      money = money + r
      earnTime = ms + 2000
    end
  end
else
  wasBlowing = false
  for i = 1, 3 do
    if (targetPositions[i][1] > 500) then
      targetPositions[i][1] = math.random(100, 300)
      targetPositions[i][2] = math.random(20, 160)
      r = getEarnings(targetType[i])
      t = math.random(1, 100) * breathingScore
      log("Breathing score: " .. breathingScore)
      log("t" .. t)
      if (t > 50000) then
        targetType[i] = 6
      elseif (t > 10000) then
        targetType[i] = 5
      elseif (t > 5000) then
        targetType[i] = 4
      elseif (t > 1000) then
        targetType[i] = 3
      else
        targetType[i] = 2
      end
    end
  end
end

drawSprite(sRobotFunnel, 20, 120)

for i = 1, 3 do
  drawSprite(typeSprites[targetType[i]], targetPositions[i][1] - spriteWidth(typeSprites[targetType[i]])/2, targetPositions[i][2] - spriteHeight(typeSprites[targetType[i]])/2)
end


drawCircle(35, 135, 50, 0xFFFF)

setTextColor(0xFD00)
setTextSize(2)
drawString("X", inhalationCrosshairX-4, inhalationCrosshairY-6)
setTextColor(0xFFFF)
drawString("$" .. money, 190, 188)

setTextColor(0xFFE0)
displayEarnings(50, 50)
setTextColor(0xFFFF)