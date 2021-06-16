# Problem 5: WiFi Hacking

## Environment setup

The victim:
* [LinkIt™ 7697](https://labs.mediatek.com/en/platform/linkit-7697)
* runs `linkit7697/linkit7697.ino` on the board
* flag: `HW5{Jo3_Tsa1-7he_M4st3r_0F_Tra1niNg}`

WiFi AP:
* web configuration username: `super`
* web configuration password: `$!cc82p5`
* static WAN IP address: 10.88.5.23
* SSID: `Palace of Joe Tsai`
* WiFi password: `0918273645`

Website:
* run `main.go` on linux5
* tcp: listen on linux5.csie.ntu.edu.tw:6887
* current status is on http://linux5.csie.ntu.edu.tw:18080/
* flag: `HW5{j0e_ts4I_1s_d0ub1e_gun_k4i's_b3st_fr13nD}`
* files: linux5:/tmp2/chiffoncake/
* only allow access from WiFi AP
* If the the victim is disconnected for too long, disable the service. (the system might be broken or someone is cheating)

## Writeup

https://hackmd.io/@uqzWTXyyTk6IYTBwcPwnoA/SJL-sCIsd#5-WiFi-Hacking

