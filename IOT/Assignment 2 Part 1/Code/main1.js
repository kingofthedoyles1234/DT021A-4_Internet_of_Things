// Exercise 1
// Named Function

var fs = require('fs');

fs.readFile("index.html", writeFileContentToConsole);

function writeFileContentToConsole(err, data) {
    if (err) {
        console.log(err);
    } else {	
        console.log(data.toString());		
    }
}

