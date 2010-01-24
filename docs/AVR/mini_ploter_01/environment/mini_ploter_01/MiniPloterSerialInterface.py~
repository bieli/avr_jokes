#!/usr/bin/env python
# -*- coding: utf-8 -*-

import serial


"""
Send byte to serial port example: 

>>> ser = serial.Serial('/dev/ttyS0', 9600, timeout=1)
>>> ser.write("1")
"""

class MiniPloterSerialInterface(object):
    CMD_SET_HOME_X  = 'x'
    CMD_SET_HOME_Y  = 'y'
    CMD_SET_UP_X    = 'l'
    CMD_SET_DOWN_X  = 'j'
    CMD_SET_UP_Y    = 'i'
    CMD_SET_DOWN_Y  = 'k'
    CMD_SET_UP_X_SPEED    = 't'
    CMD_SET_DOWN_X_SPEED  = 'g'
    CMD_SET_UP_Y_SPEED    = 'f'
    CMD_SET_DOWN_Y_SPEED  = 'h'

    CMD_TEST_1      = '1'
    CMD_TEST_2      = '2'
    CMD_TEST_3      = '3'
    CMD_TEST_4      = '4'

    CMD_SET_MOTOR_SPEED_5   = '5'
    CMD_SET_MOTOR_SPEED_7   = '6'
    CMD_SET_MOTOR_SPEED_10  = '7'
    CMD_SET_MOTOR_SPEED_20  = '8'
    CMD_SET_MOTOR_SPEED_50  = '9'


    serial_obj = None
    portNumber = 0
    boudRate = 9600
    timeout = 1
    cmd = ''
    last_cmd = ''

    def __init__(self, portNumber = 0, boudRate = 9600, timeout = 1):
        self.portNumber = portNumber
        self.boudRate = boudRate
        self.timeout = timeout
        print '/dev/ttyS' + str(self.portNumber)
        self.serial_obj = serial.Serial('/dev/ttyS' + str(self.portNumber), self.boudRate, timeout = self.timeout)

    def send_cmd(self, cmd):
        self.cmd = cmd
        self.last_cmd = self.cmd
        self.serial_obj.write(self.cmd)

