ip=$(arp -a | grep -e patchbox -e pisound | awk '{printf $2}' | tr -d '()' )
scp -i pisound  main.cpp Makefile patch@$ip:looper2
