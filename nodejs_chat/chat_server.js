var net = require('net');
var _ = require('underscore');
var Backbone = require('backbone');

var User = Backbone.Model.extend({
	defaults : {
		name : "",
		endpoint : "",
		room : "",
		socket : null
	},
	idAttribute: "endpoint"
});

var UserList = Backbone.Collection.extend({
	model : User
});

var userList = new UserList();
var roomList = ["chat", "hottub"];

// Removes whitespaces from the start and end of strings.
function trimWS (str) {
	return str.replace(/^[\s\n\r\f\t\0]+/, '').replace(/[\s\n\r\f\t\0]+$/, '');
}

// Disallow anything lower than a space and anything higher than a tilde character.
var charFilter = new RegExp("[^\u0020-\u007e]");
var roomRegex = new RegExp("^/rooms");
var joinRegex = new RegExp("^/join");
var leaveRegex = new RegExp("^/leave");
var quitRegex = new RegExp("^/quit");

var server = net.createServer();

server.on('connection', function(listenerSocket) {
	// Attach all handlers to the listener socket first.
	listenerSocket.on('data', function(data) {
		var line = trimWS(data.toString());
		if (charFilter.test(line) || line === "")
		{
			listenerSocket.write("=> ");
			return;
		}

		var userEndPt = listenerSocket.remoteAddress + ':' + listenerSocket.remotePort;
		var chatUser = userList.findWhere({endpoint : userEndPt});

		if (chatUser && chatUser.get('name') === "")
		{
			// The data that came in was the user name.
			var existingUser = userList.findWhere({name : line});

			if (existingUser)
			{
				listenerSocket.write("<= Sorry, name taken.\r\n<= Login name?\r\n");
			}
			else
			{
				var newUser = userList.set({ endpoint: userEndPt, name: line }, {add: false, remove: false, merge: true});
				listenerSocket.write("<= Welcome " + newUser.get('name') + "!\r\n");
			}
		}
		else
		{
			// Its either a chat message for the current channel or a control string.
			// control strings : /join <room>, /leave, /rooms, /quit

			if (roomRegex.test(line))
			{
				listenerSocket.write("<= Active rooms are:\r\n");
				// This is an n^2 operation. Can be optimized by maintaining a mapping of room -> userlist.
				for (var i = roomList.length - 1; i >= 0; i--) {
					var roomCount = userList.reduce(function(memo, user) { return user.get('room') == roomList[i] ? memo + 1 : memo; }, 0);
					listenerSocket.write("<= * " + roomList[i] + " ("+roomCount+")\r\n");
				};
				/*
				var hottubCount = userList.reduce(function(memo, user) { return user.get('room') == "hottub" ? memo + 1 : memo; }, 0);
				listenerSocket.write("<= * hottub ("+hottubCount+")\r\n"); */
				listenerSocket.write("<= end of list.\r\n");
			}
			else if (joinRegex.test(line))
			{
				var match = line.match(/^\/join[\s]+([\w]*)$/);
				var roomName = null;
				if (match && match[1])
					roomName = match[1];
				else
				{
					listenerSocket.write("A room by that name dosen't exist. Please use /rooms to see available rooms.\r\n");
					listenerSocket.write("=> ");
					return;
				}
				if (_.contains(roomList, roomName))
				{
					userList.set({ endpoint : userEndPt, room: roomName }, {add: false, remove: false, merge: true});
					listenerSocket.write("<= Entering room: "+roomName+"\r\n");
					userList.forEach(function(user) {
						if (user.get('room') === roomName)
						{
							// List the name of every person in the room.
							if (user.get('name') === chatUser.get('name'))
								listenerSocket.write("<= * "+user.get('name')+" (** this is you)\r\n");
							else
							{
								listenerSocket.write("<= * "+user.get('name')+"\r\n");
								// Inform everyone else that new user joined room.
								if (user.get('socket'))
								{
									user.get('socket').write("* new user joined "+ roomName + ": " + chatUser.get('name') + "\r\n");
									user.get('socket').write("<= ");
								}
							}
						}
					});
					listenerSocket.write("<= end of list.\r\n");
				}
				else
				{
					listenerSocket.write("<= A room by that name dosen't exist.\r\n");
				}
			}
			else if (leaveRegex.test(line))
			{
				var currentRoom = chatUser.get('room');
				if (currentRoom === "")
				{
					listenerSocket.write("<= You aren't inside any room. Use /quit to end your chat session.\r\n");
					listenerSocket.write("<= ");
					return;
				}
				userList.forEach(function(user) {
					if (user.get('room') === currentRoom)
					{
						// Inform everyone that user has left the room.
						if (user.get('name') === chatUser.get('name'))
							listenerSocket.write("<= * user has left chat: "+chatUser.get('name')+" (** this is you)\r\n");
						else
						{
							user.get('socket').write("* user has left chat: "+chatUser.get('name')+"\r\n");
							user.get('socket').write("<= ");
						}
					}
				});
				// put user back in lobby.
				chatUser.set('room', "");
			}
			else if (quitRegex.test(line))
			{
				listenerSocket.end("<= BYE\r\n");
				var quitter = userList.get(userEndPt);
				if (quitter)
					userList.remove(quitter);
				return;
			}
			else
			{
				// This is a chat message. Broadcast to everyone in current channel.
				listenerSocket.write("<= " + chatUser.get('name') + ": " + line + "\r\n");
				var currChannel = chatUser.get('room');
				userList.forEach(function(user) {
					if (user.get('room') === currChannel)
					{
						var userSocket = user.get('socket');
						if (userSocket !== listenerSocket)
						{
							userSocket.write(chatUser.get('name') + ": " + line + "\r\n");
							userSocket.write("<= ");
						}
					}
				});
			}
		}

		listenerSocket.write("=> ");
	});
	
	listenerSocket.on('end', function() {
		var userEndPt = listenerSocket.remoteAddress + ":" + listenerSocket.remotePort;
		var quitter = userList.get(userEndPt);
		if (quitter)
			userList.remove(quitter);
	});
	
	// Identify this client
	var endPt = listenerSocket.remoteAddress + ":" + listenerSocket.remotePort;
	var user = userList.findWhere({endpoint : endPt});
	
	if (user == null)
	{
		// New client. Insert them in list.
		var newUser = userList.add({endpoint: endPt, socket: listenerSocket});
		// Send a welcome message.
		listenerSocket.write("<= Welcome to the XYZ chat server\r\n<= Login name?\r\n");
		return;
	}
});

server.listen(23, function() { // 'listening' listener
	console.log('Chat Server Up !\r\n');
});