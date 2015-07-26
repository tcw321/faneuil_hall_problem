# faneuil_hall_problem

July 25, 2015

Attempt at Solutions in C++ for the Faneuil Hall Problem in The Little Book of Semaphores by Allen B. Downey Version 2.1.5

There are currently two solutions.  One is based on the solution in the book using mutexes.  The second is using state machines and atomic variables.

The mutex solution works most of the time.  It does lock up occasionally and I have not determined why yet. Sometimes the immigrants get stuck outside the building.  They can't enter because a judge entered before them.  The judge confirms the immigrants in the building then leaves.  The second set of immigrants come in and without a second judge, they just wait forever.

The state machine solution seems to work but I am still testing.  It did lock up if the judge enters before any immigrants.  The immigrants can't enter the building and the judge can't leave because she is waiting for confirmation that will never come.  I set the all confirmed flag to true until the first immigrant enters and sets the flag to false.  

The initial work was done with the Clion editor on the mac.  I plan to test it with other compilers like Visual Studio 2015.

Tim Wright
