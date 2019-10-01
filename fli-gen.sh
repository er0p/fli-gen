#!/bin/bash

#set -x

csv_file=$1

possible_roles="(Центральный защитник|Вратарь|Полузащитник|Крайний защитник|Защитник|Универсал|Форвард)"

function usage() {
	echo "`basename $0`: file.csv"
}

if [ ! -f "${csv_file}" ] || [ ! -r "${csv_file}" ] ; then
	usage
	exit 64
fi

:> input.txt

skip=4
cnt=0
line=0
while read -r var
do
	((line++))
	((skip--))
	if [ $skip -gt 0 ] ; then
		continue
	fi
	tmp=$(echo "$var")
	if [[ "${tmp}" == *"Команды"* ]] ; then
		break
	fi
	full_name=$(echo $tmp | awk -F ',' '{print $1}')
	name=$(echo $full_name | awk '{print $1}')
	fname=$(echo $full_name | awk '{print $2}')

	team=$(echo $tmp | awk -F ',' '{print $(NF-2)}')

	if [[ "${team}" == *"Лазурные"* || "${team}" == *"Жёлтые"* || "${team}" == *"Красные"* ]] ; then
		raw_role=$(echo $tmp | sed -r -e 's/^.*(Центральный защитник|Вратарь|Полузащитник|Крайний защитник|Защитник|Универсал|Форвард.*$)/\1/' | grep -oP '^.*?[0-9]+' | sed -r -e  "s/\"{0,1},[0-9]$//g")
#		echo ${name}
#		echo ${tmp}
#		raw_role=$(echo $tmp | grep -oP "(?i)${possible_roles}.*?\"{0,1},")
#		echo ${raw_role}
		role=$(echo $raw_role | sed -r -e  "s/\s|\"//g" | sed -r -e  "s/,$//g" | awk '{print tolower($0)}' | sed -r -e  "s/\"{0,1},[0-9]$//g")
#		echo ${role}
		#rate=$(echo $tmp | grep -oP "${raw_role},?\"?[0-9]?,{1}[0-9]?" | grep -oP "[0-9].*$" | sed -r -e "s/,$//g" | sed -r -e "s/,/./g" )
		rate=$(echo $tmp | grep -oP "${raw_role},?\"?[0-9]?,{1}[0-9]{0,1}[^0-9]{1}" | grep -oP "[0-9].*$" | sed -r -e "s/\"?$//g" | sed -r -e "s/,*$//g" | sed -r -e "s/,/./g" )
#		echo ${rate}
		#echo $rate | tee -a numbers.in
		#echo "${line} name:\"${name}\" role:\"${role}\" rate:\"${rate}\" team:\"${team}\""
		echo "${line} ${rate} ${role} ${name} ${fname}" | tee -a input.txt
		((cnt++))
	fi

done < "$csv_file"

echo "total: ${cnt} footballers"


./fli-gen

date

exit 0
