function isNextToTunnel(x, y)
  return (x>0 and worldMapArray[x-1][y]==0) or (x<7 and worldMapArray[x+1][y]==0) or (y>0 and worldMapArray[x][y-1]==0) or (y<5 and worldMapArray[x][y+1]==0) or (x>0 and y>0 and worldMapArray[x-1][y-1]==0) or (x<7 and y>0 and worldMapArray[x+1][y-1]==0) or (x>0 and y<5 and worldMapArray[x-1][y+1]==0) or (x<7 and y<5 and worldMapArray[x+1][y+1]==0)
end

function drawFOWDirt(x, y, yOffset, showHidden)
  if (showHidden) then
    drawSprite(tiles.dirt, x*40, y*40 - yOffset)
  elseif ((x==0 or not isNextToTunnel(x-1, y)) and (y==0 or not isNextToTunnel(x, y-1))) then
    drawSprite(tiles.shadowTopLeft, x*40, y*40 - yOffset, 0x0)
  elseif ((x==7 or not isNextToTunnel(x+1, y)) and (y==0 or not isNextToTunnel(x, y-1))) then
    drawSprite(tiles.shadowTopRight, x*40, y*40 - yOffset, 0x0)
  elseif ((x==0 or not isNextToTunnel(x-1, y)) and (y==5 or not isNextToTunnel(x, y+1))) then
    drawSprite(tiles.shadowBottomLeft, x*40, y*40 - yOffset, 0x0)
  elseif ((x==7 or not isNextToTunnel(x+1, y)) and (y==5 or not isNextToTunnel(x, y+1))) then
    drawSprite(tiles.shadowBottomRight, x*40, y*40 - yOffset, 0x0)
  elseif (x==0 or not isNextToTunnel(x-1, y)) then
    drawSprite(tiles.shadowLeft, x*40, y*40 - yOffset, 0x0)
  elseif (x==7 or not isNextToTunnel(x+1, y)) then
    drawSprite(tiles.shadowRight, x*40, y*40 - yOffset, 0x0)
  elseif (y==0 or not isNextToTunnel(x, y-1)) then
    drawSprite(tiles.shadowTop, x*40, y*40 - yOffset, 0x0)
  elseif (y==5 or not isNextToTunnel(x, y+1)) then
    drawSprite(tiles.shadowBottom, x*40, y*40 - yOffset, 0x0)
  else
    drawSprite(tiles.dirt, x*40, y*40 - yOffset, 0x0)
  end
end

