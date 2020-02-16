# Multi-Thread-Scheduling

Construct a simulator of an automated control system for the railway track that uses the following programming constructs provided by the posix pthread library:

1. threads
2. mutexes
3. condition variables (convars)


Each train, which will be simulated by a thread, has the following attributes:
1. Number: an integer uniquely identifying each train.
2. Direction:
 - If the direction of a train is Westbound, it starts from the East station and travels to the West station.
 - If the direction of a train is Eastbound, it starts from the West station and travels to the East station.
3. Priority: The priority of the station from which it departs.
4. Loading Time: The amount of time that it takes to load it (with goods) before it is ready to depart.
5. Crossing Time: The amount of time that the train takes to cross the main track.
