const express = require('express');
const ejs = require('ejs');
const readline = require('readline'); // to get data from our c++ piper.
const io = require('socket.io')();
const app = express();
const path = require('path');
app.set('view engine','ejs');
app.set('views', path.join(__dirname, '/views'));
app.use("/public", express.static(path.join(__dirname, 'public')));
app.set('port',process.env.PORT || 3000);

const server = app.listen(app.get('port'));

const DELIMITER=" ";

const EVENTS = {
    STATECHANGE: 'STATECHANGE', // STATE TREE state change
    SUCCESSFUL: 'SUCCESSFUL',   // WHEN WE SUCCESSFULLY casted a spell.
    DATA: 'DATA',
};

const rl = readline.createInterface({
      input: process.stdin,
});

rl.on('line', (input) => {
      console.log(`Received: ${input}`);
        let data = input.split()
      if (data.length === 3){
          io.emit(EVENTS.DATA, {
              x: data[0],
              y: data[1],
              z: data[2],
          }); 
      }
      // PARSE line to get what happened.
      // UPDATE our viewers here.
      const wandevent = input.split(DELIMITER)[0];
      const specifier = input.split(DELIMITER)[1];

      if(wandevent === EVENTS.STATECHANGE){
      //TODO: do state change stuff here.      
      console.log(`NODE_SERVER: WAND EVENT STATE CHANGE TO ${input.split(DELIMITER)[1]}`);
      io.emit(EVENTS.STATECHANGE,{state: specifier});
      }

      if (wandevent === EVENTS.SUCCESSFUL) {
      //TODO: do wandevent successful stuff here.
      console.log(`NODE_SERVER: WAND Event SUCCESSFUL ${input.split(DELIMITER)[1]}`);
      io.emit(EVENTS.SUCCESSFUL,{state: specifier});
      }

});

console.log('node server listening on 127.0.0.1:3000');

io.attach(server);

app.get('/',(req,res)=>{
    res.render('main',{
    });
});

//Socket.io events for emiting x, y and z cordinates
io.on('connection',(socket)=>{
});

setInterval(() => { 
    io.emit(EVENTS.SUCCESSFUL, { state: "IMPOSTRIUS" });
    // io.emit(EVENTS.DATA, {
    //     x: Math.random(1)*2000,
    //     y: Math.random(1)*2000,
    //     z: Math.random(1)*2000,
    // });
    console.log("sent");
}, 4000);

