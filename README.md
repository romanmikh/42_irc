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

