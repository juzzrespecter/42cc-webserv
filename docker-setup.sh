#!/bin/bash

DOC_IMAGE=docker-webserver
DOC_CONT=docker-webserver
SUDO=""

if [ $(uname) = "Linux" ]; then
    SUDO="sudo"
fi

if [ "$1" = "clean" ]; then
    $SUDO docker rm $($SUDO docker ps -a | grep $DOC_CONT |  awk '{print $1}')
    $SUDO docker rmi $($SUDO docker images | grep $DOC_IMAGE | awk '{print $1}')
    exit 0;
fi

make fclean

if [[ -z $($SUDO docker images | grep $DOC_IMAGE | awk '{print $1}') ]]; then
    $SUDO docker build . -t $DOC_IMAGE
fi

$SUDO docker run -p 8080:8080 --rm --name $DOC_CONT -it $DOC_IMAGE
