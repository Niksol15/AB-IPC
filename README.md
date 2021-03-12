# AB-IPC

I was thinking for a long time choosing the best way to organize IPC: through named pipes or
through sockets, but in the end I decided to use TCP sockets.

### how to build
For B:
```
    cd B
    mkdir build && cd build
    cmake ..
    cmake --build .
    ./B
```
The same for A

B must be run before A
