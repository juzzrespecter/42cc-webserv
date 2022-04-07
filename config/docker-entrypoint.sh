
#!/bin/bash

# sshd setup
useradd peter
echo "peter:1234" | chpasswd
service ssh start

# mariaDB configuration setup
service mysql start
mysql -u root <<EOF
CREATE DATABASE $DB_NAME;
GRANT ALL ON $DB_NAME.* TO '$DB_USER'@'localhost' IDENTIFIED BY '$DB_PASSWORD';
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
sudo -u www-data ./webserver docker.conf
# exec ./webserver docker.conf # uncomment when finished testing
