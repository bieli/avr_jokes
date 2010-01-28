<?php

function set_led_mask_1from10_7442($led_mask, $delay)
{
    $n = 0;
    echo "\n\n";
    $led_mask_shift = $led_mask;
    $b = 1;

//    for ( n = 0; n < delay; n++ )
    {
        for ( $n = 0; $n < 8; $n++ )
        {
            $b *= 2;
//            if  ( $b & ( $led_mask_shift << $n ) )
            if  ( $b & ( $led_mask_shift << 1 ) )
            {
//                set_led_1from10_7442($n);
//                printf("set_led_1from10_7442(%d); - BIT: %d\n", $n, (int) ( $b & ( $led_mask_shift << 1 ) ));
                echo '1';
            }
            else
            {
                echo '0';                
            }
        }

//        delay_ms(1);
    }
}

set_led_mask_1from10_7442(85, 100);

