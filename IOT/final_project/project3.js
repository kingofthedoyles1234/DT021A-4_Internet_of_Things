var noble = require('noble');
var mqtt    = require('mqtt');
var client  = mqtt.connect('mqtt://broker.mqttdashboard.com');
var client2 = mqtt.connect('mqtt://broker.mqttdashboard.com');
var client3 = mqtt.connect('mqtt://broker.mqttdashboard.com');
var client4 = mqtt.connect('mqtt://broker.mqttdashboard.com');
var MongoClient = require('mongodb').MongoClient;
var url = "mongodb://doyle_colm_dit:ditpassword18@iot-shard-00-00-okfu5.mongodb.net:27017,iot-shard-00-01-okfu5.mongodb.net:27017,iot-shard-00-02-okfu5.mongodb.net:27017/sensor_readings?ssl=true&replicaSet=IOT-shard-0&authSource=admin&retryWrites=true";



// Global Variables
const deviceUUID = "e3a0450b76a5";  // BBC Microbit
var peripheralGlobal;
var deviceServices;
var accelerometerService;
var LEDService;
var magnetometerService;
var accelerometerCharacteristics;
var LEDCharacteristics;
var magnetometerCharacteristics;
var sensorLevelData;
var sensorLevelDataX;
var sensorLevelDataY;
var sensorLevelDataZ;
var sensorMagLevelDataX;
var sensorMagLevelDataY;
var sensorMagLevelDataZ;
var database;
var collection;


// MQTT Events:
client.on('connect', connectMQTTCallback); //when a 'connect' event is received call the connectCallback listener function
client2.on('connect', connectMQTTCallback2); //when a 'connect' event is received call the connectCallback listener function
client3.on('connect', connectMQTTCallback3); //when a 'connect' event is received call the connectCallback listener function
client4.on('connect', connectMQTTCallback4); //when a 'connect' event is received call the connectCallback listener function

// Mongo Events:
MongoClient.connect(url, {useNewUrlParser: true }, mongoConnect);

// Noble Events
noble.on('stateChange', stateChange); 
noble.on('scanStart', scanStart);
noble.on('scanStop', scanStop);
noble.on('discover', onDiscovery);


function connectMQTTCallback() {
  client.subscribe('COLM_TOPIC/Read');
}

function connectMQTTCallback2() {
  client2.subscribe('COLM_TOPIC/LED');
}

function connectMQTTCallback3() {
  client3.subscribe('COLM_TOPIC/Accelerometer');
}

function connectMQTTCallback4() {
  client4.subscribe('COLM_TOPIC/Magnetometer');
}

function mongoConnect(err, db) {
  if (err) throw err;
  database = db.db("sensor_readings");
  console.log("Database Created");
}

function stateChange(state) {
  if (state!="poweredOn") return;
  console.log("Starting scan...");
  noble.startScanning([], true);
}

function scanStart() { 
	console.log("Scanning started."); 
}

function scanStop () { 
	console.log("Scanning stopped.");
}

function onDiscovery(peripheral) {
	if (peripheral.uuid == deviceUUID) {
		peripheralGlobal = peripheral;
		console.log('Found BBC Microbit device with local name: ' + peripheralGlobal.advertisement.localName);
		console.log("Peripheral UUID: " + peripheralGlobal.uuid);
		peripheralGlobal.connect(connectCallback);
	};
}

// This will be executed when the connect request returns
function connectCallback(error) {
	if (error) {
		console.log("Error connecting to peripheral");
	} else {		
		console.log('Connected to peripheral: ' + peripheralGlobal.uuid  + "   " + peripheralGlobal.advertisement.localName);
		// Call the discoverServices function and when it returns the callback function discoverServicesCallback will be executed
		peripheralGlobal.discoverServices([], discoverServicesCallback); 
	}
}

