# ft_irc

**ft_irc** is a C++98 IRC server project built for the 42 school curriculum.

---

## 📚 Project Overview

Internet Relay Chat (IRC) is a text-based protocol for real-time messaging, supporting both public and private communication. This project implements an IRC server, allowing multiple clients to connect, join channels, send messages, and manage channel modes—mimicking the essential behavior of an official IRC server.

---

## 🛠️ Features

- **Multi-client Support:** Handle multiple simultaneous client connections using non-blocking I/O and a single poll (or equivalent).
- **TCP/IP Communication:** IPv4 and IPv6 support.
- **Core IRC Functionality:**  
  - User authentication (password, nickname, username)
  - Private and channel messaging
  - Channel creation and management
  - Channel operator privileges
- **Supported Channel Modes:**  
  - `i`: Invite-only channel
  - `t`: Topic changes restricted to channel operators
  - `k`: Channel password (key)
  - `o`: Operator privilege management
  - `l`: User limit per channel
- **Operator Commands:**  
  - `KICK` – Remove a client from a channel
  - `INVITE` – Invite a client to a channel
  - `TOPIC` – Change/view channel topic
  - `MODE` – Change channel modes
- **Robust Error Handling:** Gracefully manages partial/fragmented data and connection issues.
- **IRC Client Compatibility:** Fully compatible with popular IRC clients like irssi and nc.

---

## 🚀 Getting Started

### **Building**

```sh
make
```

### **Running the Server**

```sh
./ircserv <port> <password>
```
- `<port>`: Port number to listen on (e.g., 6667 or 8080)
- `<password>`: Connection password required by clients

_Example:_
```sh
./ircserv 8080 mypassword
```

---

## 💻 Connecting with Clients

### **Using irssi**
```sh
irssi -c localhost -p 8080 -n mynick -w mypass
```

### **Using netcat (nc)**
```sh
nc localhost 8080
```
Then manually enter IRC commands such as:
```
PASS mypassword
NICK mynick
USER myuser 0 * :My User
JOIN #mychannel
```

---

## 📝 Example IRC Commands

| Command                    | Example usage                   |
|----------------------------|---------------------------------|
| Set invite-only            | MODE #chan +i                   |
| Set password               | MODE #chan +k secretpass        |
| Give operator privilege    | MODE #chan +o nick              |
| Set user limit             | MODE #chan +l 10                |
| Restrict topic changes     | MODE #chan +t                   |
| Remove invite-only         | MODE #chan -i                   |
| Remove password            | MODE #chan -k                   |
| Remove operator privilege  | MODE #chan -o nick              |
| Remove user limit          | MODE #chan -l                   |

---

## ⚙️ Project Requirements (Summary)

- **No forking**—single non-blocking poll (or equivalent) for all I/O.
- **No external/Boost libraries.**
- **C++98 standard compliance.**
- **Robust—should not crash under any circumstance.**
- **Reference client compatibility required (choose your own, e.g., irssi).**
- **Makefile with standard rules (`all`, `clean`, `fclean`, `re`).**

---

## 🏆 Implemented Bonus Ideas

- File transfer support
- IRC bot functionality

---

## 👨‍💻 Authors

- [**Tudor Ursescu**](https://github.com/Tudor-Ursescu)

- [**Hryhorii Zakharchenko**](https://github.com/grysha11)

- [**Tudor Lupu**](https://github.com/DRACULATudor)

---

## 📄 License

This project is for educational purposes within the 42 school curriculum.

---