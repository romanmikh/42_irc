<h1 align="center"> 📡 ft_irc </h1>

<p align="center">
<img alt="Static Badge" src="https://img.shields.io/badge/42-London-orange"> <img alt="Static Badge" src="https://img.shields.io/badge/language-C++98-blue">  <img alt="Static Badge" src="https://img.shields.io/badge/submitted_in-2025-green">
</p>

>This project is about creating your own IRC server using C++ 98. You will use an actual IRC client to connect to your server and test its functionality.

<h2> ✏ About </h2>
ft_irc is a project that involves developing an IRC server that allows multiple clients to connect, authenticate, and communicate through private and public channels. The implementation follows the IRC protocol using TCP/IP communication.


<h2> ⚙ Requirements </h2>

- The server must use **non-blocking I/O** (poll or equivalent).
- No **forking** is allowed.
- The following system functions are authorized:
  - `socket`, `close`, `setsockopt`, `bind`, `connect`, `listen`, `accept`
  - `htons`, `htonl`, `ntohs`, `ntohl`, `inet_addr`, `inet_ntoa`
  - `send`, `recv`
  - `signal`, `sigaction`
  - `poll` (or equivalent like `select()`, `kqueue()`, `epoll()`)
  - `lseek`, `fstat`, `fcntl`
- **External libraries and Boost libraries are forbidden**.

<h2>  📝 Instructions </h2>

- The server must be started as:
`./ircserv <port> <password>`
- `<port>`: The port number the server listens on.
- `<password>`: The required password for client authentication.

1. SSH:
   ```sh
    git clone https://github.com/kellyhayd/ft_irc.git ircserv
    cd ircserv
    make
    ./ircserv <port> <password>
   ```

2. Valgrind output saved in valgrind_out.txt after:
   ```sh
   ulimit -n 1024 # necessary once only, to address 'file descriptor too high' error
   make valgrind
   ```

<h2>  ✅ Features </h2>

- The server should:
  - Support multiple clients simultaneously.
  - Handle **non-blocking** I/O operations using a **single poll() (or equivalent)**.
  - Maintain proper communication between the server and connected clients via **TCP/IP**.
  - Work seamlessly with a real **IRC client**.
- The following core IRC functionalities must be implemented:
  - Authentication, setting a nickname and username.
  - Joining and communicating in channels.
  - Sending and receiving **private messages**.
  - Message broadcasting to all channel members.
  - User roles: **Operators and regular users**.
- Implementing **operator-specific commands**:
  - `KICK` - Ejects a client from a channel.
  - `INVITE` - Invites a client to a channel.
  - `TOPIC` - Changes or views the channel topic.
  - `MODE` - Modifies channel settings:
    - `i` - Invite-only mode.
    - `t` - Restricts `TOPIC` changes to operators.
    - `k` - Sets/removes a channel password.
    - `o` - Grants/removes operator privileges.
    - `l` - Sets/removes a user limit.

- The server must **handle malformed requests** and **partial data reception**.
- Ensure that the server aggregates **packets correctly** before processing a command.


<h2> 🏗 Implementation Notes </h2>

- The server must **handle malformed requests** and **partial data reception**.
- Ensure that the server aggregates **packets correctly** before processing a command.

<h2> 🌟 Bonus Part </h2>

<h4> ⭐ Additional Features </h4>

- **File transfer support** between clients.
- **IRC bot** that can automate tasks.

<h2> 📚 Helpful Resources </h2>

1. https://reactive.so/post/42-a-comprehensive-guide-to-ft_irc/ (high-level guide)
2. https://en.wikipedia.org/wiki/IRC
3. https://modern.ircdocs.horse/ (official IRC protocol guide)
4. https://stackoverflow.com/questions/10654286/why-should-i-use-non-blocking-or-blocking-sockets
5. https://libera.chat/
6. [IRC God](https://ircgod.com/) Blog about the ft_irc by 42 students
7. [Medium ft_irc](https://medium.com/@afatir.ahmedfatir/small-irc-server-ft-irc-42-network-7cee848de6f9)
8. [Channels and Command Management](https://medium.com/@mohamedsarda2001/ft-irc-channels-and-command-management-ff1ff3758a0b)
9. [TCP Server](https://www.youtube.com/watch?v=cNdlrbZSkyQ&list=PLUJCSGGiox1Q-QvHfMMydtoyEa1IEzeLe&index=3) Creating the basic of a TCP server
10. [Network Programming](https://www.youtube.com/watch?v=gntyAFoZp-E) Sockets
