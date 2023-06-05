#!/bin/bash

sudo insmod my_driver.ko
sudo mknod /dev/simple_character_device c 242 0
sudo chmod 777 /dev/simple_character_device