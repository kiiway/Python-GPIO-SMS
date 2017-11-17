import sys
import os
import re
from time import sleep

import gammu
import RPi.GPIO as gpio

gpio.setwarnings(False)
gpio.setmode(gpio.BCM)

gm = gammu.getStateMachine()
gm.ReadConfig()
gm.Init()

def send_sms(sms_target, sms_content):
    message = {
        'Text': sms_content,
        'Unicode':True,
        'SMSC': {'Location': 1},
        'Number': sms_target,
    }
    gm.SendSMS(message)

def sms_command(sms_content):
    gpio_regex = re.match(r"gpio:([0-9]{1,2});([0-9]{1})", sms_content, re.M|re.I)
    if(gpio_regex):
        gpio.setup(gpio_regex.group(1), gpio.OUT)
        if gpio_regex.group(2):
            gpio.output(gpio_regex.group(1), gpio.HIGH)
        else:
            gpio.output(gpio_regex.group(1), gpio.LOW)
    

def main():
    while 1:
        try:
            getSms = gm.GetNextSMS(Start=True, Folder=0)
            #getSms = gm.GetNextSMS(Location=getSms[0]['Location'], Folder=0)
            sms_command(getSms[0]['Text'])
            gm.DeleteSMS(getSms[0]['Folder'], getSms[0]['Location'])
        except gammu.ERR_EMPTY:
           pass
        sleep(0.150)

if __name__ == '__main__':
    main()
