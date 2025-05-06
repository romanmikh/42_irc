# 42 IRC 

A small Internet Relay Chat (IRC) server built in C++. It can be used with used with different clients (IRSSI, NC, etc) locally, over a network, or using the host's public IP. 

![screen capture](https://i.imgur.com/XAC0tXR.png)

## Features

- Seamless communication between clients
- Make channels, grant/remove admin rights, set channel parameters
- !quote to query the inspirational bot API

## Installation and Usage

1. Clone & launch server:
   ```sh
   git clone https://github.com/romanmikh/42_irc.git 42_irc
   cd 42_irc
   make
   ./ircserv <port> <password>
   ```

2. Install IRSSI & launch client:
   ```sh
   # install ninja (irssi dependency)
   git clone https://github.com/ninja-build/ninja.git && cd ninja
   git checkout release && cd ..

   # install meson (irssi dependency)
   sudo apt install meson

   # install irssi
   wget https://codeberg.org/irssi/irssi/releases/download/1.4.5/irssi-1.4.5.tar.xz
   tar xJf irssi-1.4.5.tar.xz
   cd irssi-1.4.5
   sudo apt install cmake libssl-dev libglib2.0-dev libutf8proc-dev libncurses-dev
   meson Build
   ninja -C Build && sudo ninja -C Build install

   irssi
   ```

   From within IRSSI, execute:
   ```sh
   /connect localhost <port> <password>  # this can be done from multiple clients
   ```

3. Valgrind output saved in valgrind_out.txt after:
   ```sh
   ulimit -n 1024 # necessary once only, to address 'file descriptor too high' error
   make valgrind
   ```

## Helpful Resources
1. [mIRC]https://www.mirc.co.uk/ the OG app we're recreating
2. [en.wikipedia.org/wiki/IRC]https://en.wikipedia.org/wiki/IRC
3. [High-level Guide]https://reactive.so/post/42-a-comprehensive-guide-to-ft_irc/
4. [Official IRC Protocol]https://modern.ircdocs.horse/

This project was a collaboration between edenjamsalem, romanmikh and kellyhayd.
