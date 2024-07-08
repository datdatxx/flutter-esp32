const express = require('express')
const app = express()

var server = require("http").Server(app);

var io = require("socket.io").listen(server);

var myport = process.env.PORT || 3000;
server.listen(myport, function () {
    console.log('Server listening at: %d', myport);
});

io.on('connection', function(socket) {
    console.log('Có người truy cập id: ' + socket.id);

    // Handle disconnection
    socket.on('disconnect', function() {
        console.log(socket.id + ' ngắt kết nối server!!!!');
    });

    socket.on('flutter', function(data) {
        console.log('AppFlutter id: ' + socket.id + ' Sendata: ' + data);
        io.emit('ESP', data);
    });

    socket.on('ESPJSON', function(data) {
        const dataJson = JSON.stringify(data);
        console.log('Data ESP JSON: ' + dataJson);
        io.emit('app', dataJson);
    });
});