function drawTile(x, y, yOffset, showHidden)
  if (worldMapArray[x][y] == 0) then
    if (worldMapArray[x][y-1] == 0 or y == 0) then -- up
      if (x > 0 and worldMapArray[x-1][y] == 0) then -- left
        if (x < 7 and worldMapArray[x+1][y] == 0) then -- right
          if (worldMapArray[x][y+1] == 0) then -- down
            drawSprite(tiles.tunnelX, x*40, y*40 - yOffset)
          else -- up, left, right, not down
            drawSprite(tiles.tunnelT, x*40, y*40 - yOffset)
          end
        else -- up, left, not right
          if (worldMapArray[x][y+1] == 0) then -- down
            drawSprite(tiles.tunnelTLeft, x*40, y*40 - yOffset)
          else -- not down
            drawSprite(tiles.tunnelLLeft, x*40, y*40 - yOffset)
          end
        end
      else -- up, not left
        if (x < 7 and worldMapArray[x+1][y] == 0) then -- right
          if (worldMapArray[x][y+1] == 0) then -- down
            drawSprite(tiles.tunnelTRight, x*40, y*40 - yOffset)
          else -- not down
            drawSprite(tiles.tunnelLRight, x*40, y*40 - yOffset)
          end
        else -- up, not left, not right
          if (worldMapArray[x][y+1] == 0) then -- down
            drawSprite(tiles.tunnelI, x*40, y*40 - yOffset)
          else -- not down
            drawSprite(tiles.tunnel, x*40, y*40 - yOffset)
          end
        end
      end
    else -- not up
      if (x > 0 and worldMapArray[x-1][y] == 0) then -- left
        if (x < 7 and worldMapArray[x+1][y] == 0) then -- right
          if (worldMapArray[x][y+1] == 0) then -- down
            drawSprite(tiles.tunnelTDown, x*40, y*40 - yOffset)
          else -- not up, left, right, not down
            drawSprite(tiles.tunnelFlat, x*40, y*40 - yOffset)
          end
        else -- not up, left, not right
          if (worldMapArray[x][y+1] == 0) then -- down
            drawSprite(tiles.tunnelFLeft, x*40, y*40 - yOffset)
          else -- not down
            drawSprite(tiles.tunnelLeft, x*40, y*40 - yOffset)
          end
        end
      else -- not up, not left
        if (x < 7 and worldMapArray[x+1][y] == 0) then -- right
          if (worldMapArray[x][y+1] == 0) then -- down
            drawSprite(tiles.tunnelFRight, x*40, y*40 - yOffset)
          else -- not down
            drawSprite(tiles.tunnelRight, x*40, y*40 - yOffset)
          end
        end
      end
    end
  elseif (showHidden or isNextToTunnel(x, y)) then
    drawFOWDirt(x, y, yOffset, showHidden)
    if (worldMapArray[x][y] == 2) then
      drawSprite(sTileOreIron, x*40, y*40 - yOffset)
    elseif (worldMapArray[x][y] == 3) then
      drawSprite(sTileOreCopper, x*40, y*40 - yOffset)
    elseif (worldMapArray[x][y] == 4) then
      drawSprite(sTileOreEmerald, x*40, y*40 - yOffset)
    elseif (worldMapArray[x][y] == 5) then
      drawSprite(sTileOreGold, x*40, y*40 - yOffset)
    elseif (worldMapArray[x][y] == 6) then
      drawSprite(sTileOreDiamond, x*40, y*40 - yOffset)
    elseif (worldMapArray[x][y] == 7) then
      drawSprite(sTileRock, x*40, y*40 - yOffset)
    end
  end
end

function getEarnings(type)
  if (type == 2) then
    return 1
  elseif (type == 3) then
    return 2
  elseif (type == 4) then
    return 3
  elseif (type == 5) then
    return 4
  elseif (type == 6) then
    return 5
  end
  return 0
end

function mineTile(x,y)
  r = getEarnings(worldMapArray[x][y])
  if (r>0) then
    earnValue = earnValue + r
    earnTime = ms + 2000
    money = money + r
  end
  worldMapArray[x][y] = 0
  return r
end

function genTile(x, y, baseOdds)
  baseOdds = baseOdds or 1000
  v = math.random(0, baseOdds)
  if (stage == 1) then
    if (v < 20) then
      worldMapArray[x][y] = 7
    elseif (v < 70) then
      worldMapArray[x][y] = 3
    elseif (v < 170) then
      worldMapArray[x][y] = 2
    else
      worldMapArray[x][y] = 1
    end
  elseif (stage == 2) then
    if (v < 20) then
      worldMapArray[x][y] = 7
    elseif (v < 50) then
      worldMapArray[x][y] = 4
    elseif (v < 100) then
      worldMapArray[x][y] = 3
    elseif (v < 200) then
      worldMapArray[x][y] = 2
    else
      worldMapArray[x][y] = 1
    end
  elseif (stage == 3) then
    if (v < 20) then
      worldMapArray[x][y] = 7
    elseif (v < 40) then
      worldMapArray[x][y] = 5
    elseif (v < 70) then
      worldMapArray[x][y] = 4
    elseif (v < 120) then
      worldMapArray[x][y] = 3
    elseif (v < 220) then
      worldMapArray[x][y] = 2
    else
      worldMapArray[x][y] = 1
    end
  end
  worldMapArray[x][y] = worldMapArray[x][y]
end

function initTileFragment(dirtPath, overlayPath, flip)
  tile = loadSprite(dirtPath)
  tmp = loadSprite(overlayPath, flip, 0x0000)
  overlay = loadSprite(overlayPath, flip, 0x0000)
  drawSpriteToSprite(overlay, tmp, 0, 0) -- TODO: Fix drawSpriteToSprite so that it doesn't invert colors
  drawSpriteToSprite(tmp, tile, 0, 0)
  freeSprite(tmp)
  freeSprite(overlay)
  return tile
