function isNextToTunnel(x, y)
  return (x>0 and worldMapArray[x-1][y]==0) or (x<7 and worldMapArray[x+1][y]==0) or (y>0 and worldMapArray[x][y-1]==0) or (y<5 and worldMapArray[x][y+1]==0) or (x>0 and y>0 and worldMapArray[x-1][y-1]==0) or (x<7 and y>0 and worldMapArray[x+1][y-1]==0) or (x>0 and y<5 and worldMapArray[x-1][y+1]==0) or (x<7 and y<5 and worldMapArray[x+1][y+1]==0)
end

function drawTile(x, y, yOffset, showHidden)
  if (worldMapArray[x][y] == 0) then
    if (worldMapArray[x][y-1] == 0 or y == 0) then -- up
      if (x > 0 and worldMapArray[x-1][y] == 0) then -- left
        if (x < 7 and worldMapArray[x+1][y] == 0) then -- right
          if (worldMapArray[x][y+1] == 0) then -- down
            drawSprite(sTileTunnelX, x*40, y*40 - yOffset)
          else -- up, left, right, not down
            drawSprite(sTileTunnelT, x*40, y*40 - yOffset)
          end
        else -- up, left, not right
          if (worldMapArray[x][y+1] == 0) then -- down
            drawSprite(sTileTunnelTLeft, x*40, y*40 - yOffset)
          else -- not down
            drawSprite(sTileTunnelLLeft, x*40, y*40 - yOffset)
          end
        end
      else -- up, not left
        if (x < 7 and worldMapArray[x+1][y] == 0) then -- right
          if (worldMapArray[x][y+1] == 0) then -- down
            drawSprite(sTileTunnelTRight, x*40, y*40 - yOffset)
          else -- not down
            drawSprite(sTileTunnelLRight, x*40, y*40 - yOffset)
          end
        else -- up, not left, not right
          if (worldMapArray[x][y+1] == 0) then -- down
            drawSprite(sTileTunnelI, x*40, y*40 - yOffset)
          else -- not down
            drawSprite(sTileTunnel, x*40, y*40 - yOffset)
          end
        end
      end
    else -- not up
      if (x > 0 and worldMapArray[x-1][y] == 0) then -- left
        if (x < 7 and worldMapArray[x+1][y] == 0) then -- right
          if (worldMapArray[x][y+1] == 0) then -- down
            drawSprite(sTileTunnelTDown, x*40, y*40 - yOffset)
          else -- not up, left, right, not down
            drawSprite(sTileTunnelFlat, x*40, y*40 - yOffset)
          end
        else -- not up, left, not right
          if (worldMapArray[x][y+1] == 0) then -- down
            drawSprite(sTileTunnelFLeft, x*40, y*40 - yOffset)
          else -- not down
            drawSprite(sTileTunnelLeft, x*40, y*40 - yOffset)
          end
        end
      else -- not up, not left
        if (x < 7 and worldMapArray[x+1][y] == 0) then -- right
          if (worldMapArray[x][y+1] == 0) then -- down
            drawSprite(sTileTunnelFRight, x*40, y*40 - yOffset)
          else -- not down
            drawSprite(sTileTunnelRight, x*40, y*40 - yOffset)
          end
        end
      end
    end
  elseif (showHidden or isNextToTunnel(x, y)) then
    if (worldMapArray[x][y] == 1) then
      drawSprite(sTileDirt, x*40, y*40 - yOffset)
    elseif (worldMapArray[x][y] == 2) then
      drawSprite(sTileOreIron, x*40, y*40 - yOffset)
    elseif (worldMapArray[x][y] == 3) then
      drawSprite(sTileOreCopper, x*40, y*40 - yOffset)
    elseif (worldMapArray[x][y] == 4) then
      drawSprite(sTileOreEmerald, x*40, y*40 - yOffset)
    elseif (worldMapArray[x][y] == 5) then
      drawSprite(sTileOreGold, x*40, y*40 - yOffset)
    elseif (worldMapArray[x][y] == 6) then
      drawSprite(sTileOreDiamond, x*40, y*40 - yOffset)
    end
  end
