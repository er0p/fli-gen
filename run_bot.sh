#!/bin/bash
WD="/root/src/fli-gen"
cd $WD

kill `cat /root/src/fli-gen/pid`
echo $PWD >> /tmp/t.log
export BOT_TOKEN=`cat ./bot_token.txt`
echo $BOT_TOKEN >> /tmp/t.log
./fli-gen-bot -d
exit 0
