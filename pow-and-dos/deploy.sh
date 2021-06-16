#!/usr/bin/env bash

trap 'exit 255' INT

number=( 7 8 9 )
session="hw5-pow"

echo "[-] cleaning up ..."
for num in "${number[@]}"; do
    remote="chiffoncake@linux"$num".csie.ntu.edu.tw"
    ssh $remote 'tmux ls | grep '$session' && tmux kill-session -t '$session' || true'
done
ssh $remote 'rm -rfv ~/Downloads/pow-and-dos'
echo "[-] successfully cleaned up"

echo "[-] copying files ..."
scp -r ../pow-and-dos $remote:~/Downloads
echo "[-] finished copying files"

for num in "${number[@]}"; do
    remote="chiffoncake@linux"$num".csie.ntu.edu.tw"
    echo "[-] -----------------------------------------------"
    echo "[-] remote = linux"$num
    ssh $remote 'cd ~/Downloads/pow-and-dos && nohup make clean && nohup make && nohup tmux new-session -d -s '$session' "./server 0.0.0.0 13087"'
    echo "[-] successfully launched the service"
    echo
done
