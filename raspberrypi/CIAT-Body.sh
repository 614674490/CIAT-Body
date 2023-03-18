#! /bin/bash
###
 # @Author: Ken Kaneki
 # @Date: 2021-11-16 22:02:56
 # @LastEditTime: 2022-06-16 16:19:05
 # @Description: README
###
Socket_IP="192.168.0.102"
Socket_PORT=50001
Redis_IP="192.168.0.102"
Redis_PORT=6379
Redis_PW="ciat_xd"
sudo wpa_cli -i wlan0 select_network 1

while !(ping -c 1 -w 3 $Socket_IP >/dev/null)
do
    echo "$Socket_IP ping failed, Reconnect"
    sudo wpa_cli -i wlan0 select_network 1
done

echo "$Socket_IP ping success"

cd /home/pi/Work/CIAT_Body/
./bin/CIAT_Body --socket_ip $Socket_IP --socket_port $Socket_PORT --redis_ip $Redis_IP --redis_port $Redis_PORT --redis_pw $Redis_PW

echo "Sample End"
exit 0

