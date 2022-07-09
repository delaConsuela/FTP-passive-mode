# FTP-passive-mode

### About the project
Network course project: implementation of an FTP server in passive mode

### Breif description of the files
Server.c handles the server-side portion of socketing
Client.c handles the client-side part instead
modules.h contains all the global variables and prototypes of the various methods used in modules.c
modules.c there is the whole implementation of the various procedures
Bug: after doing a put, the input buffer also interprets the carriage return as a command (not fixed yet)
