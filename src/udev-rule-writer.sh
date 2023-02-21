#!/usr/bin/python3

"""
Copyright (C) 2023 Debayan Sutradhar (rnayabed) (debayansutradhar3@gmail.com)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
"""

import json, sys, random, os

if len(sys.argv) != 7:
    print('Insufficient arguments')
    sys.exit()

keyboards_list_path = sys.argv[1]
udev_rule_path = sys.argv[2]
exit_file = sys.argv[3]
exit_ok = sys.argv[4]
exit_keyboards_list_read_failed = sys.argv[5]
exit_udev_rule_write_failed = sys.argv[6]

def exit(code):
    f = open(exit_file, 'w')
    f.write(f'{str(random.random())[2:]}:{code}')
    f.close()
    sys.exit()

try:
    list_file = open(keyboards_list_path,'r')
    list = json.load(list_file)
    list_file.close()
except:
    exit(exit_keyboards_list_read_failed)

vid = list['vid']

content='# Rangoli udev rules\n'

for pid in list['pids']:
    content += f'KERNEL=="hidraw*", ATTRS{{idVendor}}=="{vid}", ATTRS{{idProduct}}=="{("0" * (4 - len(pid))) + pid}", TAG+="uaccess"\n'


try:
    rule_file = open(udev_rule_path, 'w')
    rule_file.write(content)
    rule_file.close()
except:
    exit(exit_udev_rule_write_failed)

os.system('udevadm trigger -c change')

exit(exit_ok)



