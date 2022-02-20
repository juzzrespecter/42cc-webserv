CREATE DATABASE wp_database;
GRANT ALL ON wp_database.* TO 'wp_user'@'localhost' IDENTIFIED BY 'wp_passwd';
FLUSH PRIVILEGES;