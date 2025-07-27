# ICMP Ping Project in C

This project, completed as part of my university coursework, consists of implementing a simplified **ping** utility in C language.

---

## Objectives

- Understand and work with low-level network protocols (IP, ICMP)  
- Manually construct IP and ICMP headers  
- Calculate checksums  
- Send packets using raw sockets  
- Receive and analyze ICMP responses

---

## How it works

The program builds a complete ICMP Echo Request packet, sends it to a target IP address, then waits for and displays the received ICMP reply.

---

## Prerequisites

- Unix/Linux or macOS system  
- Root privileges (required to create raw sockets)  
- C compiler (gcc, clang, etc.)

---

## Usage

The program requires elevated privileges to open a raw socket.  
Compile and run it with `sudo`:

```bash
gcc -o ping_icmp ping_icmp.c
sudo ./ping_icmp
```

> ⚠️ **Note**: The source and destination IP addresses are hardcoded in the program.  
> Make sure they match your network configuration so the packet can be sent correctly.  
> For example, `src_ip` must be an IP assigned to one of your network interfaces, otherwise the packet will be rejected.

---

## 📄 License

This project is under the MIT license. See the [LICENSE](LICENSE) file for more information.

---

## 👤 Author

**Mark Ballereau**  
[LinkedIn](https://www.linkedin.com/in/markblre) | [GitHub](https://github.com/markblre)