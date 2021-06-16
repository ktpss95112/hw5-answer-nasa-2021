#!/usr/bin/env bash

trap 'exit 255' INT

number=( 7 8 9 )
session="hw5-pow"

echo "[-] cleaning up ..."
for num in "${number[@]}"; do
    remote="chiffoncake@linux"$num".csie.ntu.edu.tw"
    ssh $remote 'tmux ls | grep '$session' && tmux kill-session -t '$session' || true'
done
ssh $remote 'rm -rfv ~/Downloads/pow-and-dos' \
&& echo "[-] successfully cleaned up"
