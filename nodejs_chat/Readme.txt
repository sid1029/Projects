My first nodejs app. A simple TCP based chat server.

1. You must have nodejs installed. To install on ubuntu :
	$> sudo apt-get insall nodejs
	
2. Copy chat_server.js file to a location of your choice. (say /home/Documents/chatserver)

3. 	$> cd /home/Documents/chatserver

4. Install the two dependencies using npm (installs itself with nodejs):
	$> npm install underscore
	$> npm install backbone
	
6. Run the server. uses port 23 for telnet by default:
	$> nodejs telnetServer.js

7. Connect to server using telnet :
	$> telnet localhost

8. Following commands supported :
	/rooms - List all available rooms. (To add more rooms edit line 20)
	/join <room-name> - Joins the chat room.
	/leave - Leave room.
	/quit - Close session and leave the chat server.