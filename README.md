# best-server-ever
The best factorial calculating server ever.

#### Shit Happens
Pthread was easy to make, but forks are such a shit concept
and are in general, very unwieldy. My signal handler got a 
SIGSEGV, lol.

#### /lib Folder Magic
Just move the server and the factorial code to use header files.
Now this is based.

#### Select Breaks An Infinite Loop
Go off, queen. Maybe ignoring SIGPIPE isn't the best idea people
have had in a while.        
Update: It was me. I put a return statement in the infinite loop.
I have fucked up so hard lol.

## Anyways
To redo this project,
```
mkdir build
cd build
cmake ..
make Server_<type>
./Server_<type>
```
To review the packet traffic, use `scripts/tcpdumping.sh`.  
Warning- very not portable.
