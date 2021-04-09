# timings-and-system-calls-in-C
Instructions:
Please refer to page 63 of the textbook (Chapter 6 - Page 16 if you are downloading the textbook chapter by chapter) for the description of the homework.
Please submit the code that you have developed, along with a short report that explains your code and your findings.
The deadline for the submission is September 13th.

Report:
Overall this project was quite a challenge yet enjoyable. The system call measuring part was relatively easy but the context switching proved to be a challenge for me. I asked my friend for a pointer on where to start and he recommended that I google c functions and their meanings. This is why if you refer to the code, there are an abundant number of comments explaining each step of the way so that if someone who has just started learning operating systems, like myself, starts to look at this code, they may understand with relative ease the meaning and importance of each line.
Because of this approach, I feel very comfortable with all the c code that I submitted and could most likely easily recreate it from scratch with minimal reference material such as official documentation.
To talk about the code for a little, I simply get the system time using gettimeofday and pass in a pointer to a timeval struct to store it in. I then call getpid() “loopyLoopNum” times and then get the average of the number of system calls in micro seconds and print it out to the console.
The context switching was way harder but it was eventually completed. The main things to test context switching were establishing a child and parent process using fork, and then setting up an if-else condition so that the parent and child process each do something different. Both of the processes read from and write to different ends of a pipe between the processes. The parent, however, is the one that times the context switches and calculates the average at the end. This average is then outputted to the console. The most important thing I learned from this was the usage of the sched_setaffinity call to make sure both processes run on the same core so that the processor must switch between two processes instead of using a different core for each process.






Sample output shown below:<br/>
![](https://i.imgur.com/N35VEd0.png)

				 
References:

I used this video to help understand how to use fork in combination with reading and writing to different ends of an established pipe between two processes.
https://www.youtube.com/watch?v=Mqb2dVRe0uo&b&ab_channel=CodeVault

Additionally, I used the official Ubuntu documentation on commands to understand and take notes on the commands I needed for this homework.
http://manpages.ubuntu.com/
