const express = require('express');
const readline = require('readline'); // to get data from our c++ piper.
const io = require('socket.io')();
const app = express();
app.set('view engine','ejs');
app.set('port',process.env.PORT || 3000);

const server = app.listen(app.get('port'));

const DELIMITER=" ";
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
      const wandevent = input.split(DELIMITER)[0];
      const specifier = input.split(DELIMITER)[1];

      if(wandevent === EVENTS.STATECHANGE){
      //TODO: do state change stuff here.      
      console.log(`WAND EVENT STATE CHANGE TO ${input.split(DELIMITER)[1]}`);
      io.emit(EVENTS.STATECHANGE,{state: specifier});
      }

      if (wandevent === EVENTS.SUCCESSFUL) {
      //TODO: do wandevent successful stuff here.
      console.log(`WAND Event SUCCESSFUL ${input.split(DELIMITER)[1]}`);
      io.emit(EVENTS.SUCCESSFUL,{state: specifier});
      }

});

console.log('node server listening on 127.0.0.1:3000');

io.attach(server);

app.get('/',(req,res)=>{
    res.render('main',{
    });

});
io.on('connect',(socket)=>{
});
