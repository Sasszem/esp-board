if get_pressed(1,1) then
   lcd_backlight(false)
   lcd_display(false)
   disabled = true
end

if get_pressed(1,2) then
    lcd_backlight(true)
    lcd_display(true)
    disabled = false
end


if not disabled then
  lcd_set_cursor(2,1)
  lcd_print(millis()/1000)
end
