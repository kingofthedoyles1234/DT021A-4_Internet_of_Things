// Exercise 2
// Asynchronous

const fs = require('fs');

fs.copyFile('index.html', 'indexcopy.html', (err) => {
if (err) throw err;
});
