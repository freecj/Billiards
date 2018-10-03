# billiards
My final project is a billiards game. One is a typical pool game of 16-ball. The
other is a bumper billiards.
My implementations are about the texture mapping of all things, ball movement,
camera view and so on. For the texture mapping, I focus on the room, lamp, pool
table, ball, stick. For the ball movement, I deal with the rolling effect, ball-table
collision, ball-ball collision and hitting of ball and pockets. I have 2 camera
transformations. One is the normal one, you can change by parameters. The other
is following the vision of the ball you hit. I have also draw the stick and the set all
the rules for the game. If the cue ball is in the hole, it will return to his previous
state. Most geometry is created using sphere, cube, and cylinder primitives. Ball
shadows are done by drawing grey disks below them. Table holes are done by
drawing black disks on the table.
For the bumper billiards. Each player gets 5 balls. The table has two pockets and
twelve bumpers. And it has all the rules for that game. First, if a player sinks one
of his own balls (but not the last) in their opponent's pocket, the opponent may
drop two balls into their pocket. Secondly, avoid knocking you opponent's balls off
of the table. If you knock an opponent's ball off of the table, replace it in the
position it was before the shot and place 1 of your own balls in the dead center of
the table. Play passes to your opponent.
And I have a special strike. If the cue ball hits any other balls, all balls disappear
and become lots of particles.
My video link is https://www.youtube.com/watch?v=KH3U7l0Dozw