function discoverServicesCallback(error, services) { //this will be executed when the discoverServices request returns
	if (error) {
		console.log("Error discovering services");
	} else {
		console.log("The BBC Microbit device contains the following services");
		deviceServices = services;	
		for (var i in deviceServices) {
			console.log('  ' + i + ' UUID: ' + deviceServices[i].uuid);
		}
		accelerometerService = deviceServices[3];
		LEDService = deviceServices[2];
		magnetometerService = deviceServices[4];
		client.on('message', accessSensorTopic);
		client2.on('message', accessLEDTopic);
	}
}


function accessSensorTopic (topic, message) {
	if (message == "Accelerometer"){
		accelerometerService.discoverCharacteristics(null, discoverAccelerometerCharsCallback);
	} else if (message == "Magnetometer"){
		magnetometerService.discoverCharacteristics(null, discoverMagnetometerCharsCallback);
	}
}

function discoverAccelerometerCharsCallback(error, characteristics) { //this will be executed when the discoverCharacteristics request returns
	if (error) {
		console.log("Error discovering characteristics");
	} else {
		accelerometerCharacteristics = characteristics;
		console.log("The accelerometer has the following characteristics");
		for (var i in accelerometerCharacteristics) {
			console.log('  ' + i + ' UUID: ' + accelerometerCharacteristics[i].uuid + ' ');
		}
		publishAccelerometerValues();
	}
}

function discoverMagnetometerCharsCallback(error, characteristics) { //this will be executed when the discoverCharacteristics request returns
	if (error) {
		console.log("Error discovering characteristics");
	} else {
		magnetometerCharacteristics = characteristics;
		console.log("The magnetometer has the following characteristics");
		for (var i in accelerometerCharacteristics) {
			console.log('  ' + i + ' UUID: ' + magnetometerCharacteristics[i].uuid + ' ');
		}
		publishMagnetometerValues();
	}
}

function publishAccelerometerValues(){
	sensorLevelDataX = accelerometerCharacteristics[0];
	sensorLevelDataY = accelerometerCharacteristics[1];
	sensorLevelDataZ = accelerometerCharacteristics[2];
	sensorLevelDataX.read(readAcceleromterXDataCallback);
	sensorLevelDataY.read(readAcceleromterYDataCallback);
	sensorLevelDataZ.read(readAcceleromterZDataCallback);
}

function publishMagnetometerValues(){
	sensorMagLevelDataX = magnetometerCharacteristics[0];
	sensorMagLevelDataY = magnetometerCharacteristics[1];
	sensorMagLevelDataZ = magnetometerCharacteristics[2];
	sensorMagLevelDataX.read(readMagnetometerXDataCallback);
	sensorMagLevelDataY.read(readMagnetometerYDataCallback);
	sensorMagLevelDataZ.read(readMagnetometerZDataCallback);	
}

// This will be executed when the read request returns
function readAcceleromterXDataCallback(error, data) {
	if (error) {
		console.log("Error reading data");
	} else {
		var xplane = "x";
		// when a 'connect' event is received call the connectCallback listener function
		client.publish('COLM_TOPIC/Accelerometer', "Accelerometer X-Axis: " + data.toString('hex'), publishAcceleromterCallback);
		insertAccelerometerMessageInDB(xplane, data);
	}
}

// This will be executed when the read request returns
function readAcceleromterYDataCallback(error, data) {
	if (error) {
		console.log("Error reading data");
	} else {
		var yplane = "y";
		// when a 'connect' event is received call the connectCallback listener function
		client.publish('COLM_TOPIC/Accelerometer', "Accelerometer Y-Axis: " + data.toString('hex'), publishAcceleromterCallback);
		insertAccelerometerMessageInDB(yplane,data);
	}
}

// This will be executed when the read request returns
function readAcceleromterZDataCallback(error, data) {
	if (error) {
		console.log("Error reading data");
	} else {
		var zplane = "z";
		// when a 'connect' event is received call the connectCallback listener function
		client.publish('COLM_TOPIC/Accelerometer', "Accelerometer Z-Axis: " + data.toString('hex'), publishAcceleromterCallback);
		insertAccelerometerMessageInDB(zplane, data);
	}
}

