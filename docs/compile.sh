#!/usr/bin/sh

echo "START"
echo "Compliking, linkin, hex files generating for $1 ..."

cd $1

rm *.hex *.lst *.elf *.o *.map

avr-gcc -g -Os -mmcu=at90s2313 -c $1.c
avr-gcc -g -Os -mmcu=at90s2313 -c $1.c
avr-gcc -g -mmcu=at90s2313 -o $1.elf  $1.o
avr-objdump -h -S $1.elf > $1.lst
avr-gcc -g -mmcu=at90s2313 -Wl,-Map,$1.map -o $1.elf $1.o
avr-objcopy -j .text -j .data -O ihex $1.elf $1.hex
avr-objcopy -j .eeprom --change-section-lma .eeprom=0 -O ihex $1.elf $1_eeprom.hex
cp $1.hex ../stk200_deploy
echo "STOP"
cd ..

