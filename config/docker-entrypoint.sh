#!/bin/bash

# mariaDB configuration setup
service mariadb start
mysql -u root <<EOF
CREATE DATABASE wp_database;
GRANT ALL ON wp_database.* TO 'wp_user'@'localhost' IDENTIFIED BY 'wp_passwd';
FLUSH PRIVILEGES;
EOF

# webserver initialization
cd ${SERVER_PATH}
make
tail -f /var/www/webserv/config/webserver.conf
#./webserver
