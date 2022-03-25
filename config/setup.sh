#!/bin/bash

service php7-fpm start
service mysql start
cd ${SERVER_PATH}
make
./webserver