// This will be executed when the read request returns
function readMagnetometerXDataCallback(error, data) {
	if (error) {
		console.log("Error reading data");
	} else {
		var xMagPlane = "x";
		// when a 'connect' event is received call the connectCallback listener function
		client.publish('COLM_TOPIC/Magnetometer', "Magnetometer X-Axis: " + data.toString('hex'), publishMagnetometerCallback);
		insertMagnetometerMessageInDB(xMagPlane, data);
	}
}

// This will be executed when the read request returns
function readMagnetometerYDataCallback(error, data) {
	if (error) {
		console.log("Error reading data");
	} else {
		var yMagPlane = "y";
		// when a 'connect' event is received call the connectCallback listener function
		client.publish('COLM_TOPIC/Magnetometer', "Magnetometer Y-Axis: " + data.toString('hex'), publishMagnetometerCallback);
		insertMagnetometerMessageInDB(yMagPlane,data);
	}
}

// This will be executed when the read request returns
function readMagnetometerZDataCallback(error, data) {
	if (error) {
		console.log("Error reading data");
	} else {
		var zMagPlane = "z";
		// when a 'connect' event is received call the connectCallback listener function
		client.publish('COLM_TOPIC/Magnetometer', "Magnetometer Z-Axis: " + data.toString('hex'), publishMagnetometerCallback);
		insertMagnetometerMessageInDB(zMagPlane,data);
	}
}

// This will be executed when the publish request returns
function publishAcceleromterCallback(error) {     
   	if (error) {
		console.log("Error publishing data");
	} else {	 
        console.log("Accelerometer Reading has been published");
    }
}

// This will be executed when the publish request returns
function publishMagnetometerCallback(error) {     
   	if (error) {
		console.log("Error publishing data");
	} else {	 
        console.log("Magnetometer Reading has been published");
    }
}

function insertAccelerometerMessageInDB(plane, data) {
	var fromMqttObj = { Sensor: "Accelerometer", Axis: plane, Value:data.toString('hex'), Date:new Date() };
	database.collection("accelerometer").insertOne(fromMqttObj, function(err, res) {
		if (err) throw err;
		console.log("1 document inserted");
	});
}

function insertMagnetometerMessageInDB(plane, data) {
	var fromMqttObj = { Sensor: "Magnetometer", Axis: plane, Value:data.toString('hex'), Date:new Date() };
	database.collection("magnetometer").insertOne(fromMqttObj, function(err, res) {
		if (err) throw err;
		console.log("1 document inserted");
	});
}


function accessLEDTopic (topic, message) {
	if (message == 1){
		LEDService.discoverCharacteristics(null, function (error, characteristics) {
			LEDCharacteristics = characteristics;
			sensorLevelData = LEDCharacteristics[0];
			console.log("The LED will turn on as a " + message.toString() + " will be written to the LED.");
			sensorLevelData.write(new Buffer([message]),false, writeDataCallback);
		});
	} else if (message == 0){
		LEDService.discoverCharacteristics(null, function (error, characteristics) {
			LEDCharacteristics = characteristics;
			sensorLevelData = LEDCharacteristics[0];
			console.log("The LED will turn off as a " + message.toString() + " will be written to the LED.");
			sensorLevelData.write(new Buffer([message]),false, writeDataCallback);
		});
	} else {
		console.log("Invalid Value");
	}
}


function writeDataCallback(error) { //this will be executed when the read request returns
	if (error) {
		console.log("Error reading data");
	} else {	
		console.log("The LED value has been written to.");
		var standard_input = process.stdin;
		standard_input.setEncoding('utf-8');
		console.log("If you are finished setting LED type 'exit'");
		standard_input.on('data', function (data) {
			if(data === 'exit\n'){
				// Program exit.
				console.log("User input complete, program exit.");
				client.end();
				client2.end();
				client3.end();
				client4.end();
				peripheralGlobal.disconnect(disconnectCallback);
			}
		});
	}
}

function disconnectCallback(){
	console.log('Disconnected from Device');
	process.exit();
}

