const express = require('express');
const readline = require('readline'); // to get data from our c++ piper.
const io = require('socket.io')();
const app = express();
app.set('view engine','ejs');
app.set('port',process.env.PORT || 3000);

const server = app.listen(app.get('port'));

const EVENTS = {
    STATECHANGE: 'STATECHANGE', // STATE TREE state change
    SUCCESSFUL: 'SUCCESSFUL',   // WHEN WE SUCCESSFULLY casted a spell.
};
const rl = readline.createInterface({
      input: process.stdin,
});


rl.on('line', (input) => {
      console.log(`Received: ${input}`);
      // PARSE line to get what happened.
      // UPDATE our viewers here.
});

console.log('node server listening on 127.0.0.1:3000');

io.attach(server);


io.on('connect',(socket)=>{
});
