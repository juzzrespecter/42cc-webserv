#!/bin/bash

service php7-fpm start
service mysql start
make -c ${SERVER_PATH}
./webserver