//Using Hive broker - dashboard available at http://www.hivemq.com/demos/websocket-client/
//Uses the Paho MQTT JS client library - http://www.eclipse.org/paho/files/jsdoc/index.html to send and receive messages using a web browser
//Example code available at https://www.hivemq.com/blog/mqtt-client-library-encyclopedia-paho-js

var x = document.getElementById("frm1");
var text1 = "";
var text2 = "";
var text3 = "";
text1 += x.elements[0].value;
text2 += x.elements[1].value;
text3 += x.elements[2].value;
var brokerURL = text1;
var port = parseInt(text2, 10);
var clientId = text3;


// Create a client instance
client = new Paho.MQTT.Client(brokerURL, port, clientId);


document.addEventListener("DOMContentLoaded", getid)
document.getElementById("connectButton").addEventListener("click", connectToBroker);
document.getElementById("publishButton").addEventListener("click", publishMessage);
document.getElementById("subscribeButton").addEventListener("click", subscribeToTopic);
document.getElementById("disconnectButton").addEventListener("click", startDisconnect)



client.onConnectionLost = onConnectionLost;
client.onMessageArrived = onMessageArrived;
 
var connectOptions = {
    onSuccess: onConnectCallback //other options available to set
};

function connectToBroker() {
    // connect the client
    client.connect(connectOptions);
    document.getElementById("connectionMessage").innerHTML += '<span>Connecting to: ' + brokerURL + ' on port: ' + port + '</span><br/>';
}

function publishMessage() {
  var y = document.getElementById("frm2");
  var text4 = "";
  text4 += y.elements[0].value;
  var checkBox = document.getElementById("publishRetain");
  // If the checkbox is checked, display the output text
  if (checkBox.checked == true){
    var publishRetain = true;
  } else {
    var publishRetain = false;
  }
  var text5 = "";
  text5 += document.getElementById("publishQoSInput").value;
  var publishQoSInput = parseInt(text5,10);
  var topicInput = text4;
  var publishPayload = document.getElementById("publishPayload").value;
  if (topicInput == "" || topicInput== null) {
    alert("Topic name field must be filled out");
    return false;
  } else {
    // connect the client
    client.publish(topicInput, publishPayload, publishQoSInput, publishRetain); //publish a message to the broker
  }
}

// called when the client connect request is successful
function onConnectCallback() {
  // Once a connection has been made, make a subscription and send a message.
  console.log("connected");
  client.publish("/DIT_IoT", "Colm_Connection Made", 0, false); //publish a message to the broker
}

// Called when the client loses its connection
function onConnectionLost(responseObject) {
  document.getElementById("connectionMessage").innerHTML += '<span>ERROR: Connection lost</span><br/>';
  if (responseObject.errorCode !== 0) {
      document.getElementById("connectionMessage").innerHTML += '<span>ERROR: ' + + responseObject.errorMessage + '</span><br/>';
  }
}

function getid(){
  var str1 = "clientID-";
  var str2 = (makeid());
  var clientIdInput = str1.concat(str2);
  document.getElementById("clientIdInput").value += clientIdInput;
}

function makeid() {
  var text = "";
  var possible = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

  for (var i = 0; i < 10; i++)
      text += possible.charAt(Math.floor(Math.random() * possible.length));

  return text;
}

function subscribeToTopic(){
  var z = document.getElementById("frm3");
  var text6 = "";
  text6 += z.elements[0].value;
  var text7 = "";
  text7 += document.getElementById("QoSInput").value;
  var QoSInput = parseInt(text7,10);
  var topicInputSubscribe = text6;
  var subscribeOptions = {
    qos: QoSInput
  }
  if (topicInputSubscribe == "" || topicInputSubscribe== null) {
    alert("Topic name field must be filled out");
    return false;
  } else {
    // connect the client
    client.subscribe(topicInputSubscribe, subscribeOptions);
    document.getElementById("subscriptions").innerHTML += '<span>Subscription: ' + topicInputSubscribe + '</span><br/>';
  }
}

// Called when a message arrives
function onMessageArrived(message) {
  console.log("onMessageArrived: " + message.payloadString);
  document.getElementById("messages").innerHTML += '<span>Topic: ' + message.destinationName + '  |  Messages: ' + message.payloadString + '</span><br/>';
}

// Called when the disconnection button is pressed
function startDisconnect() {
  client.disconnect();
  document.getElementById("connectionMessage").innerHTML += '<span>Disconnected</span><br/>';
}

// Updates #messages div to auto-scroll
function updateScroll() {
  var element = document.getElementById("messages");
  element.scrollTop = element.scrollHeight;
}

// Updates #messages div to auto-scroll
function updateScroll() {
  var element = document.getElementById("connectionMessage");
  element.scrollTop = element.scrollHeight;
}

// Updates #messages div to auto-scroll
function updateScroll() {
  var element = document.getElementById("subscriptions");
  element.scrollTop = element.scrollHeight;
}