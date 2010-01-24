#!/usr/bin/sh

MMCU=atmega8
REL_PATH=`pwd`
MSG_HASH="[ INFO ] --- "

#DEPLOY_MODE=$2
DEPLOY_MODE=dev
APP_PATH=apps/$DEPLOY_MODE/$1
APP_FILE_PATH=$APP_PATH/$1.c
DEPLOY_DIR=batch/avrdoper_deploy


echo "$MSG_HASH START"
echo "$MSG_HASH Plik projektu: '$APP_FILE_PATH'"
echo "$MSG_HASH Status       : '$DEPLOY_MODE'"

cd $APP_PATH

echo "$MSG_HASH Usuwanie rezultatow ostatniej kompilacji ..."
rm *.lst *.elf *.o *.map

echo "$MSG_HASH Kompilacja, linkowanie, konsolidacja ..."
echo "$MSG_HASH jestem w katalogu '`pwd`'"

avr-gcc -g -Os -mmcu=$MMCU -c $1.c
avr-gcc -g -Os -mmcu=$MMCU -c $1.c
avr-gcc -g -mmcu=$MMCU -o $1.elf  $1.o
avr-objdump -h -S $1.elf > $1.lst
avr-gcc -g -mmcu=$MMCU -Wl,-Map,$1.map -o $1.elf $1.o
avr-objcopy -j .text -j .data -O ihex $1.elf $1.hex
avr-objcopy -j .eeprom --change-section-lma .eeprom=0 -O ihex $1.elf $MMCU_$1_eeprom.hex

echo "$MSG_HASH Kopiowanie wyników *.hex do '$REL_PATH/$DEPLOY_DIR' z `pwd`..."

cp $1.hex $REL_PATH/$DEPLOY_DIR
cp $MMCU_$1_eeprom.hex $REL_PATH/$DEPLOY_DIR

echo "$MSG_HASH Wgrywanie programu do urzadzenia..."

cd $REL_PATH/$DEPLOY_DIR
sh flash.sh $1.hex

echo "STOP"
cd ../../

