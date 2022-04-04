FROM debian:buster

ENV SERVER_PATH="/var/www/webserv/"
ENV DB_NAME="wp_database"
ENV DB_USER="wp_user"
ENV DB_PASSWORD="wp_passwd"

ADD [".", "/var/www/webserv/"]

COPY ["./config/docker-entrypoint.sh", "/tmp"]

RUN apt-get update \
    && apt-get install wget \
       	       	   curl \
                   make \
                   clang \
                   default-mysql-server \
                   php \
		   php-curl \
		   php-dom \
		   php-exif \
		   php-fileinfo \
		   php-imagick \
		   php-json \
		   php-mbstring \
		   php-xml \
		   php-zip \
                   php-cgi \
                   php-mysql -y \
    &&  cd /tmp \               
    &&  wget -c https://www.wordpress.org/latest.tar.gz \
    &&  tar -xvf latest.tar.gz \
    &&  mkdir -v /var/www/html \
    &&  mv -v wordpress/* /var/www/html/ \
    &&  mv /var/www/html/wp-config-sample.php      /var/www/html/wp-config.php \
    &&  sed -i -e "s/database_name_here/$DB_NAME/" /var/www/html/wp-config.php \
    	       -e "s/username_here/$DB_USER/"      /var/www/html/wp-config.php \
    	       -e "s/password_here/$DB_PASSWORD/"  /var/www/html/wp-config.php \
    &&  chmod +x ./docker-entrypoint.sh \
    &&  rm -rfv /tmp/wordpress/

EXPOSE 8080/tcp

WORKDIR "/var/www/webserv/"

ENTRYPOINT [ "/tmp/docker-entrypoint.sh" ]
