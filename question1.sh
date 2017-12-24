#!/bin/bash
# This is the solution to Q1

declare -a students # store the student ID to find out duplication
declare -i i
i=1


cat "$HOME/matching.txt" | while read -r line # read each line in matching.txt
do 
    name=`echo $line | cut -d' '  -f 1` # find out the original file name
    id=`echo $line | cut -d' ' -f 2` # find out the matched ID
    code=`echo $line | cut -d' ' -f 3` # find out the programme code should in
    
    
    if [ ! -f "$HOME/student_photo/"$name ]; then
        # check wheather the photo exist
        echo "$name does not exist, so it cannot be renamed and moved."
        continue
    fi

    if ! echo "$id" | grep -q '^[0-9]\+$'; then
        # check wheather the ID is valid
        echo "$id is an invalid 8-digit student ID, so $name is not renamed and moved."
        continue
    else
        if [ ! ${#id} -eq 8 ] ; then
            echo "$id is an invalid 8-digit student ID, so $name is not renamed and moved."
            continue
        fi
    fi

    if ! echo "$code" | grep -q '^[0-9]\+$'; then
        # check wheather the programme code is valid
        echo "$code is an invalid 4-digit programme code, so $name is not renamed and moved."
        continue
    else
        if [ ! ${#code} -eq 4 ]; then
            echo "$code is an invalid 4-digit programme code, so $name is not renamed and moved."
            continue
        fi
    fi

    if ! echo "${students[@]}" | grep -w $id &>/dev/null; then
        # check wheather the ID is repeated
        students[$i]=$id
        i=i+1
    else
        echo "$id is a duplicate student ID, so $name is not renamed and moved."
        continue
    fi
    

    name="$HOME/student_photo/"$name
    # the original path
    students[$i]=id
    code="$HOME/student_photo/"$code

    if [ ! -d $code ]; then
        # if the folder do not exist, creat it
        mkdir $code
    fi

    path=$code"/"$id".jpg"
    mv $name $path # move to the new folder and rename
done