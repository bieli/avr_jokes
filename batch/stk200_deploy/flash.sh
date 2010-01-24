#!/bin/sh

# zainicjowanie portu szeregowego

#### dodanie gdy nie bedzie widoczny w systemie
sudo mknod /dev/parport0 c 99 0
sudo chmod a+rw /dev/parport0

sudo modprobe parport
sudo modprobe ppdev


# odczytanie i backup plku programu z urzadzenia
sudo avrdude -P /dev/parport0 -c stk200 -p t2313 -U flash:r:last_proram_backup.hex:i -F

# zapisanie pliku programu do urzadzenia
sudo avrdude -P /dev/parport0 -c stk200 -p t2313 -U flash:w:$1:i -F
