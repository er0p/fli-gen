#/bin/sh
#set -x
. curl.env 

if [ ! -z $CUSTOM_SHEET_ID ] ; then 
	sheetID=$CUSTOM_SHEET_ID
	echo "CUSTOM_SHEET_ID = $CUSTOM_SHEET_ID"
fi


players=$(curl -s -X GET "https://sheets.googleapis.com/v4/spreadsheets/${sheetID}/values/'Игроки'!A:A?key=${key}&majorDimension=COLUMNS")

rates=$(curl -s -X GET "https://sheets.googleapis.com/v4/spreadsheets/${sheetID}/values/'Игроки'!J:J?key=${key}&majorDimension=COLUMNS")

roles=$(curl -s -X GET "https://sheets.googleapis.com/v4/spreadsheets/${sheetID}/values/'Игроки'!E:E?key=${key}&majorDimension=COLUMNS")

defending_skill=$(curl -s -X GET "https://sheets.googleapis.com/v4/spreadsheets/${sheetID}/values/'Игроки'!F:F?key=${key}&majorDimension=COLUMNS")
attack_skill=$(curl -s -X GET "https://sheets.googleapis.com/v4/spreadsheets/${sheetID}/values/'Игроки'!G:G?key=${key}&majorDimension=COLUMNS")
passing_skill=$(curl -s -X GET "https://sheets.googleapis.com/v4/spreadsheets/${sheetID}/values/'Игроки'!H:H?key=${key}&majorDimension=COLUMNS")
phys_cond=$(curl -s -X GET "https://sheets.googleapis.com/v4/spreadsheets/${sheetID}/values/'Игроки'!I:I?key=${key}&majorDimension=COLUMNS")
birthday=$(curl -s -X GET "https://sheets.googleapis.com/v4/spreadsheets/${sheetID}/values/'Игроки'!N:N?key=${key}&majorDimension=COLUMNS")

#curl -X GET "https://sheets.googleapis.com/v4/spreadsheets/${sheetID}?key=${key}&fields=sheets.properties"

#row_1=$(curl -s -X GET "https://sheets.googleapis.com/v4/spreadsheets/${sheetID}/values/'Игроки'!1:1?key=${key}&majorDimension=COLUMNS")
#date=$(echo $row_1 | jq -r ".values[-3]")

laz=$(curl -s  -X GET "https://sheets.googleapis.com/v4/spreadsheets/${sheetID}/values/'Игроки'!A1:XX?key=${key}&majorDimension=COLUMNS")
#echo $laz | jq ".values[-3]" | awk '{print i++" "$0}' | grep "Лазурные"

search_pattern="(Красные|Лазурные"
if [ ! -z "$ALL_CANDIDATES" ] ; then
	search_pattern="$search_pattern""|?|??"
	echo "ALL_CANDIDATES is set"
fi
search_pattern="${search_pattern})"
#echo "search pattern = ${search_pattern}"
ind=$(echo $laz | jq ".values[-4]" | awk 'BEGIN{i=-1}{print i++" "$0}' | grep -E "${search_pattern}" | awk '{print $1}')
#ind=$(echo $laz | jq ".values[-6]" | awk 'BEGIN{i=-1}{print i++" "$0}' | grep -E "(Красные|Лазурные)" | awk '{print $1}')
#echo $laz
#bash
#echo $ind > /tmp/test
#bash

:> input.txt
for i in $ind ;
do
	id=$(echo $((i+1)))
	rate=$(echo $rates   | jq  ".values[0]" | jq  ".[$i]" | sed "s/^\"//g" | sed "s/\"$//" | sed "s/,/./")
	as=$(echo $attack_skill  | jq  ".values[0]" | jq  ".[$i]" | sed "s/^\"//g" | sed "s/\"$//" | sed "s/,/./")
	ds=$(echo $defending_skill  | jq  ".values[0]" | jq  ".[$i]" | sed "s/^\"//g" | sed "s/\"$//" | sed "s/,/./")
	ps=$(echo $passing_skill  | jq  ".values[0]" | jq  ".[$i]" | sed "s/^\"//g" | sed "s/\"$//" | sed "s/,/./")
	pc=$(echo $phys_cond  | jq  ".values[0]" | jq  ".[$i]" | sed "s/^\"//g" | sed "s/\"$//" | sed "s/,/./")
	bd=$(echo $birthday  | jq  ".values[0]" | jq  ".[$i]" | sed "s/^\"//g" | sed "s/\"$//" | sed "s/,/./")
	role=$(echo $roles   | jq  ".values[0]" | jq  ".[$i]" | sed "s/^\"//g" | sed "s/\"$//" | tr -d "[:space:]" | sed 's/.*/\L&/g')
	player=$(echo $players | jq  ".values[0]" | jq  ".[$i]" | sed "s/^\"//g" | sed "s/\"$//" | awk '{print $1" "$2}')
	#echo $id $rate $role $player | tee -a input.txt
	echo $id $rate $role $player $as $ds $ps $pc $bd >>  input.txt
done
