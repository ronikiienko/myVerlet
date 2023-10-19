# myVerlet

Simple and fast C++ physics engine oriented for handling big number of simple objects: circles and constraints
Can handle 100k objects with 8 substeps on 60fps+
Lowering number of substeps to 2 and pushing it's limits on 30fps+ can handle around 450k objects.
Handles gravity, collisions, wall constraints
Using SFML library.
Not using GPU for physics calculations.
