# Shared-Memory-in-Linux
Objectives:

To use shared memory across cooperating processes.
To use semaphores for mutual exclusion between processes.
To catch signals (such at SIGINT) with a signal handler.
To create a dynamically shared library.

Overview

We will implement a client and server that communicate through a shared memory segment to display statistics about the client processes. All of the processes are running on the same machine. Each client process will periodically write to the shared memory segment with updates about its recent behavior (e.g., how much progress it has made and how much CPU it has been allocated); the server process collects this information (by reading from the shared memory segment) and periodically displays the information for all the client processes.

When processes (or threads) cooperate through shared memory, they require synchronization primitives (such as locks or semaphores) to ensure that they do not have race conditions when they are each simultaneously updating the same memory locations. To minimize our need for synchronization, we will construct the clients and server so that only a single client is (usually) writing to each memory location and the server is only reading (not writing); if the server happens to occasionally read data that is not up-to-date, that is okay, since the data is just usage statistics (and not your bank account). The only time you will need to worry about mutual exclusion will be when clients are first starting and when they exit.

We will be implementing three components: a server process that displays client statistics every second, a library containing two functions for updating those statistics, and a client process that sleeps and computes at rates specified by command-line arguments. We see how the Linux scheduler treats processes at different priorities and with different behavior.
