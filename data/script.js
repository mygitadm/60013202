var ws = new WebSocket('ws://' + location.hostname + ':80/ws');

ws.onopen = function() {
    console.log('Connected');
};

ws.onmessage = function(evt) {
    var data = JSON.parse(evt.data);
    document.getElementById('timestamp').innerHTML = data.timestamp;
    document.getElementById('temp1').innerHTML = data.solutionTemp;
    document.getElementById('temp2').innerHTML = data.substrateTemp;
    document.getElementById('soilMoisture').innerHTML = data.substrateMoisture;
};

ws.onclose = function() {
    console.log('Disconnected');
};