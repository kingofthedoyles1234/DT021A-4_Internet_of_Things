// Exercise 2
// Synchronous

const fs = require('fs');

fs.copyFileSync("index.html", "indexcopy.html");

console.log("index.html copied to indexcopysync.html");
