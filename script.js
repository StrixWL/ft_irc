const { Socket } = require('net')

let arr = []
let i = 0;
const f = async () => new Promise(r => {
	if (arr.length > 15) {
		r();
		return ;
	}
	const socket = new Socket();
	socket.logged = false;
	socket.connect(6667, "127.0.0.1", () => {
		console.log("connected!");
		socket.write("NICK mok" + i++ + "\r\n");
		socket.write("USER o o o o\r\n");
		socket.write("PASS 123\r\n");
		socket.write("JOIN #myChannel\r\n");
		socket.logged = true;
	});
	arr.push(socket);
	setTimeout(r, 10);
}) 
let k = 0;
const init = async () => {
	while (1) {
		for (const socket of arr) {
			if (socket.logged) {
				await new Promise(r => setTimeout(r, 3))
				socket.write(`PRIVMSG #myChannel :!downvote <Topic ${k++}> slm hh\r\n`)
			}
		}
		await f()
		// console.log("connected clients:", arr.length);
	}
}

init()

// import { io } from 'socket.io-client'

// io("wss://127.0.0.1:6667")