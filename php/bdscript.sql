CREATE DATABASE sensor_data;

USE sensor_data;

CREATE TABLE readings (
    id INT AUTO_INCREMENT PRIMARY KEY,
    temperature FLOAT NOT NULL,
    humidity FLOAT NOT NULL,
    reading_time TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
