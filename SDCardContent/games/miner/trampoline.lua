if (worldMapArray[3][1] ~= 0) then -- init
  for x = 0, 7 do
    for y = 0, 4 do
      worldMapArray[x][y] = 1
    end
  end
  for x = 3, 4 do
    for y = 0, 2 do
      worldMapArray[x][y] = 0
    end
  end
end

if (newJump) then
  mineTile(3, 3)
  mineTile(4, 3)
end

if (lastJumpMs + 200 > ms) then
  jumpOffset = (ms-lastJumpMs) / 8
  yDigOffset = (ms-lastJumpMs) / 5
elseif (yDigOffset > 0) then
  for x = 0, 7 do
    for y = 0, 4 do
      worldMapArray[x][y] = worldMapArray[x][y+1]
    end
    genTile(x, 5, 2000)
  end
  yDigOffset = 0
else
  yDigOffset = 0
  jumpOffset = 25
end

for x = 0, 7 do
  for y = 0, 4 do
    drawTile(x, y, yDigOffset, true)
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

drawSprite(sBigDigger, 157-spriteWidth(sBigDigger)/2, 85-spriteHeight(sBigDigger)/2 - jumpOffset + yDigOffset)

displayEarnings(160, 80)
setTextSize(2)
drawString("$" .. money, 190, 188)