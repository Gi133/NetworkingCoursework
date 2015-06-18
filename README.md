# NetworkingCoursework
An application making of the Angel2D engine to provide a spectator mode for a test game. 
The idea behind the spectator mode is to use a technique similar to the Snapshot technique found over at https://developer.valvesoftware.com/wiki/Source_Multiplayer_Networking

This means that every set amount of time a snapshot of the current game world is taken by the player and then broadcasted to all spectators.
The code on the spectator side takes this new world state and applies it to the current world, making adjustments to the previous one. This could mean that if a new entity exists in the new snapshot then that new entity will be created and initialised at this new location. On the other hand, if the entity in question already existed then its values will be updated to match the new world state given to the application by the application it is spectating.
Between these snapshots the game logic is the same for the spectator and player. This is an allowance due to the nature of the game in question where the player tries to avoid a ball that is flinged at them in a 2D space with only physics being calculated on the two objects. This means that there can be very little deviation between updates and what deviation there is can more often than not be caught by the next snapshot update.

# Issues:
Currently there can only be one spectator and that is running on the same local network as the player. This was done due to a mix of both inexperience as well as lack of time when this project was being developed. This can be remedied by including functionality to check for calls to a specific port for the player then adding any potential new spectators to the spectator pool. Some of this code already exists within the application but modifications need to be done in order for it to properly work.

# How do I get this to run?
Download and build, there should not be any problems and this application was last checked on Windows 7 and built using Visual Studio 2013.
