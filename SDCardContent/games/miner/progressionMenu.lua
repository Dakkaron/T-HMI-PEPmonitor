disableCaching()
setTextSize(3)
drawString("Shop", 10, 30, 0xFFFF)
setTextSize(2)
drawString("Geld: $" .. money, 170, 10, 0xFFFF)

setTextSize(2)

if (stage == 1) then
  if (money < 2000) then
    fillRect(10, 60, 220, 35, 0xF800)
  else
    fillRect(10, 60, 220, 35, 0x001F)
  end
  
  drawString("Tiefer graben: $2000", 15, 65, 0xFFFF)
  if (money >= 2000 and isTouchInZone(10, 60, 220, 35)) then
    money = money - 2000
    stage = 2
  else
    setTextSize(1)
    drawString("Nicht genug Geld", 15, 84, 0xFFFF)
    prefsSetInt("stage", stage)
    prefsSetInt("money", money)
  end
elseif (stage == 2) then
  if (money < 10000) then
    fillRect(10, 60, 220, 35, 0xF800)
  else
    fillRect(10, 60, 220, 35, 0x001F)
  end

  drawString("Tiefer graben: $10000", 15, 65, 0xFFFF)
  if (money >= 10000 and isTouchInZone(10, 60, 220, 35)) then
    drawString("Kaufen", 10, 95, 0xFFFF)
    money = money - 10000
    stage = 3
    prefsSetNumber("stage", stage)
    prefsSetNumber("money", money)
  else
    setTextSize(1)
    drawString("Nicht genug Geld", 15, 84, 0xFFFF)
  end
end

setTextSize(2)
fillRect(220, 200, 100, 40, 0x001F)
drawString("Fertig", 245, 215, 0xFFFF)
if (isTouchInZone(220, 200, 100, 40)) then
  closeProgressionMenu()
end