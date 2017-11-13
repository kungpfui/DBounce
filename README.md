DBounce
=======
Fast, low resource, configurable, digital debouncing library for Arduino.

Intention
---------
When you start with an Arduino board you get immediately to the point where you have to debounce some switches and buttons. There are debouncing libraries for Arduino like [Bounce2] by Thomas Fredericks. They work but they are slow and need a lot of memory and endless lines of configuration code when you have a lot of buttons to debounce.

I thought that I can do it better. The implementations I saw looked rather like traditional C implementations. So, why not using the power of a C++ template class?

Well C++ templates are powerful but not all of my ideas could be implemented the way I wanted (the C++ standard can still be improved), but take a look by yourself.


Documentation
=============
... work in progress. I made already some nice scope picture which show the debounce state very nicely.   

How to use
----------
Watch out the *examples* folder and comments in the header file.



[Bounce2]: https://github.com/thomasfredericks/Bounce2