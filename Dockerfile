FROM debian:buster

ENV SERVER_PATH="/etc/webserv/"

ADD [".", "/etc/webserv/"]

WORKDIR ["/tmp"]

COPY ["./config/setup.sh", "/tmp"]
COPY ["./config/setup_mysql.sql", "/tmp"]

RUN apt update; \
    apt install wget \
                mysql-client \
                mysql-server \
                php \
                php-mysql \
            &&  wget -c https://www.wordpress.org/latest.tar.gz \
            &&  tar -xfv latest.tar.gz \
            &&  mv wordpress/* /var/www/html/
            &&  mysql -u root -p < /etc/setup_mysql.sql

EXPOSE 80

ENTRYPOINT [ "./setup.sh" ]
