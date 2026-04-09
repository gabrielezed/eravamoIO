# eravamoIO

## English

*It was me, Nicotina Nick, Grimy Joe and Megan Chill.*

eravamoIO (eravamoInputOutput) it's a backend module write in C that brings a Nivra inside joke to an enterprise level.
Input mode is used to store a new name in a SQLite database.
Output mode generates a sentence of the type "It was me, NameX, NameY, NameZ", retrieving random names from the SQLite database.

### Usage
Start with ./eravamoIO -h to see help.

### Dependencies
This project uses [SQLite](https://www.sqlite.org/index.html) for data storage. 
The database engine is integrated using the [amalgamation](https://www.sqlite.org/amalgamation.html) 
source files (`sqlite3.c` and `sqlite3.h`), which are included directly in the repository.

This project also use [GNU Libmicrohttpd](https://www.gnu.org/software/libmicrohttpd/) for the networking part. It won't compile if you haven't it installed on your machine.
