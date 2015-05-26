#!/bin/bash 

export SSHPASS=../sshpass-1.05/sshpass

echo "put *.res" > script
echo "exit" >> script

while [ 1 ]; do

  killall test_ecdlp

  $SSHPASS -p Bo4hae7eemooy7 sftp ul01@eccbreaker.student.iaik.tugraz.at:/incoming < script

  rm *.res

  COUNTER=0
  while [  $COUNTER -lt $1 ]; do
    export filename=$(hostname -f)-$(date +"%y-%m-%d-%T").res

    echo "generating $filename"
    ./bin/test_ecdlp < ../ecdlp_challenges/ecdlp_ITSEC2015_88.chal > $filename &
    sleep 1
    let COUNTER=COUNTER+1 
  done

  sleep 3600

done
