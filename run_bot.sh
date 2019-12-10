#!/bin/bash
kill `cat ./pid`
export BOT_TOKEN=`cat bot_token.txt`
./fli-gen-bot -d
