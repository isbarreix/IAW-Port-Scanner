  /*
    variable de conexion con socket.
  */
  var socket = new WebSocket('ws://127.0.0.1:4567');

  /*
    Establecer conexion con socket
  */
  socket.onopen = function () {
      console.log("Ready to go");
      console.log(socket.readyState);  
  };

  /*
    Si llega un mensaje del servidor
  */
  socket.onmessage = function (event) {
      console.log(event.data);
      var obj = JSON.parse(event.data);
      console.log(obj);

  };

  /*
    Si se cierra conexion con el server
  */
  socket.onclose = function (event) {
      console.log("WebSocket is closed now.");
  };

  function scan() {
      socket.send('MSG_TEST: Hello server');
  }