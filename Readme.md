Client:
1.	Receive info from server
	a.	Use it to render the grid
2.	Receive movement from user
	a.	Send it to the server (2)

Server:
1.	All player positions and score 
	a.	Send info to client (1)
2.	Receive player movement from client
	a.	Send it to all the clients (1)
3.	Set up position for the grid (player spawn and tomato)
4.	Synchronization to make sure players are not going to the same position

Server sends to each client:
(player1.x,player1.y,player2.x,player2.y,player3.x,player3.y,player4.x,player4.y,level,score,NumOfTomatos,playerId)

(After player moves)
Each client sends to the server:
(playerId.x, playerId.y)

