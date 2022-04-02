#!/bin/bash

# mariaDB configuration setup
service mysql start
mysql -u root <<EOF
CREATE DATABASE wp_database;
GRANT ALL ON wp_database.* TO 'wp_user'@'localhost' IDENTIFIED BY 'wp_passwd';
FLUSH PRIVILEGES;
EOF

# webserver configuration setup
cat <<EOF > ${SERVER_PATH}docker.conf
server {
       listen 8080 ;

       accept_method GET POST ;
       
       index index.html index.php ;

       root /var/www/html/ ;

       location / {
       		cgi_pass php /usr/bin/php-cgi ;
       }
}
EOF

# test
echo "<?php phpinfo(); ?>" > /var/www/html/info.php

# webserver initialization
cd ${SERVER_PATH}
make
./webserver docker.conf
