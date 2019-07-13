# Practice2
## Неre presented summer practice in MAI on 2nd course. 

### Server run

Before compilation server you shoud add MySQL Server and MySQL Connector C++ (8.0 version all) to your include-path 
and also take your project mode to "Rlease 64-bit".

Before running you must point your MySQL login data in main function of server.cpp and create MySQL database "users" with table "persons" 
which contains 3 rows: (id INT UNSIGNED PRIMARY KEY AUTO_INCREMENT, login CHAR(30) NOT NULL, password BIGINT UNSIGNED NOT NULL). 


### Client run 

Before client compilation you shoud take your project configuration to "64-bit". 

You need to point server host and port of connetion, which you can find after server run. That needs for correct connection.

### Principe of works

Server can works with many clients. Each client processed in separate thread. Server stores in MySQL database all logins of authorised users and also all unread messages. 
After client request on reading messages deletes from server.

Client use Standart WinAPI Forms for construct windows interface.

Conection beetwen server and user creared in WinAPI sockets (winsock2). Messages encripts with algorithm RSA. 

Google and VK authorisation doesn't work yet.
##### Thith work used only for education purposes. For more information please report.pdf
#### Made by Bronnikov Maksim and Zhivih Maksim. 
