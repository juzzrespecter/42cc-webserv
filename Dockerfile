FROM debian:buster

ENV SERVER_PATH="/var/www/webserv/"

ADD [".", "/var/www/webserv/"]

COPY ["./config/docker-entrypoint.sh", "/tmp"]
COPY ["./config/setup_mysql.sql", "/tmp"]

RUN apt-get update \
    && apt-get install wget \
       	       	   curl \
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
    &&  chmod +x ./docker-entrypoint.sh

COPY ["./config/docker.conf", "/var/www/webserv/config/webserver.conf"]

EXPOSE 8080/tcp

WORKDIR "/var/www/webserv/"

ENTRYPOINT [ "/tmp/docker-entrypoint.sh" ]