end

function mineTile(x,y)
  r = 0
  if (worldMapArray[x][y] == 2) then
    r = 1
  elseif (worldMapArray[x][y] == 3) then
    r = 2
  elseif (worldMapArray[x][y] == 4) then
    r = 3
  elseif (worldMapArray[x][y] == 5) then
    r = 4
  elseif (worldMapArray[x][y] == 6) then
    r = 5
  end
  if (r>0) then
    earnValue = r
    earnTime = ms + 2000
    money = money + r
  end
  worldMapArray[x][y] = 0
  return r
end

function genTile(x, y)
  v = math.random(0, 100)
  if (v < 5) then
    worldMapArray[x][y] = 3
  elseif (v < 15) then
    worldMapArray[x][y] = 2
  else
    worldMapArray[x][y] = 1
  end
end

sRobotIdle = loadSprite("gfx/idle.bmp")
sRobotDig = loadSprite("gfx/dig.bmp")
sRobotFunnel = loadSprite("gfx/funnel.bmp")
sTileDirt = loadSprite("gfx/tile_dirt.bmp")
sTileOreIron = loadSprite("gfx/tile_ore_iron.bmp")
sTileOreCopper = loadSprite("gfx/tile_ore_copper.bmp")
sTileOreEmerald = loadSprite("gfx/tile_ore_emerald.bmp")
sTileOreGold = loadSprite("gfx/tile_ore_gold.bmp")
sTileOreDiamond = loadSprite("gfx/tile_ore_diamond.bmp")
sTileTunnel = loadSprite("gfx/tile_tunnel.bmp")
sTileTunnelI = loadSprite("gfx/tile_tunnel_I.bmp")
sTileTunnelFlat = loadSprite("gfx/tile_tunnel_flat.bmp")
sTileTunnelLeft = loadSprite("gfx/tile_tunnel_left.bmp")
sTileTunnelRight = loadSprite("gfx/tile_tunnel_left.bmp", 1)
sTileTunnelLLeft = loadSprite("gfx/tile_tunnel_L_left.bmp")
sTileTunnelLRight = loadSprite("gfx/tile_tunnel_L_left.bmp", 1)
sTileTunnelFLeft = loadSprite("gfx/tile_tunnel_L_left.bmp", 2)
sTileTunnelFRight = loadSprite("gfx/tile_tunnel_L_left.bmp", 3)
sTileTunnelT = loadSprite("gfx/tile_tunnel_T.bmp")
sTileTunnelTDown = loadSprite("gfx/tile_tunnel_T.bmp", 2)
sTileTunnelTLeft = loadSprite("gfx/tile_tunnel_T_left.bmp")
sTileTunnelTRight = loadSprite("gfx/tile_tunnel_T_left.bmp", 1)
sTileTunnelX = loadSprite("gfx/tile_tunnel_X.bmp")

sExplosionAnim = loadAnimSprite("gfx/explosion-0.bmp", 80, 48)

math.randomseed(ms)

worldMapArray = {}
for x = 0, 7 do
  worldMapArray[x] = {}
  for y = 0, 5 do
    genTile(x,y)
  end
end
worldMapArray[2][0] = 0
worldMapArray[2][1] = 0
worldMapArray[2][2] = 0

--[[worldMapArray[4][1] = 2
worldMapArray[4][2] = 3
worldMapArray[4][3] = 4
worldMapArray[5][1] = 5
worldMapArray[5][2] = 6]]

yDigOffset = 0
playerX = 2
playerXOffset = 0
isCurrentlyTouching = false
stepsLeft = 2
money = prefsGetInt("money", 0)
earnValue = 0
earnTime = 0

rockImageLoaded = -1

--[[
-- 0 = tunnel
-- 1 = dirt
-- 2 = copper
-- 3 = iron
-- 4 = emerald
-- 5 = gold
-- 6 = diamond
]]

