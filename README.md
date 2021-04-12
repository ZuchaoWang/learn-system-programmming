# learn-system-programmming

This is the C code for system programming, accompanying the [system programming course](https://www.youtube.com/watch?v=6OSeJFo6GOc) from LinuxHint.

Although the code is intended for Linux, it should run in most UNIX-like system. In fact, I write and execute the code in Mac OS. Still some compatibility issues exsit. For example, "Monitoring File Events" uses "sys/inotify.h", which is unavailable in Mac OS. So this part of code is not included.

The update is currently suspended at "fifo", because it becomes increasingly difficult to follow the video course, without first understanding linux.

Most propgrams are compiled via `gcc xxx.c`, but `threads.c` is compiled via `gcc xxx.c -pthread`.
