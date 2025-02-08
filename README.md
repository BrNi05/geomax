# Intro

GeoMax is a university project for my first semester C programming course. It is a console window coordinate geometry program with limited, yet very useful functions.

My main goal with this program was to create something more complex then required by uni, and this something should be 100% stable and mostly unbreakable. I think I achieved the set goals, and even did a little more. I created a console app that does not look awful. (I do think that at least..)<br/>
During the development, I paid attention to cross platform compatibility, so I dove deeper into the language itself in order to make the program work on POSIX and Windows environments as well.<br/>
I also experienced the stress and crunch that a close deadline can cause, and that is a valuable lesson on time management and planning.

## Basic usage

- As a console app, it has no UI for displaying shapes. It is done through SVG-s and a load/save system.
- Due to the uni project nature of the program, some parts of it had to be written in Hungarian, so the UI language and the official manual (not uploaded) is in Hungarian, while the programming is in English, but at the end of the day, that is the important part.
- Move in the menu with W, A, S, D keys and select with E. Go back in menu with K (or sometimes any 'invalid' key) when promted. Because of how C works, you have to press an ENTER after your input, but the input can be stacked (like DSE = right, down, select).
- In the path inputs, do not use SPACE characters, as it ends the reading. When not promted, include .svg at the end of the filename.
- When selecting shapes, the logic is like this: C - circle, E - line, S - segment.
- On the data panel, monotonity is described as: N - growing, C - decreasing, - - constant, | - vertical.

## Some tech details

- The viewport is scaled with circles and segments, and lines adapt to this scaling, since SVG has no line type, so lines are SVG segments from viewport edge to edge. The minimum and default viewport size is 50x50.
- The program cannot store two shapes that are the same. This is checked at shape addition and after geometric constructions. There is no ID-based or similar identification.
- With little 'cheats', horizontal and perpendicular lines/segments are processed correctly.
- The 'Save SVG now.' menu saves the current viewport into the %temp% folder and opens it in the default browser. On closing the program, these files are deleted.
- The SVG saving/loading system is a pretty smart one. It uses comments as borders of 'meaningful content', uses colors to distinguish lines from segments, ignores XY-axis when loading and so on.
- The accessing of global variables is strict to ensure stability. Basically a 'get/set' idea is used. Limited amount of global variables are used to maintain code clarity (not passing variables through dozens of functions).
  
- The code compiles with -Wall and -Werror compiler flags, which was a uni requirement.
- There is an included library called debugmalloc.h. It is a uni provided code that checks for memory leaks and out-of-bounds memory accesses by replacing the built-in malloc and free functions with it's own, tracking implementation.
- The program assumes that the "legacy console" is being used, and that way, the "UI" changes it's size and title, so I recommend using that, instead of Windows Terminal.
    - [On Win11: Settings > System > For developers > Terminal : Windows Console Host]
- The program was developed and tested on Windows platform and it compiles without errors on Linux as well, but I did no testing there. Since I own no Apple hardware, and therefore can do absolutely no testing, I disabled the execution on Apple devices with a macro.

  ## The project

  - This project was mostly used for learning purposes and to try language functions, while keeping up with the deadline and uni requirements.
  - I made it public, as it may contain useful ideas for other.
