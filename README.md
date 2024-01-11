# myVerlet

Simple and fast C++ m_physics engine oriented for handling big number of simple m_objects: circles and constraints  

Can handle 100k m_objects with 8 substeps on 60fps+  

Lowering number of substeps to 2 and pushing it's limits on 30fps+ can handle around 450k m_objects.  

Handles m_gravity, collisions, wall constraints  

Using SFML library.  

Not using GPU for m_physics calculations.  

![myVerlet_pqiWjUpu5l](https://github.com/ronikiienko/myVerlet/assets/106737540/fadf1ffc-47d6-4a30-97c9-e7726acefb3e)
![image](https://github.com/ronikiienko/myVerlet/assets/106737540/5402b812-5c53-4290-8cb6-b0d48e0ffb65)
![image](https://github.com/ronikiienko/myVerlet/assets/106737540/d5d2cdc9-ee5b-4dea-a775-57e176f7549e)

Try:
[myVerlet (3).zip](https://github.com/ronikiienko/myVerlet/files/13621621/myVerlet.3.zip)



Compiled for windows only. M - add random. Spacebar - shoot. right click - explosion. WASD - m_shooter more, Arrows - m_shooter rotate, Backspase - remove all.
Not fully showcasing possibilities, and m_window size is small meaning you can fit only around 50k without cluttering. 8 substeps.
