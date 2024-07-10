-- SQL DB creation and startup
DROP DATABASE IF EXISTS veda;
DROP USER IF EXISTS 'veda_dev'@'localhost';
CREATE DATABASE veda;
CREATE USER IF NOT EXISTS 'veda_dev'@'localhost' IDENTIFIED BY 'veda_dev_pwd';
GRANT ALL PRIVILEGES ON veda.* TO 'veda_dev'@'localhost';
FLUSH PRIVILEGES;
