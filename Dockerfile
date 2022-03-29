FROM debian:buster

ENV SERVER_PATH="/var/www/webserv/"

ADD [".", "/var/www/webserv/"]

COPY ["./config/docker-entrypoint.sh", "/tmp"]
COPY ["./config/setup_mysql.sql", "/tmp"]

RUN apt-get update \
    && apt-get install wget \
                   make \
                   clang \
                   default-mysql-server \
                   php \
                   php-cgi \
                   php-mysql -y \
    &&  cd /tmp \               
    &&  wget -c https://www.wordpress.org/latest.tar.gz \
    &&  tar -xvf latest.tar.gz \
    &&  mkdir /var/www/html \
    &&  mv wordpress/* /var/www/html/ \
    &&  service mysql start \
    &&  mysql -u root -p < /tmp/setup_mysql.sql \
    &&  mkdir $SERVER_PATH/cgi-bin/ \
    &&  cp $(which php-cgi) $SERVER_PATH/cgi-bin/ \
    &&  chmod +x ./docker-entrypoint.sh

COPY ["./config/docker.conf", "/var/www/webserv/config/webserver.conf"]

EXPOSE 80

WORKDIR "/var/www"

ENTRYPOINT [ "/tmp/docker-entrypoint.sh" ]
