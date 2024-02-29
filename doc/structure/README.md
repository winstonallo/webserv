# project structure

## problems

### blocking operations
**_the server must be non-blocking_**
* **problem:** 
    * thread is waiting for an operation to complete before proceeding
    * can lead to performance bottlenecks, especially with **multiple connections**
    * **example:**
        * ``I/O operations``: in a blocking model, the thread waits until a write operation is over, making the thread inactive for an undefined period
* **solution: ``asynchronous I/O``**
    * allows a server to initiate a I/O operation while continuing with other tasks
       * this way, a thread can manage multiple connections at the same time
    * asynchronous I/O functions include ``select``, ``poll``, ``epoll``
    * ``select`` is the most portable across different operating systems
#### using select 

```select``` is a UNIX system call that allows asynchronous monitoring of multiple file descriptors

1. **initialize ``fd_set``**
    * initialize fd_set structures for each type of event (read, write)
    * they will serve as bit masks representing sets of file descriptors
        * bit is set: ready for operation
        * bit is not set: busy
2. **populate the set**
    * use macro ```FD_SET``` to add file descriptors to the set
3. **set the timeout**
    * set ``timeout value`` to determine the maximum duration ```select``` should wait for en event
    * if the timeout elapses with no event, ```select``` returns -> program can proceed
4. **call ``select``**
    * call ```select``` with args (man select):
        * ```max_fd + 1```
        * ```fd_sets```
        * ```timeout```
5. **block/return**
    * ```select``` enters a blocking state
    * monitors the file descriptor until:
        * event
        * timeout
6. **check ``fd_set``**
    * program examines the ```fd_set``` to identify which fd's are ready for events
7. **handle events**
    * respond to events by executing actions based on the file descriptors, for example:
        * read/write data from a socket
        * handle errors
8. **loop/exit**
    * employ a loop to repeat the monitoring process

### implement HTTP/1.1

**_message parsing -> understand request - generate response_**

1. **request line**

    ```method URI HTTP-version``` 
    * **example:** 
        * ```GET /path/to/resource HTTP/1.1```
        * **_GET_** is the method
        * **_path/to/resource_** is the URI
        * **_HTTP/1.1_** is the HTTP version
2. **status line/response line**
    
    ```HTTP-version status-code ´status-message```
    * **example:**
        * ```HTTP/1.1 200 OK```
        * **_HTTP/1.1_** is the HTTP version
        * **_200_** is the status code
        * **_OK_** is the message

3. **headers**

    requests/responses can include headers, which provide information about the message