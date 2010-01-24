#!/bin/sh

# odczytanie i backup pliku programu z urzadzenia
sudo avrdude -P avrdoper -c stk500v2 -p m8 -U flash:r:last_program_backup.hex:i -F

# zapisanie pliku programu do urzadzenia
# z resetem
sudo avrdude -P avrdoper -c stk500v2 -p m8 -U flash:w:$1:i -F -E reset

# bez resetu
#sudo avrdude -P avrdoper -c stk500v2 -p m8 -U flash:w:$1:i -F


