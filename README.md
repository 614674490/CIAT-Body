# CIAT

## envirment

- raspberrypi

```txt
Raspberry Pi 4B 4G
bullseye 64bit lite
```

- RaspberryPi & TX2

| Raspberry_id  | MAC               |BLE                |
| ------------- | ----------------- |------------------ |
| Raspberry_1   | dc:a6:32:29:3b:5d | DC:A6:32:63:3B:5E |
| Raspberry_2   | dc:a6:32:63:02:dc | DC:A6:32:63:02:DE |
| Raspberry_3   | e4:5f:01:00:25:2c | 3.3V              |
| Raspberry_4   | e4:5f:01:00:24:da | 3.3V              |

| TX_id  | Redis         | WIFI    |
| ------ | ------------- |-------- |
| TX_1   | 192.168.1.102 | hotpot  |
| TX_2   | 192.168.1.134 | hotpot2 |
| TX_3   | 192.168.1.136 | hotpot3 |
| TX_4   | 192.168.1.148 | hotpot4 |

```sh
cd ~/Work/CIAT_Body/raspberrypi
sudo cp wpa_supplicant.conf /etc/wpa_supplicant/wpa_supplicant.conf

wpa_cli -i wlan0 add_network
wpa_cli -i wlan0 set_network 1 ssid \"hotpot\"
wpa_cli -i wlan0 set_network 1 psk \"12345678\"
wpa_cli -i wlan0 set_network 1 key_mgmt WPA-PSK
wpa_cli -i wlan0 set_network 1 id_str \"TX2\"
wpa_cli -i wlan0 set_network 1 priority 2
wpa_cli -i wlan0 enable_network 1
wpa_cli -i wlan0 select_network 1
wpa_cli -i wlan0 status
wpa_cli -i wlan0 save_config

sudo wpa_cli list_network
sudo wpa_cli -i wlan0 select_network 1
sudo wpa_cli -i wlan0 status

sudo cp sources.list /etc/apt/sources.list
sudo cp raspi.list /etc/apt/sources.list.d/raspi.list
sudo apt update -y && sudo apt dist-upgrade -y
sudo apt-get upgrade -y

sudo apt-get install vim -y
sudo apt-get install git -y
sudo apt-get install cmake -y
sudo apt-get install g++ -y
sudo apt-get install gcc -y
```

- GPIO

| name     | BCM gpio |
| -------- | ------   |
| VCC      | 3.3V     |
| JY_TXD   | GPIO4    |
| JY_RXD   | GPIO45   |
| RT_TXD   | GPIO40   |
| RT_RXD   | GPIO41   |
| ADC_SCL  | SCL.1    |
| ADC_SDA  | SDA.1    |

- WiringPi

```sh
cd ~/Download/WiringPi
sudo sh ./build
gpio -v
```

- raspidjson

```sh
cd ~/Download/rapidjson
mkdir build
cd build
cmake ..
make
```

- IIC&Usart

```sh
sudo raspi-config
--> 3.Inferfacing Options
sudo apt-get install i2c-tools
sudo i2cdetect -y 1

sudo vim /boot/config.txt
dtparam=i2c_arm=on
dtoverlay=uart2
dtoverlay=uart3
```

- Bluetooth

```sh
sudo /etc/init.d/bluetooth restart
sudo service bluetooth start

systemctl status bluetooth
sudo bluetoothctl
discoverable on
pairable on
agent on
default-agent

hciconfig
sudo hciconfig hci0 -name raspberrypi_2
sudo hciconfig hci0 up
sudo hciconfig hci0 piscan
sudo rfcomm watch hci0

sudo apt-get install pi-bluetooth bluez bluez-firmware blueman
sudo usermod -G bluetooth -a pi
sudo vi /etc/systemd/system/dbus-org.bluez.service
ExecStart=/usr/libexec/bluetooth/bluetoothd -C
ExecStartPost=/usr/bin/sdptool add SP

trust 94:E9:EE:AD:E2:52
```

- Redis

```sh
git clone https://github.com/redis/hiredis
cd hiredis
make
sudo make install
sudo ldconfig /usr/local/lib
```

- StartUp

```sh
cd ./raspberrypi
sudo cp CIATClient /etc/init.d/CIATClient
sudo chmod 777 /etc/init.d/CIATClient

sudo update-rc.d CIATClient defaults
sudo update-rc.d -f CIATClient remove

sudo service CIATClient start
sudo service CIATClient stop

```

- exe

```sh
g++ -g /home/pi/Work/CIAT_Body/source/*.cpp -I /home/pi/Work/CIAT_Body/include -lpthread -lwiringPi -lhiredis -o /home/pi/Work/CIAT_Body/bin/CIAT_Body
```
