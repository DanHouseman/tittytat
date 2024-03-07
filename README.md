# TITTYTAT

A C program that creates a communication channel between two pseudo-terminal devices and manages data transfer between them. It is designed to run on systems supporting POSIX terminal interfaces, using mechanisms like posix_openpt, grantpt, unlockpt, and ptsname_r for terminal manipulation. This program is a solid foundation for pseudo-terminal based communication. 

## Program Flow
### Initialization: The program defines constants for buffer size (BUF_SIZE) and baud rate (BAUD_RATE). It then declares prototypes for functions that open pseudo-terminal master devices (ptym_open), configure serial port settings (configure_serial), and copy data between file descriptors (copy_data).

### Main Function:

1. It declares buffers for storing names of master-slave pseudo-terminal pairs (master1, slave1, master2, slave2) and file descriptors for these terminals (fd1, fd2).
2. It opens two pairs of master-slave pseudo-terminals using ptym_open.
    1. If at least three arguments are provided to the program, it attempts to create symbolic links from the command-line arguments to the slave devices.
    2. This is useful for applications that need to interact with terminal devices using specific file paths.
3. It configures the serial settings (like baud rate and character size) for both master pseudo-terminals using configure_serial.
4. Enters an infinite loop where it uses select to monitor the master pseudo-terminals for incoming data. 
5. When data is available on one, it copies it to the other using copy_data.

### ptym_open Function:

1. Opens a new pseudo-terminal master device,
    1. sets it to non-blocking mode,
    2. and obtains its slave device's name.
2. Performs error handling for each step and uses ptsname_r for a thread-safe way to obtain the slave device's name.

### configure_serial Function:

1. Fetches and then modifies the terminal I/O settings for a pseudo-terminal master device to raw mode, 
    1. sets the input and output baud rate to B9600, and
    2. ensures the terminal can read data and ignores modem control lines.
2. Applies the new settings immediately with tcsetattr and flushes both input and output queues.

### copy_data Function:

1. Continuously reads data from one file descriptor and writes it to another in a non-blocking fashion.
    1. It handles partial writes and retries as necessary, while also 
    2. managing errors related to read/write operations.
  
### Observations and Suggestions
__Error Handling:__ The program includes basic error handling, especially in ptym_open and configure_serial, ensuring resources like file descriptors are closed in case of failure.  
__Non-blocking I/O:__ By using non-blocking I/O and select, the program efficiently waits for data to be available for reading or writing, reducing CPU usage.  
__Data Copying Logic:__ In copy_data, there's careful management of partial writes and error conditions, important for maintaining data integrity during transfer.  

### Potential Improvements
__Magic Numbers:__ While BAUD_RATE is defined, there are still hardcoded values (like CS8, CLOCAL, CREAD) that could be defined as constants for clarity.  
__Robust Error Checking:__ More comprehensive error checking and handling could be implemented, especially after operations like read, write, tcsetattr, and in the main loop to handle potential failures more gracefully.  
**Resource Cleanup:** Ensure that resources (e.g., file descriptors) are properly released in all error paths to prevent resource leaks.  
**Modularity:** The program is well-structured, but further breaking down of functionality into more granular functions could improve readability and maintainability.  
