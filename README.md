# IAW-Port-Scanner
- Port scanner client-server using libwebsock and TCP-sockets non-blocking in C (linux)

## Prerequisites

 - libevent
    ```sh
        apt-get install libevent-dev
    ```
 - openssl
    ```sh
        apt-get install libssl-dev
    ```
 - libwebsock
    ```sh
      git clone git://github.com/payden/libwebsock.git
      tar -xvzf libwebsock-1.0.4.tar.gz
    ```

## Building libwebsock
- Antes de compilar, borrar de lo siguientes archivos de libwebsock "../src/utf.h" && "../src/utf.c": 'inline'
    ```sh
    ./autogen.sh
    ./configure && make && sudo make install
    ```
## Building and running Scan-port server
- Once you have built and installed libwebsock, built
    ```sh
        gcc Apuertos.c list.c -g -O2 -o server server.c -lwebsock 
    ```    
- Configure dynamic linker run-time bindings
    ```sh
        ldconfig
    ```    
- Run port example 4567
    ```sh
         ./server 4567
    ```    
### Credits

C library for WebSockets servers [libwebsock](https://github.com/payden/libwebsock)