end

function initTileFragments(tileTable, dirtPath)
  tileTable["dirt"] = loadSprite(dirtPath, 0, 0x0000)
  tileTable["tunnel"] = initTileFragment(dirtPath, "gfx/tilefragments/tile_tunnel.bmp", 0)
  tileTable["tunnelI"] = initTileFragment(dirtPath, "gfx/tilefragments/tile_tunnel_I.bmp", 0)
  tileTable["tunnelFlat"] = initTileFragment(dirtPath, "gfx/tilefragments/tile_tunnel_flat.bmp", 0)
  tileTable["tunnelLeft"] = initTileFragment(dirtPath, "gfx/tilefragments/tile_tunnel_left.bmp", 0)
  tileTable["tunnelRight"] = initTileFragment(dirtPath, "gfx/tilefragments/tile_tunnel_left.bmp", 1)
  tileTable["tunnelLLeft"] = initTileFragment(dirtPath, "gfx/tilefragments/tile_tunnel_L_left.bmp", 0)
  tileTable["tunnelLRight"] = initTileFragment(dirtPath, "gfx/tilefragments/tile_tunnel_L_left.bmp", 1)
  tileTable["tunnelFLeft"] = initTileFragment(dirtPath, "gfx/tilefragments/tile_tunnel_L_left.bmp", 2)
  tileTable["tunnelFRight"] = initTileFragment(dirtPath, "gfx/tilefragments/tile_tunnel_L_left.bmp", 3)
  tileTable["tunnelT"] = initTileFragment(dirtPath, "gfx/tilefragments/tile_tunnel_T.bmp", 0)
  tileTable["tunnelTDown"] = initTileFragment(dirtPath, "gfx/tilefragments/tile_tunnel_T.bmp", 2)
  tileTable["tunnelTLeft"] = initTileFragment(dirtPath, "gfx/tilefragments/tile_tunnel_T_left.bmp", 0)
  tileTable["tunnelTRight"] = initTileFragment(dirtPath, "gfx/tilefragments/tile_tunnel_T_left.bmp", 1)
  tileTable["tunnelX"] = initTileFragment(dirtPath, "gfx/tilefragments/tile_tunnel_X.bmp", 0)

  sFOWBottom = loadSprite("gfx/tilefragments/tile_fow_down.bmp", 0, 0x0001)
  sFOWTop = loadSprite("gfx/tilefragments/tile_fow_down.bmp", 2, 0x0001)
  sFOWLeft = loadSprite("gfx/tilefragments/tile_fow_left.bmp", 0, 0x0001)
  sFOWRight = loadSprite("gfx/tilefragments/tile_fow_left.bmp", 1, 0x0001)
  tileTable["shadowLeft"] = loadSprite(dirtPath)
  tileTable["shadowRight"] = loadSprite(dirtPath)
  tileTable["shadowTop"] = loadSprite(dirtPath)
  tileTable["shadowBottom"] = loadSprite(dirtPath)
  tileTable["shadowTopLeft"] = loadSprite(dirtPath)
  tileTable["shadowTopRight"] = loadSprite(dirtPath)
  tileTable["shadowBottomLeft"] = loadSprite(dirtPath)
  tileTable["shadowBottomRight"] = loadSprite(dirtPath)

  drawSpriteToSprite(sFOWLeft, tileTable["shadowLeft"], 0, 0)
  drawSpriteToSprite(sFOWRight, tileTable["shadowRight"], 31, 0)
  drawSpriteToSprite(sFOWTop, tileTable["shadowTop"], 0, 0)
  drawSpriteToSprite(sFOWBottom, tileTable["shadowBottom"], 0, 31)

  drawSpriteToSprite(sFOWLeft, tileTable["shadowTopLeft"], 0, 0)
  drawSpriteToSprite(sFOWTop, tileTable["shadowTopLeft"], 0, 0)

  drawSpriteToSprite(sFOWRight, tileTable["shadowTopRight"], 31, 0)
  drawSpriteToSprite(sFOWTop, tileTable["shadowTopRight"], 0, 0)

  drawSpriteToSprite(sFOWLeft, tileTable["shadowBottomLeft"], 0, 0)
  drawSpriteToSprite(sFOWBottom, tileTable["shadowBottomLeft"], 0, 31)

  drawSpriteToSprite(sFOWRight, tileTable["shadowBottomRight"], 31, 0)
  drawSpriteToSprite(sFOWBottom, tileTable["shadowBottomRight"], 0, 31)

  freeSprite(sFOWBottom)
  freeSprite(sFOWTop)
  freeSprite(sFOWLeft)
  freeSprite(sFOWRight)
