ip=$(arp -a | grep -e patchbox -e pisound | awk '{printf $2}' | tr -d '()' )
scp -i ~/.ssh/pisound  main.cpp graphics.hpp channel.hpp Makefile patch@$ip:looper2
