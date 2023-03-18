#! /bin/bash

cd ~/Work/CIAT_Body/raspberry

sudo cp wpa_supplicant.conf /etc/wpa_supplicant/wpa_supplicant.conf
sudo cp CIATClient /etc/init.d/CIATClient
sudo chmod 777 CIATClient
sudo cp sources.list /etc/apt/sources.list
sudo cp raspi.list /etc/apt/sources.list.d/raspi.list

sudo apt update -y && sudo apt dist-upgrade -y
sudo apt-get upgrade -y

sudo apt-get install vim -y
sudo apt-get install git -y
sudo apt-get install cmake -y
sudo apt-get install g++ -y
sudo apt-get install gcc -y

cd ~/Download/WiringPi
sudo sh ./build
gpio -v

cd ~/Download/rapidjson
mkdir build
cd build
cmake ..
make

cd ~/Download/hiredis
make
sudo make install
sudo ldconfig /usr/local/lib

sudo raspi-config
sudo vim /boot/config.txt

g++ -g /home/pi/Work/CIAT_Body/source/*.cpp -I /home/pi/Work/CIAT_Body/include -lpthread -lwiringPi -lhiredis -o /home/pi/Work/CIAT_Body/bin/CIAT_Body

cd ~/Work/CIAT_Body/raspberrypi
sudo cp CIATClient /etc/init.d/CIATClient
sudo chmod 777 /etc/init.d/CIATClient

sudo update-rc.d CIATClient defaults

sudo reboot