end

function freeTileFragments(tileTable)
  for k, v in pairs(tileTable) do
    freeSprite(v)
  end
end

function displayEarnings(x, y)
  if (earnTime > ms) then
    setTextSize(2)
    if (earnValue > 0) then
      drawString("+" .. earnValue, x, y + (earnTime - ms) * 0.01)
    else
      drawString("" .. earnValue, x, y + (earnTime - ms) * 0.01)
    end
  else
    earnValue = 0
  end
end

tiles = {}
stage = prefsGetInt("stage", 1)
initTileFragments(tiles, "gfx/tilefragments/tile_dirt" .. stage .. ".bmp")

sRobotIdle = loadSprite("gfx/idle.bmp", 0, 0x0001)
sRobotDig = loadSprite("gfx/dig.bmp", 0, 0x0001)
sRobotFunnel = loadSprite("gfx/funnel.bmp", 0, 0x0001)

sBigDigger = loadSprite("gfx/bigdigger.bmp", 0, 0xf81f)
sBigDigger2 = loadSprite("gfx/bigdigger2.bmp", 0, 0xf81f)

sTileOreIron = loadSprite("gfx/tilefragments/tile_ore_iron.bmp", 0, 0x0001)
sTileOreCopper = loadSprite("gfx/tilefragments/tile_ore_copper.bmp", 0, 0x0001)
sTileOreEmerald = loadSprite("gfx/tilefragments/tile_ore_emerald.bmp", 0, 0x0001)
sTileOreGold = loadSprite("gfx/tilefragments/tile_ore_gold.bmp", 0, 0x0001)
sTileOreDiamond = loadSprite("gfx/tilefragments/tile_ore_diamond.bmp", 0, 0x0001)

sTileRock = loadSprite("gfx/tilefragments/tile_rock.bmp", 0, 0x0001)

sItemLantern = loadSprite("gfx/item_lantern.bmp", 0, 0x0001)

sExplosionAnim = loadAnimSprite("gfx/explosion-0.bmp", 80, 48, 0, 0x0001)

math.randomseed(ms)

worldMapArray = {}
for x = 0, 7 do
  worldMapArray[x] = {}
  for y = 0, 5 do
    genTile(x,y)
  end
end
worldMapArray[2][0] = 1
worldMapArray[2][1] = 1
mineTile(2, 0)
mineTile(2, 1)

yDigOffset = 0
playerX = 2
playerXOffset = 0
isCurrentlyTouching = false
stepsLeft = 2
money = prefsGetInt("money", 0)
earnValue = 0
earnTime = 0
fullVisibilityTime = 0


-- Inhalation game
function getDistancePointLine(l1x, l1y, l2x, l2y, px, py)
  return math.abs(((l2y - l1y) * px - (l2x - l1x) * py + l2x * l1y - l2y * l1x) / math.sqrt((l2y - l1y) * (l2y - l1y) + (l2x - l1x) * (l2x - l1x)))
end

inhalationCrosshairX = 85
inhalationCrosshairY = 135
inhalationVectorX = 1
inhalationVectorY = 0
targetPositions = {{210.0, 120.0}, {200.0, 50.0}, {150.0, 20.0}}
targetEffectiveness = {0.0, 0.0, 0.0}
targetType = {2, 2, 2}
typeSprites = {0, sTileOreIron, sTileOreCopper, sTileOreEmerald, sTileOreGold, sTileOreDiamond}
wasBlowing = false

-- Shortblows
rockImageLoaded = -1

--[[
-- 0 = tunnel
-- 1 = dirt
-- 2 = iron
-- 3 = copper
-- 4 = emerald
-- 5 = gold
-- 6 = diamond
-- 7 = rock
]]
