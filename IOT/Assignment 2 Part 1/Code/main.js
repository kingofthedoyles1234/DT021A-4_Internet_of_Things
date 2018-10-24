// Exercise 1
// Anonymous Function

var fs = require('fs');

fs.readFile("index.html", function(err, data){
    if (err) return console.error(err);	
    console.log(data.toString());		
});

