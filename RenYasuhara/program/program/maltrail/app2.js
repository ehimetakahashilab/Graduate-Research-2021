

// Express モジュールを読み込む
var express = require('express'),
	http = require('http'),
	app = express();
// index.html を表示する
app.use(express.static(__dirname + '/public'));
// 10000ポートで接続を待つ
var server = http.createServer(app).listen(10000, function() {
	console.log('接続待機中...');
});
// Socket.IO モジュールを読み込む
var io = require('socket.io'),
	io = io(server);

// 接続時の処理
io.sockets.on('connection', function(socket) {
	console.log('接続開始');
	//var i = 0;
	
	var spawn = require('child_process').spawn,
		python = spawn('sudo', ['-E', 'python3', 'sensor2.py']);
	
	python.stdout.on('data', function(data) {
		//console.log(data.toString());
		console.log('python stdout: ' + data.toString());
		socket.broadcast.emit('packet', data.toString());
		//i++;
	});
	python.stderr.on('data', function(data) {
		console.log('python stderr: ' + data);
	});
	
	
	socket.on('capture', function() {
		console.log('yahoo');
		//startCapture(socket);
	});
	socket.on('disconnect', function() {
		console.log('接続切断');
		//python.kill();
	});
		
});



function startCapture(socket) {
}
