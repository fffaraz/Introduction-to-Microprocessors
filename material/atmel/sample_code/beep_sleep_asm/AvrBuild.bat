@ECHO OFF
"C:\Program Files\Atmel\AVR Tools\AvrAssembler2\avrasm2.exe" -S "E:\course\4723\atmel\sample_code\beep_sleep_asm\labels.tmp" -fI -W+ie -o "E:\course\4723\atmel\sample_code\beep_sleep_asm\beep_sleep_asm.hex" -d "E:\course\4723\atmel\sample_code\beep_sleep_asm\beep_sleep_asm.obj" -e "E:\course\4723\atmel\sample_code\beep_sleep_asm\beep_sleep_asm.eep" -m "E:\course\4723\atmel\sample_code\beep_sleep_asm\beep_sleep_asm.map" "E:\course\4723\atmel\sample_code\beep_sleep_asm\beep_sleep.asm"
